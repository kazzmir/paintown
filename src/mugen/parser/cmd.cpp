

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

Ast::Value * makeFunction(const std::string & name, const Value & arg1){
    Ast::Value * function = new Ast::Function(name, as<Ast::ValueList*>(arg1));
    GC::save(function);
    return function;
}

Ast::Value * makeFunction1(const Value & name, const Value & arg1){
    return makeFunction(name, makeValueList(arg1, Value()));
}

std::string combine(const char * name, const Value & what){
    if (what.getValue() != 0){
        std::ostringstream out;
        out << name << as<Ast::Number*>(what)->toString();
        return out.str();
    } else {
        return std::string(name);
    }
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

/* for commands with no keys listed */
Ast::Key * makeEmptyKeyList(){
    std::vector<Ast::Key*> all;
    Ast::Key * object = new Ast::KeyList(all);
    GC::save(object);
    return object;
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
    
    RuleTrace trace_peg_135(stream, "start");
    int myposition = position;
    
    try{
        
    
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
                    
                        {
                                
                                result_peg_16.reset();
                                do{
                                    Result result_peg_48(result_peg_16.getPosition());
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar(" "[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_59;
                                                }
                                            }
                                            result_peg_48.setValue((void*) " ");
                                                
                                        }
                                        goto success_peg_57;
                                        out_peg_59:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\t"[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_61;
                                                }
                                            }
                                            result_peg_48.setValue((void*) "\t");
                                                
                                        }
                                        goto success_peg_57;
                                        out_peg_61:
                                        goto out_peg_56;
                                        success_peg_57:
                                        ;
                                        
                                    }
                                    goto success_peg_49;
                                    out_peg_56:
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_71;
                                                }
                                            }
                                            result_peg_48.setValue((void*) "\n");
                                                
                                        }
                                        goto success_peg_69;
                                        out_peg_71:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_73;
                                                }
                                            }
                                            result_peg_48.setValue((void*) "\r");
                                                
                                        }
                                        goto success_peg_69;
                                        out_peg_73:
                                        goto out_peg_68;
                                        success_peg_69:
                                        ;
                                        
                                    }
                                    goto success_peg_49;
                                    out_peg_68:
                                    goto loop_peg_47;
                                    success_peg_49:
                                    ;
                                    result_peg_16.addResult(result_peg_48);
                                } while (true);
                                loop_peg_47:
                                ;
                                        
                            }
                            goto success_peg_18;
                            goto loop_peg_15;
                            success_peg_18:
                            ;
                        
                        
                        
                        result_peg_16 = rule_line(stream, result_peg_16.getPosition(), current);
                            if (result_peg_16.error()){
                                goto loop_peg_15;
                            }
                        
                        
                        
                        result_peg_16 = rule_whitespace(stream, result_peg_16.getPosition());
                            if (result_peg_16.error()){
                                goto loop_peg_15;
                            }
                        
                        
                        
                        int save_peg_76 = result_peg_16.getPosition();
                            
                            result_peg_16 = rule_line_end(stream, result_peg_16.getPosition());
                            if (result_peg_16.error()){
                                
                                result_peg_16 = Result(save_peg_76);
                                result_peg_16.setValue((void*) 0);
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_16);
                } while (true);
                loop_peg_15:
                ;
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_108(result_peg_2.getPosition());
                        {
                            
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(" "[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_119;
                                    }
                                }
                                result_peg_108.setValue((void*) " ");
                                    
                            }
                            goto success_peg_117;
                            out_peg_119:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\t"[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_121;
                                    }
                                }
                                result_peg_108.setValue((void*) "\t");
                                    
                            }
                            goto success_peg_117;
                            out_peg_121:
                            goto out_peg_116;
                            success_peg_117:
                            ;
                            
                        }
                        goto success_peg_109;
                        out_peg_116:
                        {
                            
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_131;
                                    }
                                }
                                result_peg_108.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_129;
                            out_peg_131:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_133;
                                    }
                                }
                                result_peg_108.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_129;
                            out_peg_133:
                            goto out_peg_128;
                            success_peg_129:
                            ;
                            
                        }
                        goto success_peg_109;
                        out_peg_128:
                        goto loop_peg_107;
                        success_peg_109:
                        ;
                        result_peg_2.addResult(result_peg_108);
                    } while (true);
                    loop_peg_107:
                    ;
                            
                }
                goto success_peg_78;
                goto out_peg_5;
                success_peg_78:
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
        
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    RuleTrace trace_peg_202(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_137(myposition);
        
        {
        
            {
                    
                    result_peg_137.reset();
                    do{
                        Result result_peg_149(result_peg_137.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_149.getPosition()))){
                                    result_peg_149.nextPosition();
                                } else {
                                    goto out_peg_152;
                                }
                            }
                            result_peg_149.setValue((void*) " ");
                                
                        }
                        goto success_peg_150;
                        out_peg_152:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_149.getPosition()))){
                                    result_peg_149.nextPosition();
                                } else {
                                    goto out_peg_154;
                                }
                            }
                            result_peg_149.setValue((void*) "\t");
                                
                        }
                        goto success_peg_150;
                        out_peg_154:
                        goto loop_peg_148;
                        success_peg_150:
                        ;
                        result_peg_137.addResult(result_peg_149);
                    } while (true);
                    loop_peg_148:
                    ;
                            
                }
                goto success_peg_139;
                goto out_peg_155;
                success_peg_139:
                ;
            
            
            
            result_peg_137 = rule_comment(stream, result_peg_137.getPosition());
                if (result_peg_137.error()){
                    goto out_peg_155;
                }
            
            
            
            Result result_peg_157(result_peg_137.getPosition());
                result_peg_157 = rule_line_end(stream, result_peg_157.getPosition());
                if (result_peg_157.error()){
                    goto out_peg_155;
                }
            
            
        }
        stream.update(result_peg_137.getPosition());
        
        return result_peg_137;
        out_peg_155:
        Result result_peg_158(myposition);
        
        {
        
            {
                    
                    result_peg_158.reset();
                    do{
                        Result result_peg_170(result_peg_158.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_170.getPosition()))){
                                    result_peg_170.nextPosition();
                                } else {
                                    goto out_peg_173;
                                }
                            }
                            result_peg_170.setValue((void*) " ");
                                
                        }
                        goto success_peg_171;
                        out_peg_173:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_170.getPosition()))){
                                    result_peg_170.nextPosition();
                                } else {
                                    goto out_peg_175;
                                }
                            }
                            result_peg_170.setValue((void*) "\t");
                                
                        }
                        goto success_peg_171;
                        out_peg_175:
                        goto loop_peg_169;
                        success_peg_171:
                        ;
                        result_peg_158.addResult(result_peg_170);
                    } while (true);
                    loop_peg_169:
                    ;
                            
                }
                goto success_peg_160;
                goto out_peg_176;
                success_peg_160:
                ;
            
            
            
            result_peg_158 = rule_section(stream, result_peg_158.getPosition());
                if (result_peg_158.error()){
                    goto out_peg_176;
                }
            
            Result result_peg_177 = result_peg_158;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_177.getValues());
                    result_peg_158.setValue(value);
                }
            
            
        }
        stream.update(result_peg_158.getPosition());
        
        return result_peg_158;
        out_peg_176:
        Result result_peg_178(myposition);
        
        {
        
            {
                    
                    result_peg_178.reset();
                    do{
                        Result result_peg_190(result_peg_178.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_190.getPosition()))){
                                    result_peg_190.nextPosition();
                                } else {
                                    goto out_peg_193;
                                }
                            }
                            result_peg_190.setValue((void*) " ");
                                
                        }
                        goto success_peg_191;
                        out_peg_193:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_190.getPosition()))){
                                    result_peg_190.nextPosition();
                                } else {
                                    goto out_peg_195;
                                }
                            }
                            result_peg_190.setValue((void*) "\t");
                                
                        }
                        goto success_peg_191;
                        out_peg_195:
                        goto loop_peg_189;
                        success_peg_191:
                        ;
                        result_peg_178.addResult(result_peg_190);
                    } while (true);
                    loop_peg_189:
                    ;
                            
                }
                goto success_peg_180;
                goto out_peg_196;
                success_peg_180:
                ;
            
            
            
            {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_178.getPosition()))){
                            result_peg_178.nextPosition();
                        } else {
                            goto out_peg_199;
                        }
                    }
                    result_peg_178.setValue((void*) "\n");
                        
                }
                goto success_peg_197;
                out_peg_199:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_178.getPosition()))){
                            result_peg_178.nextPosition();
                        } else {
                            goto out_peg_201;
                        }
                    }
                    result_peg_178.setValue((void*) "\r");
                        
                }
                goto success_peg_197;
                out_peg_201:
                goto out_peg_196;
                success_peg_197:
                ;
            
            
        }
        stream.update(result_peg_178.getPosition());
        
        return result_peg_178;
        out_peg_196:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_203 = stream.getColumn(position);
    if (column_peg_203.chunk0 != 0 && column_peg_203.chunk0->chunk_line_end.calculated()){
        return column_peg_203.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_217(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_204(myposition);
        
        result_peg_204.reset();
        do{
            Result result_peg_206(result_peg_204.getPosition());
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_206.getPosition()))){
                        result_peg_206.nextPosition();
                    } else {
                        goto out_peg_209;
                    }
                }
                result_peg_206.setValue((void*) "\n");
                    
            }
            goto success_peg_207;
            out_peg_209:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_206.getPosition()))){
                        result_peg_206.nextPosition();
                    } else {
                        goto out_peg_211;
                    }
                }
                result_peg_206.setValue((void*) "\r");
                    
            }
            goto success_peg_207;
            out_peg_211:
            goto loop_peg_205;
            success_peg_207:
            ;
            result_peg_204.addResult(result_peg_206);
        } while (true);
        loop_peg_205:
        if (result_peg_204.matches() == 0){
            goto out_peg_212;
        }
        
        if (column_peg_203.chunk0 == 0){
            column_peg_203.chunk0 = new Chunk0();
        }
        column_peg_203.chunk0->chunk_line_end = result_peg_204;
        stream.update(result_peg_204.getPosition());
        
        
        return result_peg_204;
        out_peg_212:
        Result result_peg_213(myposition);
        
        {
        
            Result result_peg_215(result_peg_213.getPosition());
                if ('\0' == stream.get(result_peg_215.getPosition())){
                    result_peg_215.nextPosition();
                    result_peg_215.setValue((void *) '\0');
                } else {
                    goto out_peg_216;
                }
            
            
            
            
            
            
        }
        
        if (column_peg_203.chunk0 == 0){
            column_peg_203.chunk0 = new Chunk0();
        }
        column_peg_203.chunk0->chunk_line_end = result_peg_213;
        stream.update(result_peg_213.getPosition());
        
        
        return result_peg_213;
        out_peg_216:
    
        if (column_peg_203.chunk0 == 0){
            column_peg_203.chunk0 = new Chunk0();
        }
        column_peg_203.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_218 = stream.getColumn(position);
    if (column_peg_218.chunk0 != 0 && column_peg_218.chunk0->chunk_whitespace.calculated()){
        return column_peg_218.chunk0->chunk_whitespace;
    }
    
    RuleTrace trace_peg_222(stream, "whitespace");
    int myposition = position;
    
    
    
    Result result_peg_219(myposition);
        
        result_peg_219.reset();
        do{
            Result result_peg_221(result_peg_219.getPosition());
            result_peg_221 = rule_sw(stream, result_peg_221.getPosition());
            if (result_peg_221.error()){
                goto loop_peg_220;
            }
            result_peg_219.addResult(result_peg_221);
        } while (true);
        loop_peg_220:
        ;
        
        if (column_peg_218.chunk0 == 0){
            column_peg_218.chunk0 = new Chunk0();
        }
        column_peg_218.chunk0->chunk_whitespace = result_peg_219;
        stream.update(result_peg_219.getPosition());
        
        
        return result_peg_219;
    
        if (column_peg_218.chunk0 == 0){
            column_peg_218.chunk0 = new Chunk0();
        }
        column_peg_218.chunk0->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_223 = stream.getColumn(position);
    if (column_peg_223.chunk0 != 0 && column_peg_223.chunk0->chunk_sw.calculated()){
        return column_peg_223.chunk0->chunk_sw;
    }
    
    RuleTrace trace_peg_233(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_224(myposition);
        
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
        goto out_peg_230;
        success_peg_225:
        ;
        
        if (column_peg_223.chunk0 == 0){
            column_peg_223.chunk0 = new Chunk0();
        }
        column_peg_223.chunk0->chunk_sw = result_peg_224;
        stream.update(result_peg_224.getPosition());
        
        
        return result_peg_224;
        out_peg_230:
        Result result_peg_231(myposition);
        
        result_peg_231 = rule_comment(stream, result_peg_231.getPosition());
        if (result_peg_231.error()){
            goto out_peg_232;
        }
        
        if (column_peg_223.chunk0 == 0){
            column_peg_223.chunk0 = new Chunk0();
        }
        column_peg_223.chunk0->chunk_sw = result_peg_231;
        stream.update(result_peg_231.getPosition());
        
        
        return result_peg_231;
        out_peg_232:
    
        if (column_peg_223.chunk0 == 0){
            column_peg_223.chunk0 = new Chunk0();
        }
        column_peg_223.chunk0->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_234 = stream.getColumn(position);
    if (column_peg_234.chunk1 != 0 && column_peg_234.chunk1->chunk_comment.calculated()){
        return column_peg_234.chunk1->chunk_comment;
    }
    
    RuleTrace trace_peg_293(stream, "comment");
    int myposition = position;
    
    
    
    Result result_peg_235(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_235.getPosition()))){
                        result_peg_235.nextPosition();
                    } else {
                        goto out_peg_237;
                    }
                }
                result_peg_235.setValue((void*) ";");
            
            
            
            result_peg_235.reset();
                do{
                    Result result_peg_239(result_peg_235.getPosition());
                    {
                    
                        Result result_peg_242(result_peg_239);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_242.getPosition()))){
                                        result_peg_242.nextPosition();
                                    } else {
                                        goto out_peg_245;
                                    }
                                }
                                result_peg_242.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_243;
                            out_peg_245:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_242.getPosition()))){
                                        result_peg_242.nextPosition();
                                    } else {
                                        goto out_peg_247;
                                    }
                                }
                                result_peg_242.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_243;
                            out_peg_247:
                            goto not_peg_241;
                            success_peg_243:
                            ;
                            goto loop_peg_238;
                            not_peg_241:
                            result_peg_239.setValue((void*)0);
                        
                        
                        
                        char temp_peg_248 = stream.get(result_peg_239.getPosition());
                            if (temp_peg_248 != '\0'){
                                result_peg_239.setValue((void*) (long) temp_peg_248);
                                result_peg_239.nextPosition();
                            } else {
                                goto loop_peg_238;
                            }
                        
                        
                    }
                    result_peg_235.addResult(result_peg_239);
                } while (true);
                loop_peg_238:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_235;
        stream.update(result_peg_235.getPosition());
        
        
        return result_peg_235;
        out_peg_237:
        Result result_peg_249(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_249.getPosition()))){
                        result_peg_249.nextPosition();
                    } else {
                        goto out_peg_251;
                    }
                }
                result_peg_249.setValue((void*) "=");
            
            
            
            result_peg_249.reset();
                do{
                    Result result_peg_253(result_peg_249.getPosition());
                    {
                    
                        Result result_peg_256(result_peg_253);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_256.getPosition()))){
                                        result_peg_256.nextPosition();
                                    } else {
                                        goto out_peg_259;
                                    }
                                }
                                result_peg_256.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_257;
                            out_peg_259:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_256.getPosition()))){
                                        result_peg_256.nextPosition();
                                    } else {
                                        goto out_peg_261;
                                    }
                                }
                                result_peg_256.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_257;
                            out_peg_261:
                            goto not_peg_255;
                            success_peg_257:
                            ;
                            goto loop_peg_252;
                            not_peg_255:
                            result_peg_253.setValue((void*)0);
                        
                        
                        
                        char temp_peg_262 = stream.get(result_peg_253.getPosition());
                            if (temp_peg_262 != '\0'){
                                result_peg_253.setValue((void*) (long) temp_peg_262);
                                result_peg_253.nextPosition();
                            } else {
                                goto loop_peg_252;
                            }
                        
                        
                    }
                    result_peg_249.addResult(result_peg_253);
                } while (true);
                loop_peg_252:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_249;
        stream.update(result_peg_249.getPosition());
        
        
        return result_peg_249;
        out_peg_251:
        Result result_peg_263(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_263.getPosition()))){
                        result_peg_263.nextPosition();
                    } else {
                        goto out_peg_265;
                    }
                }
                result_peg_263.setValue((void*) "-");
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_263.getPosition()))){
                        result_peg_263.nextPosition();
                    } else {
                        goto out_peg_265;
                    }
                }
                result_peg_263.setValue((void*) "-");
            
            
            
            result_peg_263.reset();
                do{
                    Result result_peg_268(result_peg_263.getPosition());
                    {
                    
                        Result result_peg_271(result_peg_268);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_271.getPosition()))){
                                        result_peg_271.nextPosition();
                                    } else {
                                        goto out_peg_274;
                                    }
                                }
                                result_peg_271.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_272;
                            out_peg_274:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_271.getPosition()))){
                                        result_peg_271.nextPosition();
                                    } else {
                                        goto out_peg_276;
                                    }
                                }
                                result_peg_271.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_272;
                            out_peg_276:
                            goto not_peg_270;
                            success_peg_272:
                            ;
                            goto loop_peg_267;
                            not_peg_270:
                            result_peg_268.setValue((void*)0);
                        
                        
                        
                        char temp_peg_277 = stream.get(result_peg_268.getPosition());
                            if (temp_peg_277 != '\0'){
                                result_peg_268.setValue((void*) (long) temp_peg_277);
                                result_peg_268.nextPosition();
                            } else {
                                goto loop_peg_267;
                            }
                        
                        
                    }
                    result_peg_263.addResult(result_peg_268);
                } while (true);
                loop_peg_267:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_263;
        stream.update(result_peg_263.getPosition());
        
        
        return result_peg_263;
        out_peg_265:
        Result result_peg_278(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_278.getPosition()))){
                        result_peg_278.nextPosition();
                    } else {
                        goto out_peg_280;
                    }
                }
                result_peg_278.setValue((void*) ":");
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_278.getPosition()))){
                        result_peg_278.nextPosition();
                    } else {
                        goto out_peg_280;
                    }
                }
                result_peg_278.setValue((void*) ":");
            
            
            
            result_peg_278.reset();
                do{
                    Result result_peg_283(result_peg_278.getPosition());
                    {
                    
                        Result result_peg_286(result_peg_283);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_286.getPosition()))){
                                        result_peg_286.nextPosition();
                                    } else {
                                        goto out_peg_289;
                                    }
                                }
                                result_peg_286.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_287;
                            out_peg_289:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_286.getPosition()))){
                                        result_peg_286.nextPosition();
                                    } else {
                                        goto out_peg_291;
                                    }
                                }
                                result_peg_286.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_287;
                            out_peg_291:
                            goto not_peg_285;
                            success_peg_287:
                            ;
                            goto loop_peg_282;
                            not_peg_285:
                            result_peg_283.setValue((void*)0);
                        
                        
                        
                        char temp_peg_292 = stream.get(result_peg_283.getPosition());
                            if (temp_peg_292 != '\0'){
                                result_peg_283.setValue((void*) (long) temp_peg_292);
                                result_peg_283.nextPosition();
                            } else {
                                goto loop_peg_282;
                            }
                        
                        
                    }
                    result_peg_278.addResult(result_peg_283);
                } while (true);
                loop_peg_282:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_278;
        stream.update(result_peg_278.getPosition());
        
        
        return result_peg_278;
        out_peg_280:
    
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_294 = stream.getColumn(position);
    if (column_peg_294.chunk1 != 0 && column_peg_294.chunk1->chunk_section.calculated()){
        return column_peg_294.chunk1->chunk_section;
    }
    
    RuleTrace trace_peg_362(stream, "section");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_295(myposition);
        
        {
        
            result_peg_295 = rule_section_title(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
            
            Result result_peg_296 = result_peg_295;
            
            {
                    Value value((void*) 0);
                    value = makeSection(result_peg_296.getValues());
                    result_peg_295.setValue(value);
                }
                ast = result_peg_295.getValues();
            
            
            
            result_peg_295 = rule_whitespace(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
            
            
            
            result_peg_295.reset();
                do{
                    Result result_peg_302(result_peg_295.getPosition());
                    {
                    
                        {
                                
                                result_peg_302.reset();
                                do{
                                    Result result_peg_334(result_peg_302.getPosition());
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar(" "[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_345;
                                                }
                                            }
                                            result_peg_334.setValue((void*) " ");
                                                
                                        }
                                        goto success_peg_343;
                                        out_peg_345:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\t"[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_347;
                                                }
                                            }
                                            result_peg_334.setValue((void*) "\t");
                                                
                                        }
                                        goto success_peg_343;
                                        out_peg_347:
                                        goto out_peg_342;
                                        success_peg_343:
                                        ;
                                        
                                    }
                                    goto success_peg_335;
                                    out_peg_342:
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_357;
                                                }
                                            }
                                            result_peg_334.setValue((void*) "\n");
                                                
                                        }
                                        goto success_peg_355;
                                        out_peg_357:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_359;
                                                }
                                            }
                                            result_peg_334.setValue((void*) "\r");
                                                
                                        }
                                        goto success_peg_355;
                                        out_peg_359:
                                        goto out_peg_354;
                                        success_peg_355:
                                        ;
                                        
                                    }
                                    goto success_peg_335;
                                    out_peg_354:
                                    goto loop_peg_333;
                                    success_peg_335:
                                    ;
                                    result_peg_302.addResult(result_peg_334);
                                } while (true);
                                loop_peg_333:
                                ;
                                        
                            }
                            goto success_peg_304;
                            goto loop_peg_301;
                            success_peg_304:
                            ;
                        
                        
                        
                        result_peg_302 = rule_section_item(stream, result_peg_302.getPosition(), ast);
                            if (result_peg_302.error()){
                                goto loop_peg_301;
                            }
                        
                        
                        
                        result_peg_302 = rule_whitespace(stream, result_peg_302.getPosition());
                            if (result_peg_302.error()){
                                goto loop_peg_301;
                            }
                        
                        
                        
                        result_peg_302 = rule_line_end(stream, result_peg_302.getPosition());
                            if (result_peg_302.error()){
                                goto loop_peg_301;
                            }
                        
                        
                    }
                    result_peg_295.addResult(result_peg_302);
                } while (true);
                loop_peg_301:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_295.setValue(value);
                }
            
            
        }
        
        if (column_peg_294.chunk1 == 0){
            column_peg_294.chunk1 = new Chunk1();
        }
        column_peg_294.chunk1->chunk_section = result_peg_295;
        stream.update(result_peg_295.getPosition());
        
        
        return result_peg_295;
        out_peg_297:
    
        if (column_peg_294.chunk1 == 0){
            column_peg_294.chunk1 = new Chunk1();
        }
        column_peg_294.chunk1->chunk_section = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    RuleTrace trace_peg_403(stream, "section_item");
    int myposition = position;
    
    
    Value data;
    Result result_peg_364(myposition);
        
        {
        
            {
                    
                    result_peg_364.reset();
                    do{
                        Result result_peg_376(result_peg_364.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_376.getPosition()))){
                                    result_peg_376.nextPosition();
                                } else {
                                    goto out_peg_379;
                                }
                            }
                            result_peg_376.setValue((void*) " ");
                                
                        }
                        goto success_peg_377;
                        out_peg_379:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_376.getPosition()))){
                                    result_peg_376.nextPosition();
                                } else {
                                    goto out_peg_381;
                                }
                            }
                            result_peg_376.setValue((void*) "\t");
                                
                        }
                        goto success_peg_377;
                        out_peg_381:
                        goto loop_peg_375;
                        success_peg_377:
                        ;
                        result_peg_364.addResult(result_peg_376);
                    } while (true);
                    loop_peg_375:
                    ;
                            
                }
                goto success_peg_366;
                goto out_peg_382;
                success_peg_366:
                ;
            
            
            
            result_peg_364 = rule_comment(stream, result_peg_364.getPosition());
                if (result_peg_364.error()){
                    goto out_peg_382;
                }
            
            
        }
        stream.update(result_peg_364.getPosition());
        
        return result_peg_364;
        out_peg_382:
        Result result_peg_383(myposition);
        
        {
        
            {
                    
                    result_peg_383.reset();
                    do{
                        Result result_peg_395(result_peg_383.getPosition());
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
                        result_peg_383.addResult(result_peg_395);
                    } while (true);
                    loop_peg_394:
                    ;
                            
                }
                goto success_peg_385;
                goto out_peg_401;
                success_peg_385:
                ;
            
            
            
            result_peg_383 = rule_assignment(stream, result_peg_383.getPosition());
                if (result_peg_383.error()){
                    goto out_peg_401;
                }
                data = result_peg_383.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_383.setValue(value);
                }
            
            
        }
        stream.update(result_peg_383.getPosition());
        
        return result_peg_383;
        out_peg_401:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_404 = stream.getColumn(position);
    if (column_peg_404.chunk1 != 0 && column_peg_404.chunk1->chunk_section_title.calculated()){
        return column_peg_404.chunk1->chunk_section_title;
    }
    
    RuleTrace trace_peg_433(stream, "section_title");
    int myposition = position;
    
    
    Value name;
    Result result_peg_405(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_405.getPosition()))){
                        result_peg_405.nextPosition();
                    } else {
                        goto out_peg_407;
                    }
                }
                result_peg_405.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_405.reset();
                    do{
                        Result result_peg_419(result_peg_405.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_419.getPosition()))){
                                    result_peg_419.nextPosition();
                                } else {
                                    goto out_peg_422;
                                }
                            }
                            result_peg_419.setValue((void*) " ");
                                
                        }
                        goto success_peg_420;
                        out_peg_422:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_419.getPosition()))){
                                    result_peg_419.nextPosition();
                                } else {
                                    goto out_peg_424;
                                }
                            }
                            result_peg_419.setValue((void*) "\t");
                                
                        }
                        goto success_peg_420;
                        out_peg_424:
                        goto loop_peg_418;
                        success_peg_420:
                        ;
                        result_peg_405.addResult(result_peg_419);
                    } while (true);
                    loop_peg_418:
                    ;
                            
                }
                goto success_peg_409;
                goto out_peg_407;
                success_peg_409:
                ;
            
            
            
            result_peg_405.reset();
                do{
                    Result result_peg_427(result_peg_405.getPosition());
                    {
                    
                        Result result_peg_430(result_peg_427);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_430.getPosition()))){
                                    result_peg_430.nextPosition();
                                } else {
                                    goto not_peg_429;
                                }
                            }
                            result_peg_430.setValue((void*) "]");
                            goto loop_peg_426;
                            not_peg_429:
                            result_peg_427.setValue((void*)0);
                        
                        
                        
                        char temp_peg_431 = stream.get(result_peg_427.getPosition());
                            if (temp_peg_431 != '\0'){
                                result_peg_427.setValue((void*) (long) temp_peg_431);
                                result_peg_427.nextPosition();
                            } else {
                                goto loop_peg_426;
                            }
                        
                        
                    }
                    result_peg_405.addResult(result_peg_427);
                } while (true);
                loop_peg_426:
                if (result_peg_405.matches() == 0){
                    goto out_peg_407;
                }
                name = result_peg_405.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_405.getPosition()))){
                        result_peg_405.nextPosition();
                    } else {
                        goto out_peg_407;
                    }
                }
                result_peg_405.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = toString(name);
                    result_peg_405.setValue(value);
                }
            
            
        }
        
        if (column_peg_404.chunk1 == 0){
            column_peg_404.chunk1 = new Chunk1();
        }
        column_peg_404.chunk1->chunk_section_title = result_peg_405;
        stream.update(result_peg_405.getPosition());
        
        
        return result_peg_405;
        out_peg_407:
    
        if (column_peg_404.chunk1 == 0){
            column_peg_404.chunk1 = new Chunk1();
        }
        column_peg_404.chunk1->chunk_section_title = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_434 = stream.getColumn(position);
    if (column_peg_434.chunk1 != 0 && column_peg_434.chunk1->chunk_assignment.calculated()){
        return column_peg_434.chunk1->chunk_assignment;
    }
    
    RuleTrace trace_peg_760(stream, "assignment");
    int myposition = position;
    
    
    Value all;
        Value line;
        Value own;
        Value exp;
        Value name;
        Value index;
    Result result_peg_435(myposition);
        
        {
        
            for (int i = 0; i < 7; i++){
                    if (compareCharCase("command"[i], stream.get(result_peg_435.getPosition()))){
                        result_peg_435.nextPosition();
                    } else {
                        goto out_peg_437;
                    }
                }
                result_peg_435.setValue((void*) "command");
            
            
            
            {
                    
                    result_peg_435.reset();
                    do{
                        Result result_peg_449(result_peg_435.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_449.getPosition()))){
                                    result_peg_449.nextPosition();
                                } else {
                                    goto out_peg_452;
                                }
                            }
                            result_peg_449.setValue((void*) " ");
                                
                        }
                        goto success_peg_450;
                        out_peg_452:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_449.getPosition()))){
                                    result_peg_449.nextPosition();
                                } else {
                                    goto out_peg_454;
                                }
                            }
                            result_peg_449.setValue((void*) "\t");
                                
                        }
                        goto success_peg_450;
                        out_peg_454:
                        goto loop_peg_448;
                        success_peg_450:
                        ;
                        result_peg_435.addResult(result_peg_449);
                    } while (true);
                    loop_peg_448:
                    ;
                            
                }
                goto success_peg_439;
                goto out_peg_437;
                success_peg_439:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_435.getPosition()))){
                        result_peg_435.nextPosition();
                    } else {
                        goto out_peg_437;
                    }
                }
                result_peg_435.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_435.reset();
                    do{
                        Result result_peg_467(result_peg_435.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_467.getPosition()))){
                                    result_peg_467.nextPosition();
                                } else {
                                    goto out_peg_470;
                                }
                            }
                            result_peg_467.setValue((void*) " ");
                                
                        }
                        goto success_peg_468;
                        out_peg_470:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_467.getPosition()))){
                                    result_peg_467.nextPosition();
                                } else {
                                    goto out_peg_472;
                                }
                            }
                            result_peg_467.setValue((void*) "\t");
                                
                        }
                        goto success_peg_468;
                        out_peg_472:
                        goto loop_peg_466;
                        success_peg_468:
                        ;
                        result_peg_435.addResult(result_peg_467);
                    } while (true);
                    loop_peg_466:
                    ;
                            
                }
                goto success_peg_457;
                goto out_peg_437;
                success_peg_457:
                ;
            
            
            
            result_peg_435 = rule_keys(stream, result_peg_435.getPosition());
                if (result_peg_435.error()){
                    goto out_peg_437;
                }
                all = result_peg_435.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("command", all);
                    result_peg_435.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_435;
        stream.update(result_peg_435.getPosition());
        
        
        return result_peg_435;
        out_peg_437:
        Result result_peg_474(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_476 = stream.getLineInfo(result_peg_474.getPosition());
                line = &line_info_peg_476;
            
            
            
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("guardsound"[i], stream.get(result_peg_474.getPosition()))){
                        result_peg_474.nextPosition();
                    } else {
                        goto out_peg_478;
                    }
                }
                result_peg_474.setValue((void*) "guardsound");
            
            
            
            {
                    
                    result_peg_474.reset();
                    do{
                        Result result_peg_490(result_peg_474.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_490.getPosition()))){
                                    result_peg_490.nextPosition();
                                } else {
                                    goto out_peg_493;
                                }
                            }
                            result_peg_490.setValue((void*) " ");
                                
                        }
                        goto success_peg_491;
                        out_peg_493:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_490.getPosition()))){
                                    result_peg_490.nextPosition();
                                } else {
                                    goto out_peg_495;
                                }
                            }
                            result_peg_490.setValue((void*) "\t");
                                
                        }
                        goto success_peg_491;
                        out_peg_495:
                        goto loop_peg_489;
                        success_peg_491:
                        ;
                        result_peg_474.addResult(result_peg_490);
                    } while (true);
                    loop_peg_489:
                    ;
                            
                }
                goto success_peg_480;
                goto out_peg_478;
                success_peg_480:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_474.getPosition()))){
                        result_peg_474.nextPosition();
                    } else {
                        goto out_peg_478;
                    }
                }
                result_peg_474.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_474.reset();
                    do{
                        Result result_peg_508(result_peg_474.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_508.getPosition()))){
                                    result_peg_508.nextPosition();
                                } else {
                                    goto out_peg_511;
                                }
                            }
                            result_peg_508.setValue((void*) " ");
                                
                        }
                        goto success_peg_509;
                        out_peg_511:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_508.getPosition()))){
                                    result_peg_508.nextPosition();
                                } else {
                                    goto out_peg_513;
                                }
                            }
                            result_peg_508.setValue((void*) "\t");
                                
                        }
                        goto success_peg_509;
                        out_peg_513:
                        goto loop_peg_507;
                        success_peg_509:
                        ;
                        result_peg_474.addResult(result_peg_508);
                    } while (true);
                    loop_peg_507:
                    ;
                            
                }
                goto success_peg_498;
                goto out_peg_478;
                success_peg_498:
                ;
            
            
            
            int save_peg_515 = result_peg_474.getPosition();
                
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_474.getPosition()))){
                        result_peg_474.nextPosition();
                    } else {
                        
                        result_peg_474 = Result(save_peg_515);
                        result_peg_474.setValue((void*) 0);
                        
                    }
                }
                result_peg_474.setValue((void*) "s");
                own = result_peg_474.getValues();
            
            
            
            result_peg_474 = rule_expr(stream, result_peg_474.getPosition());
                if (result_peg_474.error()){
                    goto out_peg_478;
                }
                exp = result_peg_474.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    if (own.getValue() != 0){
                                            value = makeAttribute("guardsound-own", exp);
                                        } else {
                                            value = makeAttribute("guardsound", exp);
                                        }
                    result_peg_474.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_474;
        stream.update(result_peg_474.getPosition());
        
        
        return result_peg_474;
        out_peg_478:
        Result result_peg_517(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_519 = stream.getLineInfo(result_peg_517.getPosition());
                line = &line_info_peg_519;
            
            
            
            for (int i = 0; i < 7; i++){
                    if (compareCharCase("sparkno"[i], stream.get(result_peg_517.getPosition()))){
                        result_peg_517.nextPosition();
                    } else {
                        goto out_peg_521;
                    }
                }
                result_peg_517.setValue((void*) "sparkno");
            
            
            
            {
                    
                    result_peg_517.reset();
                    do{
                        Result result_peg_533(result_peg_517.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_533.getPosition()))){
                                    result_peg_533.nextPosition();
                                } else {
                                    goto out_peg_536;
                                }
                            }
                            result_peg_533.setValue((void*) " ");
                                
                        }
                        goto success_peg_534;
                        out_peg_536:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_533.getPosition()))){
                                    result_peg_533.nextPosition();
                                } else {
                                    goto out_peg_538;
                                }
                            }
                            result_peg_533.setValue((void*) "\t");
                                
                        }
                        goto success_peg_534;
                        out_peg_538:
                        goto loop_peg_532;
                        success_peg_534:
                        ;
                        result_peg_517.addResult(result_peg_533);
                    } while (true);
                    loop_peg_532:
                    ;
                            
                }
                goto success_peg_523;
                goto out_peg_521;
                success_peg_523:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_517.getPosition()))){
                        result_peg_517.nextPosition();
                    } else {
                        goto out_peg_521;
                    }
                }
                result_peg_517.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_517.reset();
                    do{
                        Result result_peg_551(result_peg_517.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_551.getPosition()))){
                                    result_peg_551.nextPosition();
                                } else {
                                    goto out_peg_554;
                                }
                            }
                            result_peg_551.setValue((void*) " ");
                                
                        }
                        goto success_peg_552;
                        out_peg_554:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_551.getPosition()))){
                                    result_peg_551.nextPosition();
                                } else {
                                    goto out_peg_556;
                                }
                            }
                            result_peg_551.setValue((void*) "\t");
                                
                        }
                        goto success_peg_552;
                        out_peg_556:
                        goto loop_peg_550;
                        success_peg_552:
                        ;
                        result_peg_517.addResult(result_peg_551);
                    } while (true);
                    loop_peg_550:
                    ;
                            
                }
                goto success_peg_541;
                goto out_peg_521;
                success_peg_541:
                ;
            
            
            
            int save_peg_558 = result_peg_517.getPosition();
                
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_517.getPosition()))){
                        result_peg_517.nextPosition();
                    } else {
                        
                        result_peg_517 = Result(save_peg_558);
                        result_peg_517.setValue((void*) 0);
                        
                    }
                }
                result_peg_517.setValue((void*) "s");
            
            
            
            result_peg_517 = rule_expr(stream, result_peg_517.getPosition());
                if (result_peg_517.error()){
                    goto out_peg_521;
                }
                exp = result_peg_517.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("sparkno", exp);
                    result_peg_517.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_517;
        stream.update(result_peg_517.getPosition());
        
        
        return result_peg_517;
        out_peg_521:
        Result result_peg_560(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_562 = stream.getLineInfo(result_peg_560.getPosition());
                line = &line_info_peg_562;
            
            
            
            result_peg_560 = rule_identifier(stream, result_peg_560.getPosition());
                if (result_peg_560.error()){
                    goto out_peg_564;
                }
                name = result_peg_560.getValues();
            
            
            
            {
                    
                    result_peg_560.reset();
                    do{
                        Result result_peg_576(result_peg_560.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_576.getPosition()))){
                                    result_peg_576.nextPosition();
                                } else {
                                    goto out_peg_579;
                                }
                            }
                            result_peg_576.setValue((void*) " ");
                                
                        }
                        goto success_peg_577;
                        out_peg_579:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_576.getPosition()))){
                                    result_peg_576.nextPosition();
                                } else {
                                    goto out_peg_581;
                                }
                            }
                            result_peg_576.setValue((void*) "\t");
                                
                        }
                        goto success_peg_577;
                        out_peg_581:
                        goto loop_peg_575;
                        success_peg_577:
                        ;
                        result_peg_560.addResult(result_peg_576);
                    } while (true);
                    loop_peg_575:
                    ;
                            
                }
                goto success_peg_566;
                goto out_peg_564;
                success_peg_566:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_560.getPosition()))){
                        result_peg_560.nextPosition();
                    } else {
                        goto out_peg_564;
                    }
                }
                result_peg_560.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_560.reset();
                    do{
                        Result result_peg_594(result_peg_560.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_594.getPosition()))){
                                    result_peg_594.nextPosition();
                                } else {
                                    goto out_peg_597;
                                }
                            }
                            result_peg_594.setValue((void*) " ");
                                
                        }
                        goto success_peg_595;
                        out_peg_597:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_594.getPosition()))){
                                    result_peg_594.nextPosition();
                                } else {
                                    goto out_peg_599;
                                }
                            }
                            result_peg_594.setValue((void*) "\t");
                                
                        }
                        goto success_peg_595;
                        out_peg_599:
                        goto loop_peg_593;
                        success_peg_595:
                        ;
                        result_peg_560.addResult(result_peg_594);
                    } while (true);
                    loop_peg_593:
                    ;
                            
                }
                goto success_peg_584;
                goto out_peg_564;
                success_peg_584:
                ;
            
            
            
            result_peg_560 = rule_expr(stream, result_peg_560.getPosition());
                if (result_peg_560.error()){
                    goto out_peg_564;
                }
                exp = result_peg_560.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
                    result_peg_560.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_560;
        stream.update(result_peg_560.getPosition());
        
        
        return result_peg_560;
        out_peg_564:
        Result result_peg_601(myposition);
        
        {
        
            result_peg_601 = rule_identifier(stream, result_peg_601.getPosition());
                if (result_peg_601.error()){
                    goto out_peg_603;
                }
                name = result_peg_601.getValues();
            
            
            
            {
                    
                    result_peg_601.reset();
                    do{
                        Result result_peg_615(result_peg_601.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_615.getPosition()))){
                                    result_peg_615.nextPosition();
                                } else {
                                    goto out_peg_618;
                                }
                            }
                            result_peg_615.setValue((void*) " ");
                                
                        }
                        goto success_peg_616;
                        out_peg_618:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_615.getPosition()))){
                                    result_peg_615.nextPosition();
                                } else {
                                    goto out_peg_620;
                                }
                            }
                            result_peg_615.setValue((void*) "\t");
                                
                        }
                        goto success_peg_616;
                        out_peg_620:
                        goto loop_peg_614;
                        success_peg_616:
                        ;
                        result_peg_601.addResult(result_peg_615);
                    } while (true);
                    loop_peg_614:
                    ;
                            
                }
                goto success_peg_605;
                goto out_peg_603;
                success_peg_605:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_601.getPosition()))){
                        result_peg_601.nextPosition();
                    } else {
                        goto out_peg_603;
                    }
                }
                result_peg_601.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_601.reset();
                    do{
                        Result result_peg_633(result_peg_601.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_633.getPosition()))){
                                    result_peg_633.nextPosition();
                                } else {
                                    goto out_peg_636;
                                }
                            }
                            result_peg_633.setValue((void*) " ");
                                
                        }
                        goto success_peg_634;
                        out_peg_636:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_633.getPosition()))){
                                    result_peg_633.nextPosition();
                                } else {
                                    goto out_peg_638;
                                }
                            }
                            result_peg_633.setValue((void*) "\t");
                                
                        }
                        goto success_peg_634;
                        out_peg_638:
                        goto loop_peg_632;
                        success_peg_634:
                        ;
                        result_peg_601.addResult(result_peg_633);
                    } while (true);
                    loop_peg_632:
                    ;
                            
                }
                goto success_peg_623;
                goto out_peg_603;
                success_peg_623:
                ;
            
            
            
            result_peg_601 = rule_expr(stream, result_peg_601.getPosition());
                if (result_peg_601.error()){
                    goto out_peg_603;
                }
                exp = result_peg_601.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name, negateExpression(exp));
                    result_peg_601.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_601;
        stream.update(result_peg_601.getPosition());
        
        
        return result_peg_601;
        out_peg_603:
        Result result_peg_640(myposition);
        
        {
        
            result_peg_640 = rule_identifier(stream, result_peg_640.getPosition());
                if (result_peg_640.error()){
                    goto out_peg_642;
                }
                name = result_peg_640.getValues();
            
            
            
            {
                    
                    result_peg_640.reset();
                    do{
                        Result result_peg_654(result_peg_640.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_654.getPosition()))){
                                    result_peg_654.nextPosition();
                                } else {
                                    goto out_peg_657;
                                }
                            }
                            result_peg_654.setValue((void*) " ");
                                
                        }
                        goto success_peg_655;
                        out_peg_657:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_654.getPosition()))){
                                    result_peg_654.nextPosition();
                                } else {
                                    goto out_peg_659;
                                }
                            }
                            result_peg_654.setValue((void*) "\t");
                                
                        }
                        goto success_peg_655;
                        out_peg_659:
                        goto loop_peg_653;
                        success_peg_655:
                        ;
                        result_peg_640.addResult(result_peg_654);
                    } while (true);
                    loop_peg_653:
                    ;
                            
                }
                goto success_peg_644;
                goto out_peg_642;
                success_peg_644:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_640.getPosition()))){
                        result_peg_640.nextPosition();
                    } else {
                        goto out_peg_642;
                    }
                }
                result_peg_640.setValue((void*) "=");
            
            
            
            result_peg_640 = rule_whitespace(stream, result_peg_640.getPosition());
                if (result_peg_640.error()){
                    goto out_peg_642;
                }
            
            
            
            Result result_peg_663(result_peg_640.getPosition());
                result_peg_663 = rule_line_end(stream, result_peg_663.getPosition());
                if (result_peg_663.error()){
                    goto out_peg_642;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name);
                    result_peg_640.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_640;
        stream.update(result_peg_640.getPosition());
        
        
        return result_peg_640;
        out_peg_642:
        Result result_peg_664(myposition);
        
        {
        
            result_peg_664 = rule_identifier(stream, result_peg_664.getPosition());
                if (result_peg_664.error()){
                    goto out_peg_666;
                }
                name = result_peg_664.getValues();
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_678(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_678.getPosition()))){
                                    result_peg_678.nextPosition();
                                } else {
                                    goto out_peg_681;
                                }
                            }
                            result_peg_678.setValue((void*) " ");
                                
                        }
                        goto success_peg_679;
                        out_peg_681:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_678.getPosition()))){
                                    result_peg_678.nextPosition();
                                } else {
                                    goto out_peg_683;
                                }
                            }
                            result_peg_678.setValue((void*) "\t");
                                
                        }
                        goto success_peg_679;
                        out_peg_683:
                        goto loop_peg_677;
                        success_peg_679:
                        ;
                        result_peg_664.addResult(result_peg_678);
                    } while (true);
                    loop_peg_677:
                    ;
                            
                }
                goto success_peg_668;
                goto out_peg_666;
                success_peg_668:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_664.getPosition()))){
                        result_peg_664.nextPosition();
                    } else {
                        goto out_peg_666;
                    }
                }
                result_peg_664.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_696(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_696.getPosition()))){
                                    result_peg_696.nextPosition();
                                } else {
                                    goto out_peg_699;
                                }
                            }
                            result_peg_696.setValue((void*) " ");
                                
                        }
                        goto success_peg_697;
                        out_peg_699:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_696.getPosition()))){
                                    result_peg_696.nextPosition();
                                } else {
                                    goto out_peg_701;
                                }
                            }
                            result_peg_696.setValue((void*) "\t");
                                
                        }
                        goto success_peg_697;
                        out_peg_701:
                        goto loop_peg_695;
                        success_peg_697:
                        ;
                        result_peg_664.addResult(result_peg_696);
                    } while (true);
                    loop_peg_695:
                    ;
                            
                }
                goto success_peg_686;
                goto out_peg_666;
                success_peg_686:
                ;
            
            
            
            result_peg_664 = rule_integer(stream, result_peg_664.getPosition());
                if (result_peg_664.error()){
                    goto out_peg_666;
                }
                index = result_peg_664.getValues();
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_714(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_714.getPosition()))){
                                    result_peg_714.nextPosition();
                                } else {
                                    goto out_peg_717;
                                }
                            }
                            result_peg_714.setValue((void*) " ");
                                
                        }
                        goto success_peg_715;
                        out_peg_717:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_714.getPosition()))){
                                    result_peg_714.nextPosition();
                                } else {
                                    goto out_peg_719;
                                }
                            }
                            result_peg_714.setValue((void*) "\t");
                                
                        }
                        goto success_peg_715;
                        out_peg_719:
                        goto loop_peg_713;
                        success_peg_715:
                        ;
                        result_peg_664.addResult(result_peg_714);
                    } while (true);
                    loop_peg_713:
                    ;
                            
                }
                goto success_peg_704;
                goto out_peg_666;
                success_peg_704:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_664.getPosition()))){
                        result_peg_664.nextPosition();
                    } else {
                        goto out_peg_666;
                    }
                }
                result_peg_664.setValue((void*) ")");
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_732(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_732.getPosition()))){
                                    result_peg_732.nextPosition();
                                } else {
                                    goto out_peg_735;
                                }
                            }
                            result_peg_732.setValue((void*) " ");
                                
                        }
                        goto success_peg_733;
                        out_peg_735:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_732.getPosition()))){
                                    result_peg_732.nextPosition();
                                } else {
                                    goto out_peg_737;
                                }
                            }
                            result_peg_732.setValue((void*) "\t");
                                
                        }
                        goto success_peg_733;
                        out_peg_737:
                        goto loop_peg_731;
                        success_peg_733:
                        ;
                        result_peg_664.addResult(result_peg_732);
                    } while (true);
                    loop_peg_731:
                    ;
                            
                }
                goto success_peg_722;
                goto out_peg_666;
                success_peg_722:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_664.getPosition()))){
                        result_peg_664.nextPosition();
                    } else {
                        goto out_peg_666;
                    }
                }
                result_peg_664.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_750(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_750.getPosition()))){
                                    result_peg_750.nextPosition();
                                } else {
                                    goto out_peg_753;
                                }
                            }
                            result_peg_750.setValue((void*) " ");
                                
                        }
                        goto success_peg_751;
                        out_peg_753:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_750.getPosition()))){
                                    result_peg_750.nextPosition();
                                } else {
                                    goto out_peg_755;
                                }
                            }
                            result_peg_750.setValue((void*) "\t");
                                
                        }
                        goto success_peg_751;
                        out_peg_755:
                        goto loop_peg_749;
                        success_peg_751:
                        ;
                        result_peg_664.addResult(result_peg_750);
                    } while (true);
                    loop_peg_749:
                    ;
                            
                }
                goto success_peg_740;
                goto out_peg_666;
                success_peg_740:
                ;
            
            
            
            result_peg_664 = rule_expr(stream, result_peg_664.getPosition());
                if (result_peg_664.error()){
                    goto out_peg_666;
                }
                exp = result_peg_664.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(name, index, exp);
                    result_peg_664.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_664;
        stream.update(result_peg_664.getPosition());
        
        
        return result_peg_664;
        out_peg_666:
        Result result_peg_757(myposition);
        
        {
        
            for (int i = 0; i < 4; i++){
                    if (compareCharCase("ctrl"[i], stream.get(result_peg_757.getPosition()))){
                        result_peg_757.nextPosition();
                    } else {
                        goto out_peg_759;
                    }
                }
                result_peg_757.setValue((void*) "ctrl");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(makeSimpleIdentifier("ctrl"), makeNumber(1));
                    result_peg_757.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_757;
        stream.update(result_peg_757.getPosition());
        
        
        return result_peg_757;
        out_peg_759:
    
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_761 = stream.getColumn(position);
    if (column_peg_761.chunk2 != 0 && column_peg_761.chunk2->chunk_identifier.calculated()){
        return column_peg_761.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_771(stream, "identifier");
    int myposition = position;
    
    
    Value line;
        Value first;
        Value rest;
    Result result_peg_762(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_764 = stream.getLineInfo(result_peg_762.getPosition());
                line = &line_info_peg_764;
            
            
            
            result_peg_762 = rule_name(stream, result_peg_762.getPosition());
                if (result_peg_762.error()){
                    goto out_peg_766;
                }
                first = result_peg_762.getValues();
            
            
            
            result_peg_762.reset();
                do{
                    Result result_peg_769(result_peg_762.getPosition());
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_769.getPosition()))){
                                    result_peg_769.nextPosition();
                                } else {
                                    goto loop_peg_768;
                                }
                            }
                            result_peg_769.setValue((void*) ".");
                        
                        
                        
                        result_peg_769 = rule_name(stream, result_peg_769.getPosition());
                            if (result_peg_769.error()){
                                goto loop_peg_768;
                            }
                        
                        
                    }
                    result_peg_762.addResult(result_peg_769);
                } while (true);
                loop_peg_768:
                ;
                rest = result_peg_762.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                    result_peg_762.setValue(value);
                }
            
            
        }
        
        if (column_peg_761.chunk2 == 0){
            column_peg_761.chunk2 = new Chunk2();
        }
        column_peg_761.chunk2->chunk_identifier = result_peg_762;
        stream.update(result_peg_762.getPosition());
        
        
        return result_peg_762;
        out_peg_766:
    
        if (column_peg_761.chunk2 == 0){
            column_peg_761.chunk2 = new Chunk2();
        }
        column_peg_761.chunk2->chunk_identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_772 = stream.getColumn(position);
    if (column_peg_772.chunk2 != 0 && column_peg_772.chunk2->chunk_integer.calculated()){
        return column_peg_772.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_790(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_773(myposition);
        
        {
        
            int save_peg_775 = result_peg_773.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_773.getPosition()))){
                            result_peg_773.nextPosition();
                        } else {
                            goto out_peg_778;
                        }
                    }
                    result_peg_773.setValue((void*) "-");
                        
                }
                goto success_peg_776;
                out_peg_778:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_773.getPosition()))){
                            result_peg_773.nextPosition();
                        } else {
                            goto out_peg_780;
                        }
                    }
                    result_peg_773.setValue((void*) "+");
                        
                }
                goto success_peg_776;
                out_peg_780:
                
                result_peg_773 = Result(save_peg_775);
                result_peg_773.setValue((void*) 0);
                
                success_peg_776:
                ;
            
            Result result_peg_774 = result_peg_773;
            
            result_peg_773.reset();
                do{
                    Result result_peg_783(result_peg_773.getPosition());
                    {
                        
                        char letter_peg_788 = stream.get(result_peg_783.getPosition());
                        if (letter_peg_788 != '\0' && strchr("0123456789", letter_peg_788) != NULL){
                            result_peg_783.nextPosition();
                            result_peg_783.setValue((void*) (long) letter_peg_788);
                        } else {
                            goto out_peg_787;
                        }
                        
                    }
                    goto success_peg_784;
                    out_peg_787:
                    goto loop_peg_782;
                    success_peg_784:
                    ;
                    result_peg_773.addResult(result_peg_783);
                } while (true);
                loop_peg_782:
                if (result_peg_773.matches() == 0){
                    goto out_peg_789;
                }
            
            Result result_peg_781 = result_peg_773;
            
            {
                    Value value((void*) 0);
                    value = makeInteger(result_peg_774.getValues(), result_peg_781.getValues());
                    result_peg_773.setValue(value);
                }
            
            
        }
        
        if (column_peg_772.chunk2 == 0){
            column_peg_772.chunk2 = new Chunk2();
        }
        column_peg_772.chunk2->chunk_integer = result_peg_773;
        stream.update(result_peg_773.getPosition());
        
        
        return result_peg_773;
        out_peg_789:
    
        if (column_peg_772.chunk2 == 0){
            column_peg_772.chunk2 = new Chunk2();
        }
        column_peg_772.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_791 = stream.getColumn(position);
    if (column_peg_791.chunk2 != 0 && column_peg_791.chunk2->chunk_float.calculated()){
        return column_peg_791.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_836(stream, "float");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_792(myposition);
        
        {
        
            int save_peg_794 = result_peg_792.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_792.getPosition()))){
                            result_peg_792.nextPosition();
                        } else {
                            goto out_peg_797;
                        }
                    }
                    result_peg_792.setValue((void*) "-");
                        
                }
                goto success_peg_795;
                out_peg_797:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_792.getPosition()))){
                            result_peg_792.nextPosition();
                        } else {
                            goto out_peg_799;
                        }
                    }
                    result_peg_792.setValue((void*) "+");
                        
                }
                goto success_peg_795;
                out_peg_799:
                
                result_peg_792 = Result(save_peg_794);
                result_peg_792.setValue((void*) 0);
                
                success_peg_795:
                ;
            
            Result result_peg_793 = result_peg_792;
            
            result_peg_792.reset();
                do{
                    Result result_peg_802(result_peg_792.getPosition());
                    {
                        
                        char letter_peg_807 = stream.get(result_peg_802.getPosition());
                        if (letter_peg_807 != '\0' && strchr("0123456789", letter_peg_807) != NULL){
                            result_peg_802.nextPosition();
                            result_peg_802.setValue((void*) (long) letter_peg_807);
                        } else {
                            goto out_peg_806;
                        }
                        
                    }
                    goto success_peg_803;
                    out_peg_806:
                    goto loop_peg_801;
                    success_peg_803:
                    ;
                    result_peg_792.addResult(result_peg_802);
                } while (true);
                loop_peg_801:
                ;
                left = result_peg_792.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_792.getPosition()))){
                        result_peg_792.nextPosition();
                    } else {
                        goto out_peg_809;
                    }
                }
                result_peg_792.setValue((void*) ".");
            
            
            
            result_peg_792.reset();
                do{
                    Result result_peg_812(result_peg_792.getPosition());
                    {
                        
                        char letter_peg_817 = stream.get(result_peg_812.getPosition());
                        if (letter_peg_817 != '\0' && strchr("0123456789", letter_peg_817) != NULL){
                            result_peg_812.nextPosition();
                            result_peg_812.setValue((void*) (long) letter_peg_817);
                        } else {
                            goto out_peg_816;
                        }
                        
                    }
                    goto success_peg_813;
                    out_peg_816:
                    goto loop_peg_811;
                    success_peg_813:
                    ;
                    result_peg_792.addResult(result_peg_812);
                } while (true);
                loop_peg_811:
                if (result_peg_792.matches() == 0){
                    goto out_peg_809;
                }
                right = result_peg_792.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_793.getValues(), parseDouble(left,right));
                    result_peg_792.setValue(value);
                }
            
            
        }
        
        if (column_peg_791.chunk2 == 0){
            column_peg_791.chunk2 = new Chunk2();
        }
        column_peg_791.chunk2->chunk_float = result_peg_792;
        stream.update(result_peg_792.getPosition());
        
        
        return result_peg_792;
        out_peg_809:
        Result result_peg_818(myposition);
        
        {
        
            int save_peg_820 = result_peg_818.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_818.getPosition()))){
                            result_peg_818.nextPosition();
                        } else {
                            goto out_peg_823;
                        }
                    }
                    result_peg_818.setValue((void*) "-");
                        
                }
                goto success_peg_821;
                out_peg_823:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_818.getPosition()))){
                            result_peg_818.nextPosition();
                        } else {
                            goto out_peg_825;
                        }
                    }
                    result_peg_818.setValue((void*) "+");
                        
                }
                goto success_peg_821;
                out_peg_825:
                
                result_peg_818 = Result(save_peg_820);
                result_peg_818.setValue((void*) 0);
                
                success_peg_821:
                ;
            
            Result result_peg_819 = result_peg_818;
            
            result_peg_818.reset();
                do{
                    Result result_peg_828(result_peg_818.getPosition());
                    {
                        
                        char letter_peg_833 = stream.get(result_peg_828.getPosition());
                        if (letter_peg_833 != '\0' && strchr("0123456789", letter_peg_833) != NULL){
                            result_peg_828.nextPosition();
                            result_peg_828.setValue((void*) (long) letter_peg_833);
                        } else {
                            goto out_peg_832;
                        }
                        
                    }
                    goto success_peg_829;
                    out_peg_832:
                    goto loop_peg_827;
                    success_peg_829:
                    ;
                    result_peg_818.addResult(result_peg_828);
                } while (true);
                loop_peg_827:
                if (result_peg_818.matches() == 0){
                    goto out_peg_834;
                }
                left = result_peg_818.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_818.getPosition()))){
                        result_peg_818.nextPosition();
                    } else {
                        goto out_peg_834;
                    }
                }
                result_peg_818.setValue((void*) ".");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_819.getValues(), parseDouble(left));
                    result_peg_818.setValue(value);
                }
            
            
        }
        
        if (column_peg_791.chunk2 == 0){
            column_peg_791.chunk2 = new Chunk2();
        }
        column_peg_791.chunk2->chunk_float = result_peg_818;
        stream.update(result_peg_818.getPosition());
        
        
        return result_peg_818;
        out_peg_834:
    
        if (column_peg_791.chunk2 == 0){
            column_peg_791.chunk2 = new Chunk2();
        }
        column_peg_791.chunk2->chunk_float = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_837 = stream.getColumn(position);
    if (column_peg_837.chunk2 != 0 && column_peg_837.chunk2->chunk_string.calculated()){
        return column_peg_837.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_852(stream, "string");
    int myposition = position;
    
    
    Value contents;
    Result result_peg_838(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_838.getPosition()))){
                        result_peg_838.nextPosition();
                    } else {
                        goto out_peg_840;
                    }
                }
                result_peg_838.setValue((void*) "\"");
            
            
            
            result_peg_838.reset();
                do{
                    Result result_peg_843(result_peg_838.getPosition());
                    {
                    
                        Result result_peg_846(result_peg_843);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\""[i], stream.get(result_peg_846.getPosition()))){
                                    result_peg_846.nextPosition();
                                } else {
                                    goto not_peg_845;
                                }
                            }
                            result_peg_846.setValue((void*) "\"");
                            goto loop_peg_842;
                            not_peg_845:
                            result_peg_843.setValue((void*)0);
                        
                        
                        
                        Result result_peg_849(result_peg_843);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_849.getPosition()))){
                                    result_peg_849.nextPosition();
                                } else {
                                    goto not_peg_848;
                                }
                            }
                            result_peg_849.setValue((void*) "\n");
                            goto loop_peg_842;
                            not_peg_848:
                            result_peg_843.setValue((void*)0);
                        
                        
                        
                        char temp_peg_850 = stream.get(result_peg_843.getPosition());
                            if (temp_peg_850 != '\0'){
                                result_peg_843.setValue((void*) (long) temp_peg_850);
                                result_peg_843.nextPosition();
                            } else {
                                goto loop_peg_842;
                            }
                        
                        
                    }
                    result_peg_838.addResult(result_peg_843);
                } while (true);
                loop_peg_842:
                ;
                contents = result_peg_838.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_838.getPosition()))){
                        result_peg_838.nextPosition();
                    } else {
                        goto out_peg_840;
                    }
                }
                result_peg_838.setValue((void*) "\"");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeString(contents);
                    result_peg_838.setValue(value);
                }
            
            
        }
        
        if (column_peg_837.chunk2 == 0){
            column_peg_837.chunk2 = new Chunk2();
        }
        column_peg_837.chunk2->chunk_string = result_peg_838;
        stream.update(result_peg_838.getPosition());
        
        
        return result_peg_838;
        out_peg_840:
    
        if (column_peg_837.chunk2 == 0){
            column_peg_837.chunk2 = new Chunk2();
        }
        column_peg_837.chunk2->chunk_string = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_853 = stream.getColumn(position);
    if (column_peg_853.chunk2 != 0 && column_peg_853.chunk2->chunk_range.calculated()){
        return column_peg_853.chunk2->chunk_range;
    }
    
    RuleTrace trace_peg_1154(stream, "range");
    int myposition = position;
    
    
    Value low;
        Value high;
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
                    if (compareChar("]"[i], stream.get(result_peg_854.getPosition()))){
                        result_peg_854.nextPosition();
                    } else {
                        goto out_peg_856;
                    }
                }
                result_peg_854.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllInclusive, low, high);
                    result_peg_854.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_854;
        stream.update(result_peg_854.getPosition());
        
        
        return result_peg_854;
        out_peg_856:
        Result result_peg_929(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_929.getPosition()))){
                        result_peg_929.nextPosition();
                    } else {
                        goto out_peg_931;
                    }
                }
                result_peg_929.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_943(result_peg_929.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_943.getPosition()))){
                                    result_peg_943.nextPosition();
                                } else {
                                    goto out_peg_946;
                                }
                            }
                            result_peg_943.setValue((void*) " ");
                                
                        }
                        goto success_peg_944;
                        out_peg_946:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_943.getPosition()))){
                                    result_peg_943.nextPosition();
                                } else {
                                    goto out_peg_948;
                                }
                            }
                            result_peg_943.setValue((void*) "\t");
                                
                        }
                        goto success_peg_944;
                        out_peg_948:
                        goto loop_peg_942;
                        success_peg_944:
                        ;
                        result_peg_929.addResult(result_peg_943);
                    } while (true);
                    loop_peg_942:
                    ;
                            
                }
                goto success_peg_933;
                goto out_peg_931;
                success_peg_933:
                ;
            
            
            
            result_peg_929 = rule_expr_c(stream, result_peg_929.getPosition());
                if (result_peg_929.error()){
                    goto out_peg_931;
                }
                low = result_peg_929.getValues();
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_961(result_peg_929.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_961.getPosition()))){
                                    result_peg_961.nextPosition();
                                } else {
                                    goto out_peg_964;
                                }
                            }
                            result_peg_961.setValue((void*) " ");
                                
                        }
                        goto success_peg_962;
                        out_peg_964:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_961.getPosition()))){
                                    result_peg_961.nextPosition();
                                } else {
                                    goto out_peg_966;
                                }
                            }
                            result_peg_961.setValue((void*) "\t");
                                
                        }
                        goto success_peg_962;
                        out_peg_966:
                        goto loop_peg_960;
                        success_peg_962:
                        ;
                        result_peg_929.addResult(result_peg_961);
                    } while (true);
                    loop_peg_960:
                    ;
                            
                }
                goto success_peg_951;
                goto out_peg_931;
                success_peg_951:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_929.getPosition()))){
                        result_peg_929.nextPosition();
                    } else {
                        goto out_peg_931;
                    }
                }
                result_peg_929.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_979(result_peg_929.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_979.getPosition()))){
                                    result_peg_979.nextPosition();
                                } else {
                                    goto out_peg_982;
                                }
                            }
                            result_peg_979.setValue((void*) " ");
                                
                        }
                        goto success_peg_980;
                        out_peg_982:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_979.getPosition()))){
                                    result_peg_979.nextPosition();
                                } else {
                                    goto out_peg_984;
                                }
                            }
                            result_peg_979.setValue((void*) "\t");
                                
                        }
                        goto success_peg_980;
                        out_peg_984:
                        goto loop_peg_978;
                        success_peg_980:
                        ;
                        result_peg_929.addResult(result_peg_979);
                    } while (true);
                    loop_peg_978:
                    ;
                            
                }
                goto success_peg_969;
                goto out_peg_931;
                success_peg_969:
                ;
            
            
            
            result_peg_929 = rule_expr_c(stream, result_peg_929.getPosition());
                if (result_peg_929.error()){
                    goto out_peg_931;
                }
                high = result_peg_929.getValues();
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_997(result_peg_929.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_997.getPosition()))){
                                    result_peg_997.nextPosition();
                                } else {
                                    goto out_peg_1000;
                                }
                            }
                            result_peg_997.setValue((void*) " ");
                                
                        }
                        goto success_peg_998;
                        out_peg_1000:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_997.getPosition()))){
                                    result_peg_997.nextPosition();
                                } else {
                                    goto out_peg_1002;
                                }
                            }
                            result_peg_997.setValue((void*) "\t");
                                
                        }
                        goto success_peg_998;
                        out_peg_1002:
                        goto loop_peg_996;
                        success_peg_998:
                        ;
                        result_peg_929.addResult(result_peg_997);
                    } while (true);
                    loop_peg_996:
                    ;
                            
                }
                goto success_peg_987;
                goto out_peg_931;
                success_peg_987:
                ;
            
            
            
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
                    value = makeRange(Ast::Range::AllExclusive, low, high);
                    result_peg_929.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_929;
        stream.update(result_peg_929.getPosition());
        
        
        return result_peg_929;
        out_peg_931:
        Result result_peg_1004(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_1004.getPosition()))){
                        result_peg_1004.nextPosition();
                    } else {
                        goto out_peg_1006;
                    }
                }
                result_peg_1004.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1018(result_peg_1004.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1018.getPosition()))){
                                    result_peg_1018.nextPosition();
                                } else {
                                    goto out_peg_1021;
                                }
                            }
                            result_peg_1018.setValue((void*) " ");
                                
                        }
                        goto success_peg_1019;
                        out_peg_1021:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1018.getPosition()))){
                                    result_peg_1018.nextPosition();
                                } else {
                                    goto out_peg_1023;
                                }
                            }
                            result_peg_1018.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1019;
                        out_peg_1023:
                        goto loop_peg_1017;
                        success_peg_1019:
                        ;
                        result_peg_1004.addResult(result_peg_1018);
                    } while (true);
                    loop_peg_1017:
                    ;
                            
                }
                goto success_peg_1008;
                goto out_peg_1006;
                success_peg_1008:
                ;
            
            
            
            result_peg_1004 = rule_expr_c(stream, result_peg_1004.getPosition());
                if (result_peg_1004.error()){
                    goto out_peg_1006;
                }
                low = result_peg_1004.getValues();
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1036(result_peg_1004.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1036.getPosition()))){
                                    result_peg_1036.nextPosition();
                                } else {
                                    goto out_peg_1039;
                                }
                            }
                            result_peg_1036.setValue((void*) " ");
                                
                        }
                        goto success_peg_1037;
                        out_peg_1039:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1036.getPosition()))){
                                    result_peg_1036.nextPosition();
                                } else {
                                    goto out_peg_1041;
                                }
                            }
                            result_peg_1036.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1037;
                        out_peg_1041:
                        goto loop_peg_1035;
                        success_peg_1037:
                        ;
                        result_peg_1004.addResult(result_peg_1036);
                    } while (true);
                    loop_peg_1035:
                    ;
                            
                }
                goto success_peg_1026;
                goto out_peg_1006;
                success_peg_1026:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1004.getPosition()))){
                        result_peg_1004.nextPosition();
                    } else {
                        goto out_peg_1006;
                    }
                }
                result_peg_1004.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1054(result_peg_1004.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1054.getPosition()))){
                                    result_peg_1054.nextPosition();
                                } else {
                                    goto out_peg_1057;
                                }
                            }
                            result_peg_1054.setValue((void*) " ");
                                
                        }
                        goto success_peg_1055;
                        out_peg_1057:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1054.getPosition()))){
                                    result_peg_1054.nextPosition();
                                } else {
                                    goto out_peg_1059;
                                }
                            }
                            result_peg_1054.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1055;
                        out_peg_1059:
                        goto loop_peg_1053;
                        success_peg_1055:
                        ;
                        result_peg_1004.addResult(result_peg_1054);
                    } while (true);
                    loop_peg_1053:
                    ;
                            
                }
                goto success_peg_1044;
                goto out_peg_1006;
                success_peg_1044:
                ;
            
            
            
            result_peg_1004 = rule_expr_c(stream, result_peg_1004.getPosition());
                if (result_peg_1004.error()){
                    goto out_peg_1006;
                }
                high = result_peg_1004.getValues();
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1072(result_peg_1004.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1072.getPosition()))){
                                    result_peg_1072.nextPosition();
                                } else {
                                    goto out_peg_1075;
                                }
                            }
                            result_peg_1072.setValue((void*) " ");
                                
                        }
                        goto success_peg_1073;
                        out_peg_1075:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1072.getPosition()))){
                                    result_peg_1072.nextPosition();
                                } else {
                                    goto out_peg_1077;
                                }
                            }
                            result_peg_1072.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1073;
                        out_peg_1077:
                        goto loop_peg_1071;
                        success_peg_1073:
                        ;
                        result_peg_1004.addResult(result_peg_1072);
                    } while (true);
                    loop_peg_1071:
                    ;
                            
                }
                goto success_peg_1062;
                goto out_peg_1006;
                success_peg_1062:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_1004.getPosition()))){
                        result_peg_1004.nextPosition();
                    } else {
                        goto out_peg_1006;
                    }
                }
                result_peg_1004.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                    result_peg_1004.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_1004;
        stream.update(result_peg_1004.getPosition());
        
        
        return result_peg_1004;
        out_peg_1006:
        Result result_peg_1079(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_1079.getPosition()))){
                        result_peg_1079.nextPosition();
                    } else {
                        goto out_peg_1081;
                    }
                }
                result_peg_1079.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1093(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1093.getPosition()))){
                                    result_peg_1093.nextPosition();
                                } else {
                                    goto out_peg_1096;
                                }
                            }
                            result_peg_1093.setValue((void*) " ");
                                
                        }
                        goto success_peg_1094;
                        out_peg_1096:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1093.getPosition()))){
                                    result_peg_1093.nextPosition();
                                } else {
                                    goto out_peg_1098;
                                }
                            }
                            result_peg_1093.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1094;
                        out_peg_1098:
                        goto loop_peg_1092;
                        success_peg_1094:
                        ;
                        result_peg_1079.addResult(result_peg_1093);
                    } while (true);
                    loop_peg_1092:
                    ;
                            
                }
                goto success_peg_1083;
                goto out_peg_1081;
                success_peg_1083:
                ;
            
            
            
            result_peg_1079 = rule_expr_c(stream, result_peg_1079.getPosition());
                if (result_peg_1079.error()){
                    goto out_peg_1081;
                }
                low = result_peg_1079.getValues();
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1111(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1111.getPosition()))){
                                    result_peg_1111.nextPosition();
                                } else {
                                    goto out_peg_1114;
                                }
                            }
                            result_peg_1111.setValue((void*) " ");
                                
                        }
                        goto success_peg_1112;
                        out_peg_1114:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1111.getPosition()))){
                                    result_peg_1111.nextPosition();
                                } else {
                                    goto out_peg_1116;
                                }
                            }
                            result_peg_1111.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1112;
                        out_peg_1116:
                        goto loop_peg_1110;
                        success_peg_1112:
                        ;
                        result_peg_1079.addResult(result_peg_1111);
                    } while (true);
                    loop_peg_1110:
                    ;
                            
                }
                goto success_peg_1101;
                goto out_peg_1081;
                success_peg_1101:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1079.getPosition()))){
                        result_peg_1079.nextPosition();
                    } else {
                        goto out_peg_1081;
                    }
                }
                result_peg_1079.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1129(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1129.getPosition()))){
                                    result_peg_1129.nextPosition();
                                } else {
                                    goto out_peg_1132;
                                }
                            }
                            result_peg_1129.setValue((void*) " ");
                                
                        }
                        goto success_peg_1130;
                        out_peg_1132:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1129.getPosition()))){
                                    result_peg_1129.nextPosition();
                                } else {
                                    goto out_peg_1134;
                                }
                            }
                            result_peg_1129.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1130;
                        out_peg_1134:
                        goto loop_peg_1128;
                        success_peg_1130:
                        ;
                        result_peg_1079.addResult(result_peg_1129);
                    } while (true);
                    loop_peg_1128:
                    ;
                            
                }
                goto success_peg_1119;
                goto out_peg_1081;
                success_peg_1119:
                ;
            
            
            
            result_peg_1079 = rule_expr_c(stream, result_peg_1079.getPosition());
                if (result_peg_1079.error()){
                    goto out_peg_1081;
                }
                high = result_peg_1079.getValues();
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1147(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1147.getPosition()))){
                                    result_peg_1147.nextPosition();
                                } else {
                                    goto out_peg_1150;
                                }
                            }
                            result_peg_1147.setValue((void*) " ");
                                
                        }
                        goto success_peg_1148;
                        out_peg_1150:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1147.getPosition()))){
                                    result_peg_1147.nextPosition();
                                } else {
                                    goto out_peg_1152;
                                }
                            }
                            result_peg_1147.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1148;
                        out_peg_1152:
                        goto loop_peg_1146;
                        success_peg_1148:
                        ;
                        result_peg_1079.addResult(result_peg_1147);
                    } while (true);
                    loop_peg_1146:
                    ;
                            
                }
                goto success_peg_1137;
                goto out_peg_1081;
                success_peg_1137:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_1079.getPosition()))){
                        result_peg_1079.nextPosition();
                    } else {
                        goto out_peg_1081;
                    }
                }
                result_peg_1079.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                    result_peg_1079.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_1079;
        stream.update(result_peg_1079.getPosition());
        
        
        return result_peg_1079;
        out_peg_1081:
    
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_1155 = stream.getColumn(position);
    if (column_peg_1155.chunk3 != 0 && column_peg_1155.chunk3->chunk_name.calculated()){
        return column_peg_1155.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_1167(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_1156(myposition);
        
        {
        
            {
                    
                    char letter_peg_1162 = stream.get(result_peg_1156.getPosition());
                    if (letter_peg_1162 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1162) != NULL){
                        result_peg_1156.nextPosition();
                        result_peg_1156.setValue((void*) (long) letter_peg_1162);
                    } else {
                        goto out_peg_1161;
                    }
                    
                }
                goto success_peg_1158;
                out_peg_1161:
                goto out_peg_1163;
                success_peg_1158:
                ;
            
            Result result_peg_1157 = result_peg_1156;
            
            result_peg_1156.reset();
                do{
                    Result result_peg_1166(result_peg_1156.getPosition());
                    result_peg_1166 = rule_alpha_digit(stream, result_peg_1166.getPosition());
                    if (result_peg_1166.error()){
                        goto loop_peg_1165;
                    }
                    result_peg_1156.addResult(result_peg_1166);
                } while (true);
                loop_peg_1165:
                ;
            
            Result result_peg_1164 = result_peg_1156;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_1157.getValues().getValue(),result_peg_1164.getValues());
                    result_peg_1156.setValue(value);
                }
            
            
        }
        
        if (column_peg_1155.chunk3 == 0){
            column_peg_1155.chunk3 = new Chunk3();
        }
        column_peg_1155.chunk3->chunk_name = result_peg_1156;
        stream.update(result_peg_1156.getPosition());
        
        
        return result_peg_1156;
        out_peg_1163:
    
        if (column_peg_1155.chunk3 == 0){
            column_peg_1155.chunk3 = new Chunk3();
        }
        column_peg_1155.chunk3->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_1168 = stream.getColumn(position);
    if (column_peg_1168.chunk3 != 0 && column_peg_1168.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_1168.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_1183(stream, "alpha_digit");
    int myposition = position;
    
    
    
    Result result_peg_1169(myposition);
        
        {
            
            char letter_peg_1174 = stream.get(result_peg_1169.getPosition());
            if (letter_peg_1174 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1174) != NULL){
                result_peg_1169.nextPosition();
                result_peg_1169.setValue((void*) (long) letter_peg_1174);
            } else {
                goto out_peg_1173;
            }
            
        }
        goto success_peg_1170;
        out_peg_1173:
        goto out_peg_1175;
        success_peg_1170:
        ;
        
        if (column_peg_1168.chunk3 == 0){
            column_peg_1168.chunk3 = new Chunk3();
        }
        column_peg_1168.chunk3->chunk_alpha_digit = result_peg_1169;
        stream.update(result_peg_1169.getPosition());
        
        
        return result_peg_1169;
        out_peg_1175:
        Result result_peg_1176(myposition);
        
        {
            
            char letter_peg_1181 = stream.get(result_peg_1176.getPosition());
            if (letter_peg_1181 != '\0' && strchr("0123456789", letter_peg_1181) != NULL){
                result_peg_1176.nextPosition();
                result_peg_1176.setValue((void*) (long) letter_peg_1181);
            } else {
                goto out_peg_1180;
            }
            
        }
        goto success_peg_1177;
        out_peg_1180:
        goto out_peg_1182;
        success_peg_1177:
        ;
        
        if (column_peg_1168.chunk3 == 0){
            column_peg_1168.chunk3 = new Chunk3();
        }
        column_peg_1168.chunk3->chunk_alpha_digit = result_peg_1176;
        stream.update(result_peg_1176.getPosition());
        
        
        return result_peg_1176;
        out_peg_1182:
    
        if (column_peg_1168.chunk3 == 0){
            column_peg_1168.chunk3 = new Chunk3();
        }
        column_peg_1168.chunk3->chunk_alpha_digit = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_1184 = stream.getColumn(position);
    if (column_peg_1184.chunk3 != 0 && column_peg_1184.chunk3->chunk_valuelist.calculated()){
        return column_peg_1184.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1302(stream, "valuelist");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_1185(myposition);
        
        {
        
            result_peg_1185 = rule_expr_c(stream, result_peg_1185.getPosition());
                if (result_peg_1185.error()){
                    goto out_peg_1187;
                }
                first = result_peg_1185.getValues();
            
            
            
            result_peg_1185.reset();
                do{
                    Result result_peg_1190(result_peg_1185.getPosition());
                    {
                    
                        {
                                
                                result_peg_1190.reset();
                                do{
                                    Result result_peg_1202(result_peg_1190.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1202.getPosition()))){
                                                result_peg_1202.nextPosition();
                                            } else {
                                                goto out_peg_1205;
                                            }
                                        }
                                        result_peg_1202.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1203;
                                    out_peg_1205:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1202.getPosition()))){
                                                result_peg_1202.nextPosition();
                                            } else {
                                                goto out_peg_1207;
                                            }
                                        }
                                        result_peg_1202.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1203;
                                    out_peg_1207:
                                    goto loop_peg_1201;
                                    success_peg_1203:
                                    ;
                                    result_peg_1190.addResult(result_peg_1202);
                                } while (true);
                                loop_peg_1201:
                                ;
                                        
                            }
                            goto success_peg_1192;
                            goto loop_peg_1189;
                            success_peg_1192:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1190.getPosition()))){
                                    result_peg_1190.nextPosition();
                                } else {
                                    goto loop_peg_1189;
                                }
                            }
                            result_peg_1190.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1190.reset();
                                do{
                                    Result result_peg_1220(result_peg_1190.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1220.getPosition()))){
                                                result_peg_1220.nextPosition();
                                            } else {
                                                goto out_peg_1223;
                                            }
                                        }
                                        result_peg_1220.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1221;
                                    out_peg_1223:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1220.getPosition()))){
                                                result_peg_1220.nextPosition();
                                            } else {
                                                goto out_peg_1225;
                                            }
                                        }
                                        result_peg_1220.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1221;
                                    out_peg_1225:
                                    goto loop_peg_1219;
                                    success_peg_1221:
                                    ;
                                    result_peg_1190.addResult(result_peg_1220);
                                } while (true);
                                loop_peg_1219:
                                ;
                                        
                            }
                            goto success_peg_1210;
                            goto loop_peg_1189;
                            success_peg_1210:
                            ;
                        
                        
                        
                        result_peg_1190 = rule_expr_c(stream, result_peg_1190.getPosition());
                            if (result_peg_1190.error()){
                                goto loop_peg_1189;
                            }
                        
                        
                    }
                    result_peg_1185.addResult(result_peg_1190);
                } while (true);
                loop_peg_1189:
                if (result_peg_1185.matches() == 0){
                    goto out_peg_1187;
                }
                rest = result_peg_1185.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_1185.setValue(value);
                }
            
            
        }
        
        if (column_peg_1184.chunk3 == 0){
            column_peg_1184.chunk3 = new Chunk3();
        }
        column_peg_1184.chunk3->chunk_valuelist = result_peg_1185;
        stream.update(result_peg_1185.getPosition());
        
        
        return result_peg_1185;
        out_peg_1187:
        Result result_peg_1226(myposition);
        
        {
        
            {
                    
                    result_peg_1226.reset();
                    do{
                        Result result_peg_1238(result_peg_1226.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1238.getPosition()))){
                                    result_peg_1238.nextPosition();
                                } else {
                                    goto out_peg_1241;
                                }
                            }
                            result_peg_1238.setValue((void*) " ");
                                
                        }
                        goto success_peg_1239;
                        out_peg_1241:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1238.getPosition()))){
                                    result_peg_1238.nextPosition();
                                } else {
                                    goto out_peg_1243;
                                }
                            }
                            result_peg_1238.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1239;
                        out_peg_1243:
                        goto loop_peg_1237;
                        success_peg_1239:
                        ;
                        result_peg_1226.addResult(result_peg_1238);
                    } while (true);
                    loop_peg_1237:
                    ;
                            
                }
                goto success_peg_1228;
                goto out_peg_1244;
                success_peg_1228:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1226.getPosition()))){
                        result_peg_1226.nextPosition();
                    } else {
                        goto out_peg_1244;
                    }
                }
                result_peg_1226.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1226.reset();
                    do{
                        Result result_peg_1257(result_peg_1226.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1257.getPosition()))){
                                    result_peg_1257.nextPosition();
                                } else {
                                    goto out_peg_1260;
                                }
                            }
                            result_peg_1257.setValue((void*) " ");
                                
                        }
                        goto success_peg_1258;
                        out_peg_1260:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1257.getPosition()))){
                                    result_peg_1257.nextPosition();
                                } else {
                                    goto out_peg_1262;
                                }
                            }
                            result_peg_1257.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1258;
                        out_peg_1262:
                        goto loop_peg_1256;
                        success_peg_1258:
                        ;
                        result_peg_1226.addResult(result_peg_1257);
                    } while (true);
                    loop_peg_1256:
                    ;
                            
                }
                goto success_peg_1247;
                goto out_peg_1244;
                success_peg_1247:
                ;
            
            
            
            result_peg_1226 = rule_expr_c(stream, result_peg_1226.getPosition());
                if (result_peg_1226.error()){
                    goto out_peg_1244;
                }
                first = result_peg_1226.getValues();
            
            
            
            result_peg_1226.reset();
                do{
                    Result result_peg_1266(result_peg_1226.getPosition());
                    {
                    
                        {
                                
                                result_peg_1266.reset();
                                do{
                                    Result result_peg_1278(result_peg_1266.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1278.getPosition()))){
                                                result_peg_1278.nextPosition();
                                            } else {
                                                goto out_peg_1281;
                                            }
                                        }
                                        result_peg_1278.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1279;
                                    out_peg_1281:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1278.getPosition()))){
                                                result_peg_1278.nextPosition();
                                            } else {
                                                goto out_peg_1283;
                                            }
                                        }
                                        result_peg_1278.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1279;
                                    out_peg_1283:
                                    goto loop_peg_1277;
                                    success_peg_1279:
                                    ;
                                    result_peg_1266.addResult(result_peg_1278);
                                } while (true);
                                loop_peg_1277:
                                ;
                                        
                            }
                            goto success_peg_1268;
                            goto loop_peg_1265;
                            success_peg_1268:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1266.getPosition()))){
                                    result_peg_1266.nextPosition();
                                } else {
                                    goto loop_peg_1265;
                                }
                            }
                            result_peg_1266.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1266.reset();
                                do{
                                    Result result_peg_1296(result_peg_1266.getPosition());
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
                                    result_peg_1266.addResult(result_peg_1296);
                                } while (true);
                                loop_peg_1295:
                                ;
                                        
                            }
                            goto success_peg_1286;
                            goto loop_peg_1265;
                            success_peg_1286:
                            ;
                        
                        
                        
                        result_peg_1266 = rule_expr_c(stream, result_peg_1266.getPosition());
                            if (result_peg_1266.error()){
                                goto loop_peg_1265;
                            }
                        
                        
                    }
                    result_peg_1226.addResult(result_peg_1266);
                } while (true);
                loop_peg_1265:
                ;
                rest = result_peg_1226.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_1226.setValue(value);
                }
            
            
        }
        
        if (column_peg_1184.chunk3 == 0){
            column_peg_1184.chunk3 = new Chunk3();
        }
        column_peg_1184.chunk3->chunk_valuelist = result_peg_1226;
        stream.update(result_peg_1226.getPosition());
        
        
        return result_peg_1226;
        out_peg_1244:
    
        if (column_peg_1184.chunk3 == 0){
            column_peg_1184.chunk3 = new Chunk3();
        }
        column_peg_1184.chunk3->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_1303 = stream.getColumn(position);
    if (column_peg_1303.chunk3 != 0 && column_peg_1303.chunk3->chunk_expr.calculated()){
        return column_peg_1303.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_1330(stream, "expr");
    int myposition = position;
    
    
    
    Result result_peg_1304(myposition);
        
        {
        
            result_peg_1304 = rule_expr_c(stream, result_peg_1304.getPosition());
                if (result_peg_1304.error()){
                    goto out_peg_1306;
                }
            
            Result result_peg_1305 = result_peg_1304;
            
            Result result_peg_1309(result_peg_1304);
                {
                
                    {
                            
                            result_peg_1309.reset();
                            do{
                                Result result_peg_1321(result_peg_1309.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1321.getPosition()))){
                                            result_peg_1321.nextPosition();
                                        } else {
                                            goto out_peg_1324;
                                        }
                                    }
                                    result_peg_1321.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1322;
                                out_peg_1324:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1321.getPosition()))){
                                            result_peg_1321.nextPosition();
                                        } else {
                                            goto out_peg_1326;
                                        }
                                    }
                                    result_peg_1321.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1322;
                                out_peg_1326:
                                goto loop_peg_1320;
                                success_peg_1322:
                                ;
                                result_peg_1309.addResult(result_peg_1321);
                            } while (true);
                            loop_peg_1320:
                            ;
                                    
                        }
                        goto success_peg_1311;
                        goto not_peg_1308;
                        success_peg_1311:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1309.getPosition()))){
                                result_peg_1309.nextPosition();
                            } else {
                                goto not_peg_1308;
                            }
                        }
                        result_peg_1309.setValue((void*) ",");
                    
                    
                }
                goto out_peg_1306;
                not_peg_1308:
                result_peg_1304.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_1305.getValues();
                    result_peg_1304.setValue(value);
                }
            
            
        }
        
        if (column_peg_1303.chunk3 == 0){
            column_peg_1303.chunk3 = new Chunk3();
        }
        column_peg_1303.chunk3->chunk_expr = result_peg_1304;
        stream.update(result_peg_1304.getPosition());
        
        
        return result_peg_1304;
        out_peg_1306:
        Result result_peg_1327(myposition);
        
        {
        
            result_peg_1327 = rule_valuelist(stream, result_peg_1327.getPosition());
                if (result_peg_1327.error()){
                    goto out_peg_1329;
                }
            
            Result result_peg_1328 = result_peg_1327;
            
            result_peg_1327 = rule_expr2_rest(stream, result_peg_1327.getPosition(), result_peg_1328.getValues());
                if (result_peg_1327.error()){
                    goto out_peg_1329;
                }
            
            
        }
        
        if (column_peg_1303.chunk3 == 0){
            column_peg_1303.chunk3 = new Chunk3();
        }
        column_peg_1303.chunk3->chunk_expr = result_peg_1327;
        stream.update(result_peg_1327.getPosition());
        
        
        return result_peg_1327;
        out_peg_1329:
    
        if (column_peg_1303.chunk3 == 0){
            column_peg_1303.chunk3 = new Chunk3();
        }
        column_peg_1303.chunk3->chunk_expr = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_1331 = stream.getColumn(position);
    if (column_peg_1331.chunk3 != 0 && column_peg_1331.chunk3->chunk_expr_c.calculated()){
        return column_peg_1331.chunk3->chunk_expr_c;
    }
    
    RuleTrace trace_peg_1335(stream, "expr_c");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1332(myposition);
        
        {
        
            result_peg_1332 = rule_expr2(stream, result_peg_1332.getPosition());
                if (result_peg_1332.error()){
                    goto out_peg_1334;
                }
                left = result_peg_1332.getValues();
            
            
            
            result_peg_1332 = rule_expr_rest(stream, result_peg_1332.getPosition(), left);
                if (result_peg_1332.error()){
                    goto out_peg_1334;
                }
            
            
        }
        
        if (column_peg_1331.chunk3 == 0){
            column_peg_1331.chunk3 = new Chunk3();
        }
        column_peg_1331.chunk3->chunk_expr_c = result_peg_1332;
        stream.update(result_peg_1332.getPosition());
        
        
        return result_peg_1332;
        out_peg_1334:
    
        if (column_peg_1331.chunk3 == 0){
            column_peg_1331.chunk3 = new Chunk3();
        }
        column_peg_1331.chunk3->chunk_expr_c = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_1336 = stream.getColumn(position);
    if (column_peg_1336.chunk4 != 0 && column_peg_1336.chunk4->chunk_expr2.calculated()){
        return column_peg_1336.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_1340(stream, "expr2");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1337(myposition);
        
        {
        
            result_peg_1337 = rule_expr3(stream, result_peg_1337.getPosition());
                if (result_peg_1337.error()){
                    goto out_peg_1339;
                }
                left = result_peg_1337.getValues();
            
            
            
            result_peg_1337 = rule_expr2_rest(stream, result_peg_1337.getPosition(), left);
                if (result_peg_1337.error()){
                    goto out_peg_1339;
                }
            
            
        }
        
        if (column_peg_1336.chunk4 == 0){
            column_peg_1336.chunk4 = new Chunk4();
        }
        column_peg_1336.chunk4->chunk_expr2 = result_peg_1337;
        stream.update(result_peg_1337.getPosition());
        
        
        return result_peg_1337;
        out_peg_1339:
    
        if (column_peg_1336.chunk4 == 0){
            column_peg_1336.chunk4 = new Chunk4();
        }
        column_peg_1336.chunk4->chunk_expr2 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_1341 = stream.getColumn(position);
    if (column_peg_1341.chunk4 != 0 && column_peg_1341.chunk4->chunk_expr3.calculated()){
        return column_peg_1341.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_1345(stream, "expr3");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1342(myposition);
        
        {
        
            result_peg_1342 = rule_expr4(stream, result_peg_1342.getPosition());
                if (result_peg_1342.error()){
                    goto out_peg_1344;
                }
                left = result_peg_1342.getValues();
            
            
            
            result_peg_1342 = rule_expr3_rest(stream, result_peg_1342.getPosition(), left);
                if (result_peg_1342.error()){
                    goto out_peg_1344;
                }
            
            
        }
        
        if (column_peg_1341.chunk4 == 0){
            column_peg_1341.chunk4 = new Chunk4();
        }
        column_peg_1341.chunk4->chunk_expr3 = result_peg_1342;
        stream.update(result_peg_1342.getPosition());
        
        
        return result_peg_1342;
        out_peg_1344:
    
        if (column_peg_1341.chunk4 == 0){
            column_peg_1341.chunk4 = new Chunk4();
        }
        column_peg_1341.chunk4->chunk_expr3 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_1346 = stream.getColumn(position);
    if (column_peg_1346.chunk4 != 0 && column_peg_1346.chunk4->chunk_expr4.calculated()){
        return column_peg_1346.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_1350(stream, "expr4");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1347(myposition);
        
        {
        
            result_peg_1347 = rule_expr5(stream, result_peg_1347.getPosition());
                if (result_peg_1347.error()){
                    goto out_peg_1349;
                }
                left = result_peg_1347.getValues();
            
            
            
            result_peg_1347 = rule_expr4_rest(stream, result_peg_1347.getPosition(), left);
                if (result_peg_1347.error()){
                    goto out_peg_1349;
                }
            
            
        }
        
        if (column_peg_1346.chunk4 == 0){
            column_peg_1346.chunk4 = new Chunk4();
        }
        column_peg_1346.chunk4->chunk_expr4 = result_peg_1347;
        stream.update(result_peg_1347.getPosition());
        
        
        return result_peg_1347;
        out_peg_1349:
    
        if (column_peg_1346.chunk4 == 0){
            column_peg_1346.chunk4 = new Chunk4();
        }
        column_peg_1346.chunk4->chunk_expr4 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_1351 = stream.getColumn(position);
    if (column_peg_1351.chunk4 != 0 && column_peg_1351.chunk4->chunk_expr5.calculated()){
        return column_peg_1351.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_1355(stream, "expr5");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1352(myposition);
        
        {
        
            result_peg_1352 = rule_expr6(stream, result_peg_1352.getPosition());
                if (result_peg_1352.error()){
                    goto out_peg_1354;
                }
                left = result_peg_1352.getValues();
            
            
            
            result_peg_1352 = rule_expr5_rest(stream, result_peg_1352.getPosition(), left);
                if (result_peg_1352.error()){
                    goto out_peg_1354;
                }
            
            
        }
        
        if (column_peg_1351.chunk4 == 0){
            column_peg_1351.chunk4 = new Chunk4();
        }
        column_peg_1351.chunk4->chunk_expr5 = result_peg_1352;
        stream.update(result_peg_1352.getPosition());
        
        
        return result_peg_1352;
        out_peg_1354:
    
        if (column_peg_1351.chunk4 == 0){
            column_peg_1351.chunk4 = new Chunk4();
        }
        column_peg_1351.chunk4->chunk_expr5 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_1356 = stream.getColumn(position);
    if (column_peg_1356.chunk4 != 0 && column_peg_1356.chunk4->chunk_expr6.calculated()){
        return column_peg_1356.chunk4->chunk_expr6;
    }
    
    RuleTrace trace_peg_1360(stream, "expr6");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1357(myposition);
        
        {
        
            result_peg_1357 = rule_expr7(stream, result_peg_1357.getPosition());
                if (result_peg_1357.error()){
                    goto out_peg_1359;
                }
                left = result_peg_1357.getValues();
            
            
            
            result_peg_1357 = rule_expr6_rest(stream, result_peg_1357.getPosition(), left);
                if (result_peg_1357.error()){
                    goto out_peg_1359;
                }
            
            
        }
        
        if (column_peg_1356.chunk4 == 0){
            column_peg_1356.chunk4 = new Chunk4();
        }
        column_peg_1356.chunk4->chunk_expr6 = result_peg_1357;
        stream.update(result_peg_1357.getPosition());
        
        
        return result_peg_1357;
        out_peg_1359:
    
        if (column_peg_1356.chunk4 == 0){
            column_peg_1356.chunk4 = new Chunk4();
        }
        column_peg_1356.chunk4->chunk_expr6 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_1361 = stream.getColumn(position);
    if (column_peg_1361.chunk5 != 0 && column_peg_1361.chunk5->chunk_expr7.calculated()){
        return column_peg_1361.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_1365(stream, "expr7");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1362(myposition);
        
        {
        
            result_peg_1362 = rule_expr8(stream, result_peg_1362.getPosition());
                if (result_peg_1362.error()){
                    goto out_peg_1364;
                }
                left = result_peg_1362.getValues();
            
            
            
            result_peg_1362 = rule_expr7_rest(stream, result_peg_1362.getPosition(), left);
                if (result_peg_1362.error()){
                    goto out_peg_1364;
                }
            
            
        }
        
        if (column_peg_1361.chunk5 == 0){
            column_peg_1361.chunk5 = new Chunk5();
        }
        column_peg_1361.chunk5->chunk_expr7 = result_peg_1362;
        stream.update(result_peg_1362.getPosition());
        
        
        return result_peg_1362;
        out_peg_1364:
    
        if (column_peg_1361.chunk5 == 0){
            column_peg_1361.chunk5 = new Chunk5();
        }
        column_peg_1361.chunk5->chunk_expr7 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_1366 = stream.getColumn(position);
    if (column_peg_1366.chunk5 != 0 && column_peg_1366.chunk5->chunk_expr8.calculated()){
        return column_peg_1366.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_1370(stream, "expr8");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1367(myposition);
        
        {
        
            result_peg_1367 = rule_expr9(stream, result_peg_1367.getPosition());
                if (result_peg_1367.error()){
                    goto out_peg_1369;
                }
                left = result_peg_1367.getValues();
            
            
            
            result_peg_1367 = rule_expr8_rest(stream, result_peg_1367.getPosition(), left);
                if (result_peg_1367.error()){
                    goto out_peg_1369;
                }
            
            
        }
        
        if (column_peg_1366.chunk5 == 0){
            column_peg_1366.chunk5 = new Chunk5();
        }
        column_peg_1366.chunk5->chunk_expr8 = result_peg_1367;
        stream.update(result_peg_1367.getPosition());
        
        
        return result_peg_1367;
        out_peg_1369:
    
        if (column_peg_1366.chunk5 == 0){
            column_peg_1366.chunk5 = new Chunk5();
        }
        column_peg_1366.chunk5->chunk_expr8 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_1371 = stream.getColumn(position);
    if (column_peg_1371.chunk5 != 0 && column_peg_1371.chunk5->chunk_expr9.calculated()){
        return column_peg_1371.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_1375(stream, "expr9");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1372(myposition);
        
        {
        
            result_peg_1372 = rule_expr10(stream, result_peg_1372.getPosition());
                if (result_peg_1372.error()){
                    goto out_peg_1374;
                }
                left = result_peg_1372.getValues();
            
            
            
            result_peg_1372 = rule_expr9_rest(stream, result_peg_1372.getPosition(), left);
                if (result_peg_1372.error()){
                    goto out_peg_1374;
                }
            
            
        }
        
        if (column_peg_1371.chunk5 == 0){
            column_peg_1371.chunk5 = new Chunk5();
        }
        column_peg_1371.chunk5->chunk_expr9 = result_peg_1372;
        stream.update(result_peg_1372.getPosition());
        
        
        return result_peg_1372;
        out_peg_1374:
    
        if (column_peg_1371.chunk5 == 0){
            column_peg_1371.chunk5 = new Chunk5();
        }
        column_peg_1371.chunk5->chunk_expr9 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_1376 = stream.getColumn(position);
    if (column_peg_1376.chunk5 != 0 && column_peg_1376.chunk5->chunk_expr10.calculated()){
        return column_peg_1376.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_1380(stream, "expr10");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1377(myposition);
        
        {
        
            result_peg_1377 = rule_expr11(stream, result_peg_1377.getPosition());
                if (result_peg_1377.error()){
                    goto out_peg_1379;
                }
                left = result_peg_1377.getValues();
            
            
            
            result_peg_1377 = rule_expr10_rest(stream, result_peg_1377.getPosition(), left);
                if (result_peg_1377.error()){
                    goto out_peg_1379;
                }
            
            
        }
        
        if (column_peg_1376.chunk5 == 0){
            column_peg_1376.chunk5 = new Chunk5();
        }
        column_peg_1376.chunk5->chunk_expr10 = result_peg_1377;
        stream.update(result_peg_1377.getPosition());
        
        
        return result_peg_1377;
        out_peg_1379:
    
        if (column_peg_1376.chunk5 == 0){
            column_peg_1376.chunk5 = new Chunk5();
        }
        column_peg_1376.chunk5->chunk_expr10 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_1381 = stream.getColumn(position);
    if (column_peg_1381.chunk5 != 0 && column_peg_1381.chunk5->chunk_expr11.calculated()){
        return column_peg_1381.chunk5->chunk_expr11;
    }
    
    RuleTrace trace_peg_1385(stream, "expr11");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1382(myposition);
        
        {
        
            result_peg_1382 = rule_expr12(stream, result_peg_1382.getPosition());
                if (result_peg_1382.error()){
                    goto out_peg_1384;
                }
                left = result_peg_1382.getValues();
            
            
            
            result_peg_1382 = rule_expr11_rest(stream, result_peg_1382.getPosition(), left);
                if (result_peg_1382.error()){
                    goto out_peg_1384;
                }
            
            
        }
        
        if (column_peg_1381.chunk5 == 0){
            column_peg_1381.chunk5 = new Chunk5();
        }
        column_peg_1381.chunk5->chunk_expr11 = result_peg_1382;
        stream.update(result_peg_1382.getPosition());
        
        
        return result_peg_1382;
        out_peg_1384:
    
        if (column_peg_1381.chunk5 == 0){
            column_peg_1381.chunk5 = new Chunk5();
        }
        column_peg_1381.chunk5->chunk_expr11 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_1386 = stream.getColumn(position);
    if (column_peg_1386.chunk6 != 0 && column_peg_1386.chunk6->chunk_expr12.calculated()){
        return column_peg_1386.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_1390(stream, "expr12");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1387(myposition);
        
        {
        
            result_peg_1387 = rule_expr13(stream, result_peg_1387.getPosition());
                if (result_peg_1387.error()){
                    goto out_peg_1389;
                }
                left = result_peg_1387.getValues();
            
            
            
            result_peg_1387 = rule_expr12_rest(stream, result_peg_1387.getPosition(), left);
                if (result_peg_1387.error()){
                    goto out_peg_1389;
                }
            
            
        }
        
        if (column_peg_1386.chunk6 == 0){
            column_peg_1386.chunk6 = new Chunk6();
        }
        column_peg_1386.chunk6->chunk_expr12 = result_peg_1387;
        stream.update(result_peg_1387.getPosition());
        
        
        return result_peg_1387;
        out_peg_1389:
    
        if (column_peg_1386.chunk6 == 0){
            column_peg_1386.chunk6 = new Chunk6();
        }
        column_peg_1386.chunk6->chunk_expr12 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_1391 = stream.getColumn(position);
    if (column_peg_1391.chunk6 != 0 && column_peg_1391.chunk6->chunk_expr13.calculated()){
        return column_peg_1391.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_1415(stream, "expr13");
    int myposition = position;
    
    
    Value exp;
    Result result_peg_1392(myposition);
        
        {
        
            result_peg_1392.reset();
                do{
                    Result result_peg_1395(result_peg_1392.getPosition());
                    result_peg_1395 = rule_unary(stream, result_peg_1395.getPosition());
                    if (result_peg_1395.error()){
                        goto loop_peg_1394;
                    }
                    result_peg_1392.addResult(result_peg_1395);
                } while (true);
                loop_peg_1394:
                ;
            
            Result result_peg_1393 = result_peg_1392;
            
            {
                    
                    result_peg_1392.reset();
                    do{
                        Result result_peg_1407(result_peg_1392.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1407.getPosition()))){
                                    result_peg_1407.nextPosition();
                                } else {
                                    goto out_peg_1410;
                                }
                            }
                            result_peg_1407.setValue((void*) " ");
                                
                        }
                        goto success_peg_1408;
                        out_peg_1410:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1407.getPosition()))){
                                    result_peg_1407.nextPosition();
                                } else {
                                    goto out_peg_1412;
                                }
                            }
                            result_peg_1407.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1408;
                        out_peg_1412:
                        goto loop_peg_1406;
                        success_peg_1408:
                        ;
                        result_peg_1392.addResult(result_peg_1407);
                    } while (true);
                    loop_peg_1406:
                    ;
                            
                }
                goto success_peg_1397;
                goto out_peg_1413;
                success_peg_1397:
                ;
            
            
            
            result_peg_1392 = rule_expr13_real(stream, result_peg_1392.getPosition());
                if (result_peg_1392.error()){
                    goto out_peg_1413;
                }
                exp = result_peg_1392.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeUnaryExpression(result_peg_1393.getValues(), exp);
                    result_peg_1392.setValue(value);
                }
            
            
        }
        
        if (column_peg_1391.chunk6 == 0){
            column_peg_1391.chunk6 = new Chunk6();
        }
        column_peg_1391.chunk6->chunk_expr13 = result_peg_1392;
        stream.update(result_peg_1392.getPosition());
        
        
        return result_peg_1392;
        out_peg_1413:
    
        if (column_peg_1391.chunk6 == 0){
            column_peg_1391.chunk6 = new Chunk6();
        }
        column_peg_1391.chunk6->chunk_expr13 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1459(stream, "expr_rest");
    int myposition = position;
    
    tail_peg_1418:
    Value right;
        Value new_left;
    Result result_peg_1417(myposition);
        {
        
            {
                    
                    result_peg_1417.reset();
                    do{
                        Result result_peg_1430(result_peg_1417.getPosition());
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
                        result_peg_1417.addResult(result_peg_1430);
                    } while (true);
                    loop_peg_1429:
                    ;
                            
                }
                goto success_peg_1420;
                goto out_peg_1436;
                success_peg_1420:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("||"[i], stream.get(result_peg_1417.getPosition()))){
                        result_peg_1417.nextPosition();
                    } else {
                        goto out_peg_1436;
                    }
                }
                result_peg_1417.setValue((void*) "||");
            
            
            
            {
                    
                    result_peg_1417.reset();
                    do{
                        Result result_peg_1449(result_peg_1417.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1449.getPosition()))){
                                    result_peg_1449.nextPosition();
                                } else {
                                    goto out_peg_1452;
                                }
                            }
                            result_peg_1449.setValue((void*) " ");
                                
                        }
                        goto success_peg_1450;
                        out_peg_1452:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1449.getPosition()))){
                                    result_peg_1449.nextPosition();
                                } else {
                                    goto out_peg_1454;
                                }
                            }
                            result_peg_1449.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1450;
                        out_peg_1454:
                        goto loop_peg_1448;
                        success_peg_1450:
                        ;
                        result_peg_1417.addResult(result_peg_1449);
                    } while (true);
                    loop_peg_1448:
                    ;
                            
                }
                goto success_peg_1439;
                goto out_peg_1436;
                success_peg_1439:
                ;
            
            
            
            result_peg_1417 = rule_expr2(stream, result_peg_1417.getPosition());
                if (result_peg_1417.error()){
                    goto out_peg_1436;
                }
                right = result_peg_1417.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionOr(left, right);
                    result_peg_1417.setValue(value);
                }
                new_left = result_peg_1417.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1417.getPosition();
        goto tail_peg_1418;
        out_peg_1436:
        Result result_peg_1457(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1457.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1457.getPosition());
        
        return result_peg_1457;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1503(stream, "expr2_rest");
    int myposition = position;
    
    tail_peg_1462:
    Value right;
        Value new_left;
    Result result_peg_1461(myposition);
        {
        
            {
                    
                    result_peg_1461.reset();
                    do{
                        Result result_peg_1474(result_peg_1461.getPosition());
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
                        result_peg_1461.addResult(result_peg_1474);
                    } while (true);
                    loop_peg_1473:
                    ;
                            
                }
                goto success_peg_1464;
                goto out_peg_1480;
                success_peg_1464:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("^^"[i], stream.get(result_peg_1461.getPosition()))){
                        result_peg_1461.nextPosition();
                    } else {
                        goto out_peg_1480;
                    }
                }
                result_peg_1461.setValue((void*) "^^");
            
            
            
            {
                    
                    result_peg_1461.reset();
                    do{
                        Result result_peg_1493(result_peg_1461.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1493.getPosition()))){
                                    result_peg_1493.nextPosition();
                                } else {
                                    goto out_peg_1496;
                                }
                            }
                            result_peg_1493.setValue((void*) " ");
                                
                        }
                        goto success_peg_1494;
                        out_peg_1496:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1493.getPosition()))){
                                    result_peg_1493.nextPosition();
                                } else {
                                    goto out_peg_1498;
                                }
                            }
                            result_peg_1493.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1494;
                        out_peg_1498:
                        goto loop_peg_1492;
                        success_peg_1494:
                        ;
                        result_peg_1461.addResult(result_peg_1493);
                    } while (true);
                    loop_peg_1492:
                    ;
                            
                }
                goto success_peg_1483;
                goto out_peg_1480;
                success_peg_1483:
                ;
            
            
            
            result_peg_1461 = rule_expr3(stream, result_peg_1461.getPosition());
                if (result_peg_1461.error()){
                    goto out_peg_1480;
                }
                right = result_peg_1461.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionXOr(left, right);
                    result_peg_1461.setValue(value);
                }
                new_left = result_peg_1461.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1461.getPosition();
        goto tail_peg_1462;
        out_peg_1480:
        Result result_peg_1501(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1501.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1501.getPosition());
        
        return result_peg_1501;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1547(stream, "expr3_rest");
    int myposition = position;
    
    tail_peg_1506:
    Value right;
        Value new_left;
    Result result_peg_1505(myposition);
        {
        
            {
                    
                    result_peg_1505.reset();
                    do{
                        Result result_peg_1518(result_peg_1505.getPosition());
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
                        result_peg_1505.addResult(result_peg_1518);
                    } while (true);
                    loop_peg_1517:
                    ;
                            
                }
                goto success_peg_1508;
                goto out_peg_1524;
                success_peg_1508:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("&&"[i], stream.get(result_peg_1505.getPosition()))){
                        result_peg_1505.nextPosition();
                    } else {
                        goto out_peg_1524;
                    }
                }
                result_peg_1505.setValue((void*) "&&");
            
            
            
            {
                    
                    result_peg_1505.reset();
                    do{
                        Result result_peg_1537(result_peg_1505.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1537.getPosition()))){
                                    result_peg_1537.nextPosition();
                                } else {
                                    goto out_peg_1540;
                                }
                            }
                            result_peg_1537.setValue((void*) " ");
                                
                        }
                        goto success_peg_1538;
                        out_peg_1540:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1537.getPosition()))){
                                    result_peg_1537.nextPosition();
                                } else {
                                    goto out_peg_1542;
                                }
                            }
                            result_peg_1537.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1538;
                        out_peg_1542:
                        goto loop_peg_1536;
                        success_peg_1538:
                        ;
                        result_peg_1505.addResult(result_peg_1537);
                    } while (true);
                    loop_peg_1536:
                    ;
                            
                }
                goto success_peg_1527;
                goto out_peg_1524;
                success_peg_1527:
                ;
            
            
            
            result_peg_1505 = rule_expr4(stream, result_peg_1505.getPosition());
                if (result_peg_1505.error()){
                    goto out_peg_1524;
                }
                right = result_peg_1505.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAnd(left, right);
                    result_peg_1505.setValue(value);
                }
                new_left = result_peg_1505.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1505.getPosition();
        goto tail_peg_1506;
        out_peg_1524:
        Result result_peg_1545(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1545.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1545.getPosition());
        
        return result_peg_1545;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1591(stream, "expr4_rest");
    int myposition = position;
    
    tail_peg_1550:
    Value right;
        Value new_left;
    Result result_peg_1549(myposition);
        {
        
            {
                    
                    result_peg_1549.reset();
                    do{
                        Result result_peg_1562(result_peg_1549.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1562.getPosition()))){
                                    result_peg_1562.nextPosition();
                                } else {
                                    goto out_peg_1565;
                                }
                            }
                            result_peg_1562.setValue((void*) " ");
                                
                        }
                        goto success_peg_1563;
                        out_peg_1565:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1562.getPosition()))){
                                    result_peg_1562.nextPosition();
                                } else {
                                    goto out_peg_1567;
                                }
                            }
                            result_peg_1562.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1563;
                        out_peg_1567:
                        goto loop_peg_1561;
                        success_peg_1563:
                        ;
                        result_peg_1549.addResult(result_peg_1562);
                    } while (true);
                    loop_peg_1561:
                    ;
                            
                }
                goto success_peg_1552;
                goto out_peg_1568;
                success_peg_1552:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("|"[i], stream.get(result_peg_1549.getPosition()))){
                        result_peg_1549.nextPosition();
                    } else {
                        goto out_peg_1568;
                    }
                }
                result_peg_1549.setValue((void*) "|");
            
            
            
            {
                    
                    result_peg_1549.reset();
                    do{
                        Result result_peg_1581(result_peg_1549.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1581.getPosition()))){
                                    result_peg_1581.nextPosition();
                                } else {
                                    goto out_peg_1584;
                                }
                            }
                            result_peg_1581.setValue((void*) " ");
                                
                        }
                        goto success_peg_1582;
                        out_peg_1584:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1581.getPosition()))){
                                    result_peg_1581.nextPosition();
                                } else {
                                    goto out_peg_1586;
                                }
                            }
                            result_peg_1581.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1582;
                        out_peg_1586:
                        goto loop_peg_1580;
                        success_peg_1582:
                        ;
                        result_peg_1549.addResult(result_peg_1581);
                    } while (true);
                    loop_peg_1580:
                    ;
                            
                }
                goto success_peg_1571;
                goto out_peg_1568;
                success_peg_1571:
                ;
            
            
            
            result_peg_1549 = rule_expr5(stream, result_peg_1549.getPosition());
                if (result_peg_1549.error()){
                    goto out_peg_1568;
                }
                right = result_peg_1549.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseOr(left, right);
                    result_peg_1549.setValue(value);
                }
                new_left = result_peg_1549.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1549.getPosition();
        goto tail_peg_1550;
        out_peg_1568:
        Result result_peg_1589(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1589.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1589.getPosition());
        
        return result_peg_1589;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1635(stream, "expr5_rest");
    int myposition = position;
    
    tail_peg_1594:
    Value right;
        Value new_left;
    Result result_peg_1593(myposition);
        {
        
            {
                    
                    result_peg_1593.reset();
                    do{
                        Result result_peg_1606(result_peg_1593.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1606.getPosition()))){
                                    result_peg_1606.nextPosition();
                                } else {
                                    goto out_peg_1609;
                                }
                            }
                            result_peg_1606.setValue((void*) " ");
                                
                        }
                        goto success_peg_1607;
                        out_peg_1609:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1606.getPosition()))){
                                    result_peg_1606.nextPosition();
                                } else {
                                    goto out_peg_1611;
                                }
                            }
                            result_peg_1606.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1607;
                        out_peg_1611:
                        goto loop_peg_1605;
                        success_peg_1607:
                        ;
                        result_peg_1593.addResult(result_peg_1606);
                    } while (true);
                    loop_peg_1605:
                    ;
                            
                }
                goto success_peg_1596;
                goto out_peg_1612;
                success_peg_1596:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("^"[i], stream.get(result_peg_1593.getPosition()))){
                        result_peg_1593.nextPosition();
                    } else {
                        goto out_peg_1612;
                    }
                }
                result_peg_1593.setValue((void*) "^");
            
            
            
            {
                    
                    result_peg_1593.reset();
                    do{
                        Result result_peg_1625(result_peg_1593.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1625.getPosition()))){
                                    result_peg_1625.nextPosition();
                                } else {
                                    goto out_peg_1628;
                                }
                            }
                            result_peg_1625.setValue((void*) " ");
                                
                        }
                        goto success_peg_1626;
                        out_peg_1628:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1625.getPosition()))){
                                    result_peg_1625.nextPosition();
                                } else {
                                    goto out_peg_1630;
                                }
                            }
                            result_peg_1625.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1626;
                        out_peg_1630:
                        goto loop_peg_1624;
                        success_peg_1626:
                        ;
                        result_peg_1593.addResult(result_peg_1625);
                    } while (true);
                    loop_peg_1624:
                    ;
                            
                }
                goto success_peg_1615;
                goto out_peg_1612;
                success_peg_1615:
                ;
            
            
            
            result_peg_1593 = rule_expr6(stream, result_peg_1593.getPosition());
                if (result_peg_1593.error()){
                    goto out_peg_1612;
                }
                right = result_peg_1593.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseXOr(left, right);
                    result_peg_1593.setValue(value);
                }
                new_left = result_peg_1593.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1593.getPosition();
        goto tail_peg_1594;
        out_peg_1612:
        Result result_peg_1633(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1633.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1633.getPosition());
        
        return result_peg_1633;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1679(stream, "expr6_rest");
    int myposition = position;
    
    tail_peg_1638:
    Value right;
        Value new_left;
    Result result_peg_1637(myposition);
        {
        
            {
                    
                    result_peg_1637.reset();
                    do{
                        Result result_peg_1650(result_peg_1637.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1650.getPosition()))){
                                    result_peg_1650.nextPosition();
                                } else {
                                    goto out_peg_1653;
                                }
                            }
                            result_peg_1650.setValue((void*) " ");
                                
                        }
                        goto success_peg_1651;
                        out_peg_1653:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1650.getPosition()))){
                                    result_peg_1650.nextPosition();
                                } else {
                                    goto out_peg_1655;
                                }
                            }
                            result_peg_1650.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1651;
                        out_peg_1655:
                        goto loop_peg_1649;
                        success_peg_1651:
                        ;
                        result_peg_1637.addResult(result_peg_1650);
                    } while (true);
                    loop_peg_1649:
                    ;
                            
                }
                goto success_peg_1640;
                goto out_peg_1656;
                success_peg_1640:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("&"[i], stream.get(result_peg_1637.getPosition()))){
                        result_peg_1637.nextPosition();
                    } else {
                        goto out_peg_1656;
                    }
                }
                result_peg_1637.setValue((void*) "&");
            
            
            
            {
                    
                    result_peg_1637.reset();
                    do{
                        Result result_peg_1669(result_peg_1637.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1669.getPosition()))){
                                    result_peg_1669.nextPosition();
                                } else {
                                    goto out_peg_1672;
                                }
                            }
                            result_peg_1669.setValue((void*) " ");
                                
                        }
                        goto success_peg_1670;
                        out_peg_1672:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1669.getPosition()))){
                                    result_peg_1669.nextPosition();
                                } else {
                                    goto out_peg_1674;
                                }
                            }
                            result_peg_1669.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1670;
                        out_peg_1674:
                        goto loop_peg_1668;
                        success_peg_1670:
                        ;
                        result_peg_1637.addResult(result_peg_1669);
                    } while (true);
                    loop_peg_1668:
                    ;
                            
                }
                goto success_peg_1659;
                goto out_peg_1656;
                success_peg_1659:
                ;
            
            
            
            result_peg_1637 = rule_expr7(stream, result_peg_1637.getPosition());
                if (result_peg_1637.error()){
                    goto out_peg_1656;
                }
                right = result_peg_1637.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseAnd(left, right);
                    result_peg_1637.setValue(value);
                }
                new_left = result_peg_1637.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1637.getPosition();
        goto tail_peg_1638;
        out_peg_1656:
        Result result_peg_1677(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1677.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1677.getPosition());
        
        return result_peg_1677;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1723(stream, "expr7_rest");
    int myposition = position;
    
    tail_peg_1682:
    Value right;
        Value new_left;
    Result result_peg_1681(myposition);
        {
        
            {
                    
                    result_peg_1681.reset();
                    do{
                        Result result_peg_1694(result_peg_1681.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1694.getPosition()))){
                                    result_peg_1694.nextPosition();
                                } else {
                                    goto out_peg_1697;
                                }
                            }
                            result_peg_1694.setValue((void*) " ");
                                
                        }
                        goto success_peg_1695;
                        out_peg_1697:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1694.getPosition()))){
                                    result_peg_1694.nextPosition();
                                } else {
                                    goto out_peg_1699;
                                }
                            }
                            result_peg_1694.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1695;
                        out_peg_1699:
                        goto loop_peg_1693;
                        success_peg_1695:
                        ;
                        result_peg_1681.addResult(result_peg_1694);
                    } while (true);
                    loop_peg_1693:
                    ;
                            
                }
                goto success_peg_1684;
                goto out_peg_1700;
                success_peg_1684:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar(":="[i], stream.get(result_peg_1681.getPosition()))){
                        result_peg_1681.nextPosition();
                    } else {
                        goto out_peg_1700;
                    }
                }
                result_peg_1681.setValue((void*) ":=");
            
            
            
            {
                    
                    result_peg_1681.reset();
                    do{
                        Result result_peg_1713(result_peg_1681.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1713.getPosition()))){
                                    result_peg_1713.nextPosition();
                                } else {
                                    goto out_peg_1716;
                                }
                            }
                            result_peg_1713.setValue((void*) " ");
                                
                        }
                        goto success_peg_1714;
                        out_peg_1716:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1713.getPosition()))){
                                    result_peg_1713.nextPosition();
                                } else {
                                    goto out_peg_1718;
                                }
                            }
                            result_peg_1713.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1714;
                        out_peg_1718:
                        goto loop_peg_1712;
                        success_peg_1714:
                        ;
                        result_peg_1681.addResult(result_peg_1713);
                    } while (true);
                    loop_peg_1712:
                    ;
                            
                }
                goto success_peg_1703;
                goto out_peg_1700;
                success_peg_1703:
                ;
            
            
            
            result_peg_1681 = rule_expr8(stream, result_peg_1681.getPosition());
                if (result_peg_1681.error()){
                    goto out_peg_1700;
                }
                right = result_peg_1681.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAssignment(left, right);
                    result_peg_1681.setValue(value);
                }
                new_left = result_peg_1681.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1681.getPosition();
        goto tail_peg_1682;
        out_peg_1700:
        Result result_peg_1721(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1721.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1721.getPosition());
        
        return result_peg_1721;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1806(stream, "expr8_rest");
    int myposition = position;
    
    tail_peg_1726:
    Value right;
        Value new_left;
    Result result_peg_1725(myposition);
        {
        
            {
                    
                    result_peg_1725.reset();
                    do{
                        Result result_peg_1738(result_peg_1725.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1738.getPosition()))){
                                    result_peg_1738.nextPosition();
                                } else {
                                    goto out_peg_1741;
                                }
                            }
                            result_peg_1738.setValue((void*) " ");
                                
                        }
                        goto success_peg_1739;
                        out_peg_1741:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1738.getPosition()))){
                                    result_peg_1738.nextPosition();
                                } else {
                                    goto out_peg_1743;
                                }
                            }
                            result_peg_1738.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1739;
                        out_peg_1743:
                        goto loop_peg_1737;
                        success_peg_1739:
                        ;
                        result_peg_1725.addResult(result_peg_1738);
                    } while (true);
                    loop_peg_1737:
                    ;
                            
                }
                goto success_peg_1728;
                goto out_peg_1744;
                success_peg_1728:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1725.getPosition()))){
                        result_peg_1725.nextPosition();
                    } else {
                        goto out_peg_1744;
                    }
                }
                result_peg_1725.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1725.reset();
                    do{
                        Result result_peg_1757(result_peg_1725.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1757.getPosition()))){
                                    result_peg_1757.nextPosition();
                                } else {
                                    goto out_peg_1760;
                                }
                            }
                            result_peg_1757.setValue((void*) " ");
                                
                        }
                        goto success_peg_1758;
                        out_peg_1760:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1757.getPosition()))){
                                    result_peg_1757.nextPosition();
                                } else {
                                    goto out_peg_1762;
                                }
                            }
                            result_peg_1757.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1758;
                        out_peg_1762:
                        goto loop_peg_1756;
                        success_peg_1758:
                        ;
                        result_peg_1725.addResult(result_peg_1757);
                    } while (true);
                    loop_peg_1756:
                    ;
                            
                }
                goto success_peg_1747;
                goto out_peg_1744;
                success_peg_1747:
                ;
            
            
            
            result_peg_1725 = rule_expr9(stream, result_peg_1725.getPosition());
                if (result_peg_1725.error()){
                    goto out_peg_1744;
                }
                right = result_peg_1725.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionEquals(left, right);
                    result_peg_1725.setValue(value);
                }
                new_left = result_peg_1725.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1725.getPosition();
        goto tail_peg_1726;
        out_peg_1744:
        Result result_peg_1765(myposition);
        {
        
            {
                    
                    result_peg_1765.reset();
                    do{
                        Result result_peg_1777(result_peg_1765.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1777.getPosition()))){
                                    result_peg_1777.nextPosition();
                                } else {
                                    goto out_peg_1780;
                                }
                            }
                            result_peg_1777.setValue((void*) " ");
                                
                        }
                        goto success_peg_1778;
                        out_peg_1780:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1777.getPosition()))){
                                    result_peg_1777.nextPosition();
                                } else {
                                    goto out_peg_1782;
                                }
                            }
                            result_peg_1777.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1778;
                        out_peg_1782:
                        goto loop_peg_1776;
                        success_peg_1778:
                        ;
                        result_peg_1765.addResult(result_peg_1777);
                    } while (true);
                    loop_peg_1776:
                    ;
                            
                }
                goto success_peg_1767;
                goto out_peg_1783;
                success_peg_1767:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_1765.getPosition()))){
                        result_peg_1765.nextPosition();
                    } else {
                        goto out_peg_1783;
                    }
                }
                result_peg_1765.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_1765.reset();
                    do{
                        Result result_peg_1796(result_peg_1765.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1796.getPosition()))){
                                    result_peg_1796.nextPosition();
                                } else {
                                    goto out_peg_1799;
                                }
                            }
                            result_peg_1796.setValue((void*) " ");
                                
                        }
                        goto success_peg_1797;
                        out_peg_1799:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1796.getPosition()))){
                                    result_peg_1796.nextPosition();
                                } else {
                                    goto out_peg_1801;
                                }
                            }
                            result_peg_1796.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1797;
                        out_peg_1801:
                        goto loop_peg_1795;
                        success_peg_1797:
                        ;
                        result_peg_1765.addResult(result_peg_1796);
                    } while (true);
                    loop_peg_1795:
                    ;
                            
                }
                goto success_peg_1786;
                goto out_peg_1783;
                success_peg_1786:
                ;
            
            
            
            result_peg_1765 = rule_expr9(stream, result_peg_1765.getPosition());
                if (result_peg_1765.error()){
                    goto out_peg_1783;
                }
                right = result_peg_1765.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionUnequals(left, right);
                    result_peg_1765.setValue(value);
                }
                new_left = result_peg_1765.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1765.getPosition();
        goto tail_peg_1726;
        out_peg_1783:
        Result result_peg_1804(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1804.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1804.getPosition());
        
        return result_peg_1804;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1850(stream, "expr9_rest");
    int myposition = position;
    
    tail_peg_1809:
    Value maker;
        Value right;
        Value new_left;
    Result result_peg_1808(myposition);
        {
        
            {
                    
                    result_peg_1808.reset();
                    do{
                        Result result_peg_1821(result_peg_1808.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1821.getPosition()))){
                                    result_peg_1821.nextPosition();
                                } else {
                                    goto out_peg_1824;
                                }
                            }
                            result_peg_1821.setValue((void*) " ");
                                
                        }
                        goto success_peg_1822;
                        out_peg_1824:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1821.getPosition()))){
                                    result_peg_1821.nextPosition();
                                } else {
                                    goto out_peg_1826;
                                }
                            }
                            result_peg_1821.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1822;
                        out_peg_1826:
                        goto loop_peg_1820;
                        success_peg_1822:
                        ;
                        result_peg_1808.addResult(result_peg_1821);
                    } while (true);
                    loop_peg_1820:
                    ;
                            
                }
                goto success_peg_1811;
                goto out_peg_1827;
                success_peg_1811:
                ;
            
            
            
            result_peg_1808 = rule_compare(stream, result_peg_1808.getPosition());
                if (result_peg_1808.error()){
                    goto out_peg_1827;
                }
                maker = result_peg_1808.getValues();
            
            
            
            {
                    
                    result_peg_1808.reset();
                    do{
                        Result result_peg_1840(result_peg_1808.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1840.getPosition()))){
                                    result_peg_1840.nextPosition();
                                } else {
                                    goto out_peg_1843;
                                }
                            }
                            result_peg_1840.setValue((void*) " ");
                                
                        }
                        goto success_peg_1841;
                        out_peg_1843:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1840.getPosition()))){
                                    result_peg_1840.nextPosition();
                                } else {
                                    goto out_peg_1845;
                                }
                            }
                            result_peg_1840.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1841;
                        out_peg_1845:
                        goto loop_peg_1839;
                        success_peg_1841:
                        ;
                        result_peg_1808.addResult(result_peg_1840);
                    } while (true);
                    loop_peg_1839:
                    ;
                            
                }
                goto success_peg_1830;
                goto out_peg_1827;
                success_peg_1830:
                ;
            
            
            
            result_peg_1808 = rule_expr10(stream, result_peg_1808.getPosition());
                if (result_peg_1808.error()){
                    goto out_peg_1827;
                }
                right = result_peg_1808.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    {
                            typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                            value = as<compare_func>(maker)(left, right);
                            }
                    result_peg_1808.setValue(value);
                }
                new_left = result_peg_1808.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1808.getPosition();
        goto tail_peg_1809;
        out_peg_1827:
        Result result_peg_1848(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1848.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1848.getPosition());
        
        return result_peg_1848;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1851 = stream.getColumn(position);
    if (column_peg_1851.chunk8 != 0 && column_peg_1851.chunk8->chunk_compare.calculated()){
        return column_peg_1851.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_1864(stream, "compare");
    int myposition = position;
    
    
    
    Result result_peg_1852(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar("<="[i], stream.get(result_peg_1852.getPosition()))){
                        result_peg_1852.nextPosition();
                    } else {
                        goto out_peg_1854;
                    }
                }
                result_peg_1852.setValue((void*) "<=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThanEquals;
                    result_peg_1852.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1852;
        stream.update(result_peg_1852.getPosition());
        
        
        return result_peg_1852;
        out_peg_1854:
        Result result_peg_1855(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar(">="[i], stream.get(result_peg_1855.getPosition()))){
                        result_peg_1855.nextPosition();
                    } else {
                        goto out_peg_1857;
                    }
                }
                result_peg_1855.setValue((void*) ">=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThanEquals;
                    result_peg_1855.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1855;
        stream.update(result_peg_1855.getPosition());
        
        
        return result_peg_1855;
        out_peg_1857:
        Result result_peg_1858(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("<"[i], stream.get(result_peg_1858.getPosition()))){
                        result_peg_1858.nextPosition();
                    } else {
                        goto out_peg_1860;
                    }
                }
                result_peg_1858.setValue((void*) "<");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThan;
                    result_peg_1858.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1858;
        stream.update(result_peg_1858.getPosition());
        
        
        return result_peg_1858;
        out_peg_1860:
        Result result_peg_1861(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_1861.getPosition()))){
                        result_peg_1861.nextPosition();
                    } else {
                        goto out_peg_1863;
                    }
                }
                result_peg_1861.setValue((void*) ">");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThan;
                    result_peg_1861.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1861;
        stream.update(result_peg_1861.getPosition());
        
        
        return result_peg_1861;
        out_peg_1863:
    
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1865 = stream.getColumn(position);
    if (column_peg_1865.chunk8 != 0 && column_peg_1865.chunk8->chunk_all_compare.calculated()){
        return column_peg_1865.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_1872(stream, "all_compare");
    int myposition = position;
    
    
    
    Result result_peg_1866(myposition);
        
        result_peg_1866 = rule_compare(stream, result_peg_1866.getPosition());
        if (result_peg_1866.error()){
            goto out_peg_1867;
        }
        
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = result_peg_1866;
        stream.update(result_peg_1866.getPosition());
        
        
        return result_peg_1866;
        out_peg_1867:
        Result result_peg_1868(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("="[i], stream.get(result_peg_1868.getPosition()))){
                result_peg_1868.nextPosition();
            } else {
                goto out_peg_1869;
            }
        }
        result_peg_1868.setValue((void*) "=");
        
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = result_peg_1868;
        stream.update(result_peg_1868.getPosition());
        
        
        return result_peg_1868;
        out_peg_1869:
        Result result_peg_1870(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("!="[i], stream.get(result_peg_1870.getPosition()))){
                result_peg_1870.nextPosition();
            } else {
                goto out_peg_1871;
            }
        }
        result_peg_1870.setValue((void*) "!=");
        
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = result_peg_1870;
        stream.update(result_peg_1870.getPosition());
        
        
        return result_peg_1870;
        out_peg_1871:
    
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1955(stream, "expr10_rest");
    int myposition = position;
    
    tail_peg_1875:
    Value right;
        Value new_left;
    Result result_peg_1874(myposition);
        {
        
            {
                    
                    result_peg_1874.reset();
                    do{
                        Result result_peg_1887(result_peg_1874.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1887.getPosition()))){
                                    result_peg_1887.nextPosition();
                                } else {
                                    goto out_peg_1890;
                                }
                            }
                            result_peg_1887.setValue((void*) " ");
                                
                        }
                        goto success_peg_1888;
                        out_peg_1890:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1887.getPosition()))){
                                    result_peg_1887.nextPosition();
                                } else {
                                    goto out_peg_1892;
                                }
                            }
                            result_peg_1887.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1888;
                        out_peg_1892:
                        goto loop_peg_1886;
                        success_peg_1888:
                        ;
                        result_peg_1874.addResult(result_peg_1887);
                    } while (true);
                    loop_peg_1886:
                    ;
                            
                }
                goto success_peg_1877;
                goto out_peg_1893;
                success_peg_1877:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1874.getPosition()))){
                        result_peg_1874.nextPosition();
                    } else {
                        goto out_peg_1893;
                    }
                }
                result_peg_1874.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_1874.reset();
                    do{
                        Result result_peg_1906(result_peg_1874.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1906.getPosition()))){
                                    result_peg_1906.nextPosition();
                                } else {
                                    goto out_peg_1909;
                                }
                            }
                            result_peg_1906.setValue((void*) " ");
                                
                        }
                        goto success_peg_1907;
                        out_peg_1909:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1906.getPosition()))){
                                    result_peg_1906.nextPosition();
                                } else {
                                    goto out_peg_1911;
                                }
                            }
                            result_peg_1906.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1907;
                        out_peg_1911:
                        goto loop_peg_1905;
                        success_peg_1907:
                        ;
                        result_peg_1874.addResult(result_peg_1906);
                    } while (true);
                    loop_peg_1905:
                    ;
                            
                }
                goto success_peg_1896;
                goto out_peg_1893;
                success_peg_1896:
                ;
            
            
            
            result_peg_1874 = rule_expr11(stream, result_peg_1874.getPosition());
                if (result_peg_1874.error()){
                    goto out_peg_1893;
                }
                right = result_peg_1874.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAdd(left, right);
                    result_peg_1874.setValue(value);
                }
                new_left = result_peg_1874.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1874.getPosition();
        goto tail_peg_1875;
        out_peg_1893:
        Result result_peg_1914(myposition);
        {
        
            {
                    
                    result_peg_1914.reset();
                    do{
                        Result result_peg_1926(result_peg_1914.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1926.getPosition()))){
                                    result_peg_1926.nextPosition();
                                } else {
                                    goto out_peg_1929;
                                }
                            }
                            result_peg_1926.setValue((void*) " ");
                                
                        }
                        goto success_peg_1927;
                        out_peg_1929:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1926.getPosition()))){
                                    result_peg_1926.nextPosition();
                                } else {
                                    goto out_peg_1931;
                                }
                            }
                            result_peg_1926.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1927;
                        out_peg_1931:
                        goto loop_peg_1925;
                        success_peg_1927:
                        ;
                        result_peg_1914.addResult(result_peg_1926);
                    } while (true);
                    loop_peg_1925:
                    ;
                            
                }
                goto success_peg_1916;
                goto out_peg_1932;
                success_peg_1916:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1914.getPosition()))){
                        result_peg_1914.nextPosition();
                    } else {
                        goto out_peg_1932;
                    }
                }
                result_peg_1914.setValue((void*) "-");
            
            
            
            {
                    
                    result_peg_1914.reset();
                    do{
                        Result result_peg_1945(result_peg_1914.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1945.getPosition()))){
                                    result_peg_1945.nextPosition();
                                } else {
                                    goto out_peg_1948;
                                }
                            }
                            result_peg_1945.setValue((void*) " ");
                                
                        }
                        goto success_peg_1946;
                        out_peg_1948:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1945.getPosition()))){
                                    result_peg_1945.nextPosition();
                                } else {
                                    goto out_peg_1950;
                                }
                            }
                            result_peg_1945.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1946;
                        out_peg_1950:
                        goto loop_peg_1944;
                        success_peg_1946:
                        ;
                        result_peg_1914.addResult(result_peg_1945);
                    } while (true);
                    loop_peg_1944:
                    ;
                            
                }
                goto success_peg_1935;
                goto out_peg_1932;
                success_peg_1935:
                ;
            
            
            
            result_peg_1914 = rule_expr11(stream, result_peg_1914.getPosition());
                if (result_peg_1914.error()){
                    goto out_peg_1932;
                }
                right = result_peg_1914.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionSubtract(left, right);
                    result_peg_1914.setValue(value);
                }
                new_left = result_peg_1914.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1914.getPosition();
        goto tail_peg_1875;
        out_peg_1932:
        Result result_peg_1953(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1953.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1953.getPosition());
        
        return result_peg_1953;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2077(stream, "expr11_rest");
    int myposition = position;
    
    tail_peg_1958:
    Value right;
        Value new_left;
    Result result_peg_1957(myposition);
        {
        
            {
                    
                    result_peg_1957.reset();
                    do{
                        Result result_peg_1970(result_peg_1957.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1970.getPosition()))){
                                    result_peg_1970.nextPosition();
                                } else {
                                    goto out_peg_1973;
                                }
                            }
                            result_peg_1970.setValue((void*) " ");
                                
                        }
                        goto success_peg_1971;
                        out_peg_1973:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1970.getPosition()))){
                                    result_peg_1970.nextPosition();
                                } else {
                                    goto out_peg_1975;
                                }
                            }
                            result_peg_1970.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1971;
                        out_peg_1975:
                        goto loop_peg_1969;
                        success_peg_1971:
                        ;
                        result_peg_1957.addResult(result_peg_1970);
                    } while (true);
                    loop_peg_1969:
                    ;
                            
                }
                goto success_peg_1960;
                goto out_peg_1976;
                success_peg_1960:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("*"[i], stream.get(result_peg_1957.getPosition()))){
                        result_peg_1957.nextPosition();
                    } else {
                        goto out_peg_1976;
                    }
                }
                result_peg_1957.setValue((void*) "*");
            
            
            
            {
                    
                    result_peg_1957.reset();
                    do{
                        Result result_peg_1989(result_peg_1957.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1989.getPosition()))){
                                    result_peg_1989.nextPosition();
                                } else {
                                    goto out_peg_1992;
                                }
                            }
                            result_peg_1989.setValue((void*) " ");
                                
                        }
                        goto success_peg_1990;
                        out_peg_1992:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1989.getPosition()))){
                                    result_peg_1989.nextPosition();
                                } else {
                                    goto out_peg_1994;
                                }
                            }
                            result_peg_1989.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1990;
                        out_peg_1994:
                        goto loop_peg_1988;
                        success_peg_1990:
                        ;
                        result_peg_1957.addResult(result_peg_1989);
                    } while (true);
                    loop_peg_1988:
                    ;
                            
                }
                goto success_peg_1979;
                goto out_peg_1976;
                success_peg_1979:
                ;
            
            
            
            result_peg_1957 = rule_expr12(stream, result_peg_1957.getPosition());
                if (result_peg_1957.error()){
                    goto out_peg_1976;
                }
                right = result_peg_1957.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionMultiply(left, right);
                    result_peg_1957.setValue(value);
                }
                new_left = result_peg_1957.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1957.getPosition();
        goto tail_peg_1958;
        out_peg_1976:
        Result result_peg_1997(myposition);
        {
        
            {
                    
                    result_peg_1997.reset();
                    do{
                        Result result_peg_2009(result_peg_1997.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2009.getPosition()))){
                                    result_peg_2009.nextPosition();
                                } else {
                                    goto out_peg_2012;
                                }
                            }
                            result_peg_2009.setValue((void*) " ");
                                
                        }
                        goto success_peg_2010;
                        out_peg_2012:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2009.getPosition()))){
                                    result_peg_2009.nextPosition();
                                } else {
                                    goto out_peg_2014;
                                }
                            }
                            result_peg_2009.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2010;
                        out_peg_2014:
                        goto loop_peg_2008;
                        success_peg_2010:
                        ;
                        result_peg_1997.addResult(result_peg_2009);
                    } while (true);
                    loop_peg_2008:
                    ;
                            
                }
                goto success_peg_1999;
                goto out_peg_2015;
                success_peg_1999:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_1997.getPosition()))){
                        result_peg_1997.nextPosition();
                    } else {
                        goto out_peg_2015;
                    }
                }
                result_peg_1997.setValue((void*) "/");
            
            
            
            {
                    
                    result_peg_1997.reset();
                    do{
                        Result result_peg_2028(result_peg_1997.getPosition());
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
                        result_peg_1997.addResult(result_peg_2028);
                    } while (true);
                    loop_peg_2027:
                    ;
                            
                }
                goto success_peg_2018;
                goto out_peg_2015;
                success_peg_2018:
                ;
            
            
            
            result_peg_1997 = rule_expr12(stream, result_peg_1997.getPosition());
                if (result_peg_1997.error()){
                    goto out_peg_2015;
                }
                right = result_peg_1997.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionDivide(left, right);
                    result_peg_1997.setValue(value);
                }
                new_left = result_peg_1997.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1997.getPosition();
        goto tail_peg_1958;
        out_peg_2015:
        Result result_peg_2036(myposition);
        {
        
            {
                    
                    result_peg_2036.reset();
                    do{
                        Result result_peg_2048(result_peg_2036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2048.getPosition()))){
                                    result_peg_2048.nextPosition();
                                } else {
                                    goto out_peg_2051;
                                }
                            }
                            result_peg_2048.setValue((void*) " ");
                                
                        }
                        goto success_peg_2049;
                        out_peg_2051:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2048.getPosition()))){
                                    result_peg_2048.nextPosition();
                                } else {
                                    goto out_peg_2053;
                                }
                            }
                            result_peg_2048.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2049;
                        out_peg_2053:
                        goto loop_peg_2047;
                        success_peg_2049:
                        ;
                        result_peg_2036.addResult(result_peg_2048);
                    } while (true);
                    loop_peg_2047:
                    ;
                            
                }
                goto success_peg_2038;
                goto out_peg_2054;
                success_peg_2038:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("%"[i], stream.get(result_peg_2036.getPosition()))){
                        result_peg_2036.nextPosition();
                    } else {
                        goto out_peg_2054;
                    }
                }
                result_peg_2036.setValue((void*) "%");
            
            
            
            {
                    
                    result_peg_2036.reset();
                    do{
                        Result result_peg_2067(result_peg_2036.getPosition());
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
                        result_peg_2036.addResult(result_peg_2067);
                    } while (true);
                    loop_peg_2066:
                    ;
                            
                }
                goto success_peg_2057;
                goto out_peg_2054;
                success_peg_2057:
                ;
            
            
            
            result_peg_2036 = rule_expr12(stream, result_peg_2036.getPosition());
                if (result_peg_2036.error()){
                    goto out_peg_2054;
                }
                right = result_peg_2036.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionModulo(left, right);
                    result_peg_2036.setValue(value);
                }
                new_left = result_peg_2036.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2036.getPosition();
        goto tail_peg_1958;
        out_peg_2054:
        Result result_peg_2075(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2075.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2075.getPosition());
        
        return result_peg_2075;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2121(stream, "expr12_rest");
    int myposition = position;
    
    tail_peg_2080:
    Value right;
        Value new_left;
    Result result_peg_2079(myposition);
        {
        
            {
                    
                    result_peg_2079.reset();
                    do{
                        Result result_peg_2092(result_peg_2079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2092.getPosition()))){
                                    result_peg_2092.nextPosition();
                                } else {
                                    goto out_peg_2095;
                                }
                            }
                            result_peg_2092.setValue((void*) " ");
                                
                        }
                        goto success_peg_2093;
                        out_peg_2095:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2092.getPosition()))){
                                    result_peg_2092.nextPosition();
                                } else {
                                    goto out_peg_2097;
                                }
                            }
                            result_peg_2092.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2093;
                        out_peg_2097:
                        goto loop_peg_2091;
                        success_peg_2093:
                        ;
                        result_peg_2079.addResult(result_peg_2092);
                    } while (true);
                    loop_peg_2091:
                    ;
                            
                }
                goto success_peg_2082;
                goto out_peg_2098;
                success_peg_2082:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("**"[i], stream.get(result_peg_2079.getPosition()))){
                        result_peg_2079.nextPosition();
                    } else {
                        goto out_peg_2098;
                    }
                }
                result_peg_2079.setValue((void*) "**");
            
            
            
            {
                    
                    result_peg_2079.reset();
                    do{
                        Result result_peg_2111(result_peg_2079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2111.getPosition()))){
                                    result_peg_2111.nextPosition();
                                } else {
                                    goto out_peg_2114;
                                }
                            }
                            result_peg_2111.setValue((void*) " ");
                                
                        }
                        goto success_peg_2112;
                        out_peg_2114:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2111.getPosition()))){
                                    result_peg_2111.nextPosition();
                                } else {
                                    goto out_peg_2116;
                                }
                            }
                            result_peg_2111.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2112;
                        out_peg_2116:
                        goto loop_peg_2110;
                        success_peg_2112:
                        ;
                        result_peg_2079.addResult(result_peg_2111);
                    } while (true);
                    loop_peg_2110:
                    ;
                            
                }
                goto success_peg_2101;
                goto out_peg_2098;
                success_peg_2101:
                ;
            
            
            
            result_peg_2079 = rule_expr13(stream, result_peg_2079.getPosition());
                if (result_peg_2079.error()){
                    goto out_peg_2098;
                }
                right = result_peg_2079.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionPower(left, right);
                    result_peg_2079.setValue(value);
                }
                new_left = result_peg_2079.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2079.getPosition();
        goto tail_peg_2080;
        out_peg_2098:
        Result result_peg_2119(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2119.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2119.getPosition());
        
        return result_peg_2119;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_2122 = stream.getColumn(position);
    if (column_peg_2122.chunk9 != 0 && column_peg_2122.chunk9->chunk_unary.calculated()){
        return column_peg_2122.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_2140(stream, "unary");
    int myposition = position;
    
    
    
    Result result_peg_2123(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("!"[i], stream.get(result_peg_2123.getPosition()))){
                        result_peg_2123.nextPosition();
                    } else {
                        goto out_peg_2125;
                    }
                }
                result_peg_2123.setValue((void*) "!");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Not;
                    result_peg_2123.setValue(value);
                }
            
            
        }
        
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = result_peg_2123;
        stream.update(result_peg_2123.getPosition());
        
        
        return result_peg_2123;
        out_peg_2125:
        Result result_peg_2126(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2126.getPosition()))){
                        result_peg_2126.nextPosition();
                    } else {
                        goto out_peg_2128;
                    }
                }
                result_peg_2126.setValue((void*) "-");
            
            
            
            Result result_peg_2131(result_peg_2126);
                {
                    
                    char letter_peg_2136 = stream.get(result_peg_2131.getPosition());
                    if (letter_peg_2136 != '\0' && strchr("0123456789", letter_peg_2136) != NULL){
                        result_peg_2131.nextPosition();
                        result_peg_2131.setValue((void*) (long) letter_peg_2136);
                    } else {
                        goto out_peg_2135;
                    }
                    
                }
                goto success_peg_2132;
                out_peg_2135:
                goto not_peg_2130;
                success_peg_2132:
                ;
                goto out_peg_2128;
                not_peg_2130:
                result_peg_2126.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Minus;
                    result_peg_2126.setValue(value);
                }
            
            
        }
        
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = result_peg_2126;
        stream.update(result_peg_2126.getPosition());
        
        
        return result_peg_2126;
        out_peg_2128:
        Result result_peg_2137(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2137.getPosition()))){
                        result_peg_2137.nextPosition();
                    } else {
                        goto out_peg_2139;
                    }
                }
                result_peg_2137.setValue((void*) "~");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Negation;
                    result_peg_2137.setValue(value);
                }
            
            
        }
        
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = result_peg_2137;
        stream.update(result_peg_2137.getPosition());
        
        
        return result_peg_2137;
        out_peg_2139:
    
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_2141 = stream.getColumn(position);
    if (column_peg_2141.chunk9 != 0 && column_peg_2141.chunk9->chunk_expr13_real.calculated()){
        return column_peg_2141.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_2187(stream, "expr13_real");
    int myposition = position;
    
    
    Value e;
    Result result_peg_2142(myposition);
        
        result_peg_2142 = rule_helper(stream, result_peg_2142.getPosition());
        if (result_peg_2142.error()){
            goto out_peg_2143;
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2142;
        stream.update(result_peg_2142.getPosition());
        
        
        return result_peg_2142;
        out_peg_2143:
        Result result_peg_2144(myposition);
        
        result_peg_2144 = rule_function(stream, result_peg_2144.getPosition());
        if (result_peg_2144.error()){
            goto out_peg_2145;
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2144;
        stream.update(result_peg_2144.getPosition());
        
        
        return result_peg_2144;
        out_peg_2145:
        Result result_peg_2146(myposition);
        
        result_peg_2146 = rule_value(stream, result_peg_2146.getPosition());
        if (result_peg_2146.error()){
            goto out_peg_2147;
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2146;
        stream.update(result_peg_2146.getPosition());
        
        
        return result_peg_2146;
        out_peg_2147:
        Result result_peg_2148(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2148.getPosition()))){
                        result_peg_2148.nextPosition();
                    } else {
                        goto out_peg_2150;
                    }
                }
                result_peg_2148.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2148.reset();
                    do{
                        Result result_peg_2162(result_peg_2148.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2162.getPosition()))){
                                    result_peg_2162.nextPosition();
                                } else {
                                    goto out_peg_2165;
                                }
                            }
                            result_peg_2162.setValue((void*) " ");
                                
                        }
                        goto success_peg_2163;
                        out_peg_2165:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2162.getPosition()))){
                                    result_peg_2162.nextPosition();
                                } else {
                                    goto out_peg_2167;
                                }
                            }
                            result_peg_2162.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2163;
                        out_peg_2167:
                        goto loop_peg_2161;
                        success_peg_2163:
                        ;
                        result_peg_2148.addResult(result_peg_2162);
                    } while (true);
                    loop_peg_2161:
                    ;
                            
                }
                goto success_peg_2152;
                goto out_peg_2150;
                success_peg_2152:
                ;
            
            
            
            result_peg_2148 = rule_expr(stream, result_peg_2148.getPosition());
                if (result_peg_2148.error()){
                    goto out_peg_2150;
                }
                e = result_peg_2148.getValues();
            
            
            
            {
                    
                    result_peg_2148.reset();
                    do{
                        Result result_peg_2180(result_peg_2148.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2180.getPosition()))){
                                    result_peg_2180.nextPosition();
                                } else {
                                    goto out_peg_2183;
                                }
                            }
                            result_peg_2180.setValue((void*) " ");
                                
                        }
                        goto success_peg_2181;
                        out_peg_2183:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2180.getPosition()))){
                                    result_peg_2180.nextPosition();
                                } else {
                                    goto out_peg_2185;
                                }
                            }
                            result_peg_2180.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2181;
                        out_peg_2185:
                        goto loop_peg_2179;
                        success_peg_2181:
                        ;
                        result_peg_2148.addResult(result_peg_2180);
                    } while (true);
                    loop_peg_2179:
                    ;
                            
                }
                goto success_peg_2170;
                goto out_peg_2150;
                success_peg_2170:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2148.getPosition()))){
                        result_peg_2148.nextPosition();
                    } else {
                        goto out_peg_2150;
                    }
                }
                result_peg_2148.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = e;
                    result_peg_2148.setValue(value);
                }
            
            
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2148;
        stream.update(result_peg_2148.getPosition());
        
        
        return result_peg_2148;
        out_peg_2150:
    
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_2188 = stream.getColumn(position);
    if (column_peg_2188.chunk9 != 0 && column_peg_2188.chunk9->chunk_function.calculated()){
        return column_peg_2188.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2446(stream, "function");
    int myposition = position;
    
    
    Value name;
        Value args;
        Value arg1;
        Value id;
    Result result_peg_2189(myposition);
        
        {
        
            result_peg_2189 = rule_function_name(stream, result_peg_2189.getPosition());
                if (result_peg_2189.error()){
                    goto out_peg_2191;
                }
                name = result_peg_2189.getValues();
            
            
            
            {
                    
                    result_peg_2189.reset();
                    do{
                        Result result_peg_2203(result_peg_2189.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2203.getPosition()))){
                                    result_peg_2203.nextPosition();
                                } else {
                                    goto out_peg_2206;
                                }
                            }
                            result_peg_2203.setValue((void*) " ");
                                
                        }
                        goto success_peg_2204;
                        out_peg_2206:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2203.getPosition()))){
                                    result_peg_2203.nextPosition();
                                } else {
                                    goto out_peg_2208;
                                }
                            }
                            result_peg_2203.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2204;
                        out_peg_2208:
                        goto loop_peg_2202;
                        success_peg_2204:
                        ;
                        result_peg_2189.addResult(result_peg_2203);
                    } while (true);
                    loop_peg_2202:
                    ;
                            
                }
                goto success_peg_2193;
                goto out_peg_2191;
                success_peg_2193:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2189.getPosition()))){
                        result_peg_2189.nextPosition();
                    } else {
                        goto out_peg_2191;
                    }
                }
                result_peg_2189.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2189.reset();
                    do{
                        Result result_peg_2221(result_peg_2189.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2221.getPosition()))){
                                    result_peg_2221.nextPosition();
                                } else {
                                    goto out_peg_2224;
                                }
                            }
                            result_peg_2221.setValue((void*) " ");
                                
                        }
                        goto success_peg_2222;
                        out_peg_2224:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2221.getPosition()))){
                                    result_peg_2221.nextPosition();
                                } else {
                                    goto out_peg_2226;
                                }
                            }
                            result_peg_2221.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2222;
                        out_peg_2226:
                        goto loop_peg_2220;
                        success_peg_2222:
                        ;
                        result_peg_2189.addResult(result_peg_2221);
                    } while (true);
                    loop_peg_2220:
                    ;
                            
                }
                goto success_peg_2211;
                goto out_peg_2191;
                success_peg_2211:
                ;
            
            
            
            result_peg_2189 = rule_args(stream, result_peg_2189.getPosition());
                if (result_peg_2189.error()){
                    goto out_peg_2191;
                }
                args = result_peg_2189.getValues();
            
            
            
            {
                    
                    result_peg_2189.reset();
                    do{
                        Result result_peg_2239(result_peg_2189.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2239.getPosition()))){
                                    result_peg_2239.nextPosition();
                                } else {
                                    goto out_peg_2242;
                                }
                            }
                            result_peg_2239.setValue((void*) " ");
                                
                        }
                        goto success_peg_2240;
                        out_peg_2242:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2239.getPosition()))){
                                    result_peg_2239.nextPosition();
                                } else {
                                    goto out_peg_2244;
                                }
                            }
                            result_peg_2239.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2240;
                        out_peg_2244:
                        goto loop_peg_2238;
                        success_peg_2240:
                        ;
                        result_peg_2189.addResult(result_peg_2239);
                    } while (true);
                    loop_peg_2238:
                    ;
                            
                }
                goto success_peg_2229;
                goto out_peg_2191;
                success_peg_2229:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2189.getPosition()))){
                        result_peg_2189.nextPosition();
                    } else {
                        goto out_peg_2191;
                    }
                }
                result_peg_2189.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction(name, args);
                    result_peg_2189.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2189;
        stream.update(result_peg_2189.getPosition());
        
        
        return result_peg_2189;
        out_peg_2191:
        Result result_peg_2246(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2246.getPosition()))){
                        result_peg_2246.nextPosition();
                    } else {
                        goto out_peg_2248;
                    }
                }
                result_peg_2246.setValue((void*) "teammode");
                name = result_peg_2246.getValues();
            
            
            
            {
                    
                    result_peg_2246.reset();
                    do{
                        Result result_peg_2260(result_peg_2246.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2260.getPosition()))){
                                    result_peg_2260.nextPosition();
                                } else {
                                    goto out_peg_2263;
                                }
                            }
                            result_peg_2260.setValue((void*) " ");
                                
                        }
                        goto success_peg_2261;
                        out_peg_2263:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2260.getPosition()))){
                                    result_peg_2260.nextPosition();
                                } else {
                                    goto out_peg_2265;
                                }
                            }
                            result_peg_2260.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2261;
                        out_peg_2265:
                        goto loop_peg_2259;
                        success_peg_2261:
                        ;
                        result_peg_2246.addResult(result_peg_2260);
                    } while (true);
                    loop_peg_2259:
                    ;
                            
                }
                goto success_peg_2250;
                goto out_peg_2248;
                success_peg_2250:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2246.getPosition()))){
                        result_peg_2246.nextPosition();
                    } else {
                        goto out_peg_2248;
                    }
                }
                result_peg_2246.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2246.reset();
                    do{
                        Result result_peg_2278(result_peg_2246.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2278.getPosition()))){
                                    result_peg_2278.nextPosition();
                                } else {
                                    goto out_peg_2281;
                                }
                            }
                            result_peg_2278.setValue((void*) " ");
                                
                        }
                        goto success_peg_2279;
                        out_peg_2281:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2278.getPosition()))){
                                    result_peg_2278.nextPosition();
                                } else {
                                    goto out_peg_2283;
                                }
                            }
                            result_peg_2278.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2279;
                        out_peg_2283:
                        goto loop_peg_2277;
                        success_peg_2279:
                        ;
                        result_peg_2246.addResult(result_peg_2278);
                    } while (true);
                    loop_peg_2277:
                    ;
                            
                }
                goto success_peg_2268;
                goto out_peg_2248;
                success_peg_2268:
                ;
            
            
            
            result_peg_2246 = rule_identifier(stream, result_peg_2246.getPosition());
                if (result_peg_2246.error()){
                    goto out_peg_2248;
                }
                arg1 = result_peg_2246.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(name, arg1);
                    result_peg_2246.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2246;
        stream.update(result_peg_2246.getPosition());
        
        
        return result_peg_2246;
        out_peg_2248:
        Result result_peg_2285(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2285.getPosition()))){
                        result_peg_2285.nextPosition();
                    } else {
                        goto out_peg_2287;
                    }
                }
                result_peg_2285.setValue((void*) "teammode");
                name = result_peg_2285.getValues();
            
            
            
            {
                    
                    result_peg_2285.reset();
                    do{
                        Result result_peg_2299(result_peg_2285.getPosition());
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
                        result_peg_2285.addResult(result_peg_2299);
                    } while (true);
                    loop_peg_2298:
                    ;
                            
                }
                goto success_peg_2289;
                goto out_peg_2287;
                success_peg_2289:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_2285.getPosition()))){
                        result_peg_2285.nextPosition();
                    } else {
                        goto out_peg_2287;
                    }
                }
                result_peg_2285.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_2285.reset();
                    do{
                        Result result_peg_2317(result_peg_2285.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2317.getPosition()))){
                                    result_peg_2317.nextPosition();
                                } else {
                                    goto out_peg_2320;
                                }
                            }
                            result_peg_2317.setValue((void*) " ");
                                
                        }
                        goto success_peg_2318;
                        out_peg_2320:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2317.getPosition()))){
                                    result_peg_2317.nextPosition();
                                } else {
                                    goto out_peg_2322;
                                }
                            }
                            result_peg_2317.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2318;
                        out_peg_2322:
                        goto loop_peg_2316;
                        success_peg_2318:
                        ;
                        result_peg_2285.addResult(result_peg_2317);
                    } while (true);
                    loop_peg_2316:
                    ;
                            
                }
                goto success_peg_2307;
                goto out_peg_2287;
                success_peg_2307:
                ;
            
            
            
            result_peg_2285 = rule_identifier(stream, result_peg_2285.getPosition());
                if (result_peg_2285.error()){
                    goto out_peg_2287;
                }
                arg1 = result_peg_2285.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1("teammode!=", arg1);
                    result_peg_2285.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2285;
        stream.update(result_peg_2285.getPosition());
        
        
        return result_peg_2285;
        out_peg_2287:
        Result result_peg_2324(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2324.getPosition()))){
                        result_peg_2324.nextPosition();
                    } else {
                        goto out_peg_2326;
                    }
                }
                result_peg_2324.setValue((void*) "animelem");
                name = result_peg_2324.getValues();
            
            
            
            {
                    
                    result_peg_2324.reset();
                    do{
                        Result result_peg_2338(result_peg_2324.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2338.getPosition()))){
                                    result_peg_2338.nextPosition();
                                } else {
                                    goto out_peg_2341;
                                }
                            }
                            result_peg_2338.setValue((void*) " ");
                                
                        }
                        goto success_peg_2339;
                        out_peg_2341:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2338.getPosition()))){
                                    result_peg_2338.nextPosition();
                                } else {
                                    goto out_peg_2343;
                                }
                            }
                            result_peg_2338.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2339;
                        out_peg_2343:
                        goto loop_peg_2337;
                        success_peg_2339:
                        ;
                        result_peg_2324.addResult(result_peg_2338);
                    } while (true);
                    loop_peg_2337:
                    ;
                            
                }
                goto success_peg_2328;
                goto out_peg_2326;
                success_peg_2328:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2324.getPosition()))){
                        result_peg_2324.nextPosition();
                    } else {
                        goto out_peg_2326;
                    }
                }
                result_peg_2324.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2324.reset();
                    do{
                        Result result_peg_2356(result_peg_2324.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2356.getPosition()))){
                                    result_peg_2356.nextPosition();
                                } else {
                                    goto out_peg_2359;
                                }
                            }
                            result_peg_2356.setValue((void*) " ");
                                
                        }
                        goto success_peg_2357;
                        out_peg_2359:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2356.getPosition()))){
                                    result_peg_2356.nextPosition();
                                } else {
                                    goto out_peg_2361;
                                }
                            }
                            result_peg_2356.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2357;
                        out_peg_2361:
                        goto loop_peg_2355;
                        success_peg_2357:
                        ;
                        result_peg_2324.addResult(result_peg_2356);
                    } while (true);
                    loop_peg_2355:
                    ;
                            
                }
                goto success_peg_2346;
                goto out_peg_2326;
                success_peg_2346:
                ;
            
            
            
            result_peg_2324 = rule_integer(stream, result_peg_2324.getPosition());
                if (result_peg_2324.error()){
                    goto out_peg_2326;
                }
                arg1 = result_peg_2324.getValues();
            
            
            
            result_peg_2324 = rule_function_rest(stream, result_peg_2324.getPosition());
                if (result_peg_2324.error()){
                    goto out_peg_2326;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    value = makeFunction1(name, arg1);
                    result_peg_2324.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2324;
        stream.update(result_peg_2324.getPosition());
        
        
        return result_peg_2324;
        out_peg_2326:
        Result result_peg_2364(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2364.getPosition()))){
                        result_peg_2364.nextPosition();
                    } else {
                        goto out_peg_2366;
                    }
                }
                result_peg_2364.setValue((void*) "animelem");
                name = result_peg_2364.getValues();
            
            
            
            {
                    
                    result_peg_2364.reset();
                    do{
                        Result result_peg_2378(result_peg_2364.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2378.getPosition()))){
                                    result_peg_2378.nextPosition();
                                } else {
                                    goto out_peg_2381;
                                }
                            }
                            result_peg_2378.setValue((void*) " ");
                                
                        }
                        goto success_peg_2379;
                        out_peg_2381:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2378.getPosition()))){
                                    result_peg_2378.nextPosition();
                                } else {
                                    goto out_peg_2383;
                                }
                            }
                            result_peg_2378.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2379;
                        out_peg_2383:
                        goto loop_peg_2377;
                        success_peg_2379:
                        ;
                        result_peg_2364.addResult(result_peg_2378);
                    } while (true);
                    loop_peg_2377:
                    ;
                            
                }
                goto success_peg_2368;
                goto out_peg_2366;
                success_peg_2368:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("<="[i], stream.get(result_peg_2364.getPosition()))){
                        result_peg_2364.nextPosition();
                    } else {
                        goto out_peg_2366;
                    }
                }
                result_peg_2364.setValue((void*) "<=");
            
            
            
            {
                    
                    result_peg_2364.reset();
                    do{
                        Result result_peg_2396(result_peg_2364.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2396.getPosition()))){
                                    result_peg_2396.nextPosition();
                                } else {
                                    goto out_peg_2399;
                                }
                            }
                            result_peg_2396.setValue((void*) " ");
                                
                        }
                        goto success_peg_2397;
                        out_peg_2399:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2396.getPosition()))){
                                    result_peg_2396.nextPosition();
                                } else {
                                    goto out_peg_2401;
                                }
                            }
                            result_peg_2396.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2397;
                        out_peg_2401:
                        goto loop_peg_2395;
                        success_peg_2397:
                        ;
                        result_peg_2364.addResult(result_peg_2396);
                    } while (true);
                    loop_peg_2395:
                    ;
                            
                }
                goto success_peg_2386;
                goto out_peg_2366;
                success_peg_2386:
                ;
            
            
            
            result_peg_2364 = rule_integer(stream, result_peg_2364.getPosition());
                if (result_peg_2364.error()){
                    goto out_peg_2366;
                }
                arg1 = result_peg_2364.getValues();
            
            
            
            result_peg_2364 = rule_function_rest(stream, result_peg_2364.getPosition());
                if (result_peg_2364.error()){
                    goto out_peg_2366;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    value = makeFunction1("animelem<=", arg1);
                    result_peg_2364.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2364;
        stream.update(result_peg_2364.getPosition());
        
        
        return result_peg_2364;
        out_peg_2366:
        Result result_peg_2404(myposition);
        
        {
        
            for (int i = 0; i < 11; i++){
                    if (compareCharCase("projcontact"[i], stream.get(result_peg_2404.getPosition()))){
                        result_peg_2404.nextPosition();
                    } else {
                        goto out_peg_2406;
                    }
                }
                result_peg_2404.setValue((void*) "projcontact");
                name = result_peg_2404.getValues();
            
            
            
            int save_peg_2408 = result_peg_2404.getPosition();
                
                result_peg_2404 = rule_integer(stream, result_peg_2404.getPosition());
                if (result_peg_2404.error()){
                    
                    result_peg_2404 = Result(save_peg_2408);
                    result_peg_2404.setValue((void*) 0);
                    
                }
                id = result_peg_2404.getValues();
            
            
            
            {
                    
                    result_peg_2404.reset();
                    do{
                        Result result_peg_2420(result_peg_2404.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2420.getPosition()))){
                                    result_peg_2420.nextPosition();
                                } else {
                                    goto out_peg_2423;
                                }
                            }
                            result_peg_2420.setValue((void*) " ");
                                
                        }
                        goto success_peg_2421;
                        out_peg_2423:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2420.getPosition()))){
                                    result_peg_2420.nextPosition();
                                } else {
                                    goto out_peg_2425;
                                }
                            }
                            result_peg_2420.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2421;
                        out_peg_2425:
                        goto loop_peg_2419;
                        success_peg_2421:
                        ;
                        result_peg_2404.addResult(result_peg_2420);
                    } while (true);
                    loop_peg_2419:
                    ;
                            
                }
                goto success_peg_2410;
                goto out_peg_2406;
                success_peg_2410:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2404.getPosition()))){
                        result_peg_2404.nextPosition();
                    } else {
                        goto out_peg_2406;
                    }
                }
                result_peg_2404.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2404.reset();
                    do{
                        Result result_peg_2438(result_peg_2404.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2438.getPosition()))){
                                    result_peg_2438.nextPosition();
                                } else {
                                    goto out_peg_2441;
                                }
                            }
                            result_peg_2438.setValue((void*) " ");
                                
                        }
                        goto success_peg_2439;
                        out_peg_2441:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2438.getPosition()))){
                                    result_peg_2438.nextPosition();
                                } else {
                                    goto out_peg_2443;
                                }
                            }
                            result_peg_2438.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2439;
                        out_peg_2443:
                        goto loop_peg_2437;
                        success_peg_2439:
                        ;
                        result_peg_2404.addResult(result_peg_2438);
                    } while (true);
                    loop_peg_2437:
                    ;
                            
                }
                goto success_peg_2428;
                goto out_peg_2406;
                success_peg_2428:
                ;
            
            
            
            result_peg_2404 = rule_integer(stream, result_peg_2404.getPosition());
                if (result_peg_2404.error()){
                    goto out_peg_2406;
                }
                arg1 = result_peg_2404.getValues();
            
            
            
            result_peg_2404 = rule_function_rest(stream, result_peg_2404.getPosition());
                if (result_peg_2404.error()){
                    goto out_peg_2406;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    std::string name = combine("projcontact", id);
                                    value = makeFunction(name, arg1);
                    result_peg_2404.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2404;
        stream.update(result_peg_2404.getPosition());
        
        
        return result_peg_2404;
        out_peg_2406:
    
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2447 = stream.getColumn(position);
    if (column_peg_2447.chunk9 != 0 && column_peg_2447.chunk9->chunk_args.calculated()){
        return column_peg_2447.chunk9->chunk_args;
    }
    
    RuleTrace trace_peg_2489(stream, "args");
    int myposition = position;
    
    
    Value expr1;
        Value expr_rest;
    Result result_peg_2448(myposition);
        
        {
        
            result_peg_2448 = rule_expr_c(stream, result_peg_2448.getPosition());
                if (result_peg_2448.error()){
                    goto out_peg_2450;
                }
                expr1 = result_peg_2448.getValues();
            
            
            
            result_peg_2448.reset();
                do{
                    Result result_peg_2453(result_peg_2448.getPosition());
                    {
                    
                        {
                                
                                result_peg_2453.reset();
                                do{
                                    Result result_peg_2465(result_peg_2453.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2465.getPosition()))){
                                                result_peg_2465.nextPosition();
                                            } else {
                                                goto out_peg_2468;
                                            }
                                        }
                                        result_peg_2465.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2466;
                                    out_peg_2468:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2465.getPosition()))){
                                                result_peg_2465.nextPosition();
                                            } else {
                                                goto out_peg_2470;
                                            }
                                        }
                                        result_peg_2465.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2466;
                                    out_peg_2470:
                                    goto loop_peg_2464;
                                    success_peg_2466:
                                    ;
                                    result_peg_2453.addResult(result_peg_2465);
                                } while (true);
                                loop_peg_2464:
                                ;
                                        
                            }
                            goto success_peg_2455;
                            goto loop_peg_2452;
                            success_peg_2455:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2453.getPosition()))){
                                    result_peg_2453.nextPosition();
                                } else {
                                    goto loop_peg_2452;
                                }
                            }
                            result_peg_2453.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2453.reset();
                                do{
                                    Result result_peg_2483(result_peg_2453.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2483.getPosition()))){
                                                result_peg_2483.nextPosition();
                                            } else {
                                                goto out_peg_2486;
                                            }
                                        }
                                        result_peg_2483.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2484;
                                    out_peg_2486:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2483.getPosition()))){
                                                result_peg_2483.nextPosition();
                                            } else {
                                                goto out_peg_2488;
                                            }
                                        }
                                        result_peg_2483.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2484;
                                    out_peg_2488:
                                    goto loop_peg_2482;
                                    success_peg_2484:
                                    ;
                                    result_peg_2453.addResult(result_peg_2483);
                                } while (true);
                                loop_peg_2482:
                                ;
                                        
                            }
                            goto success_peg_2473;
                            goto loop_peg_2452;
                            success_peg_2473:
                            ;
                        
                        
                        
                        result_peg_2453 = rule_expr_c(stream, result_peg_2453.getPosition());
                            if (result_peg_2453.error()){
                                goto loop_peg_2452;
                            }
                        
                        
                    }
                    result_peg_2448.addResult(result_peg_2453);
                } while (true);
                loop_peg_2452:
                ;
                expr_rest = result_peg_2448.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(expr1, expr_rest);
                    result_peg_2448.setValue(value);
                }
            
            
        }
        
        if (column_peg_2447.chunk9 == 0){
            column_peg_2447.chunk9 = new Chunk9();
        }
        column_peg_2447.chunk9->chunk_args = result_peg_2448;
        stream.update(result_peg_2448.getPosition());
        
        
        return result_peg_2448;
        out_peg_2450:
    
        if (column_peg_2447.chunk9 == 0){
            column_peg_2447.chunk9 = new Chunk9();
        }
        column_peg_2447.chunk9->chunk_args = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2490 = stream.getColumn(position);
    if (column_peg_2490.chunk10 != 0 && column_peg_2490.chunk10->chunk_function_name.calculated()){
        return column_peg_2490.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_2555(stream, "function_name");
    int myposition = position;
    
    
    
    Result result_peg_2491(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("abs"[i], stream.get(result_peg_2491.getPosition()))){
                result_peg_2491.nextPosition();
            } else {
                goto out_peg_2492;
            }
        }
        result_peg_2491.setValue((void*) "abs");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2491;
        stream.update(result_peg_2491.getPosition());
        
        
        return result_peg_2491;
        out_peg_2492:
        Result result_peg_2493(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("const"[i], stream.get(result_peg_2493.getPosition()))){
                result_peg_2493.nextPosition();
            } else {
                goto out_peg_2494;
            }
        }
        result_peg_2493.setValue((void*) "const");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2493;
        stream.update(result_peg_2493.getPosition());
        
        
        return result_peg_2493;
        out_peg_2494:
        Result result_peg_2495(myposition);
        
        for (int i = 0; i < 13; i++){
            if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2495.getPosition()))){
                result_peg_2495.nextPosition();
            } else {
                goto out_peg_2496;
            }
        }
        result_peg_2495.setValue((void*) "selfanimexist");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2495;
        stream.update(result_peg_2495.getPosition());
        
        
        return result_peg_2495;
        out_peg_2496:
        Result result_peg_2497(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("ifelse"[i], stream.get(result_peg_2497.getPosition()))){
                result_peg_2497.nextPosition();
            } else {
                goto out_peg_2498;
            }
        }
        result_peg_2497.setValue((void*) "ifelse");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2497;
        stream.update(result_peg_2497.getPosition());
        
        
        return result_peg_2497;
        out_peg_2498:
        Result result_peg_2499(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("gethitvar"[i], stream.get(result_peg_2499.getPosition()))){
                result_peg_2499.nextPosition();
            } else {
                goto out_peg_2500;
            }
        }
        result_peg_2499.setValue((void*) "gethitvar");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2499;
        stream.update(result_peg_2499.getPosition());
        
        
        return result_peg_2499;
        out_peg_2500:
        Result result_peg_2501(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("floor"[i], stream.get(result_peg_2501.getPosition()))){
                result_peg_2501.nextPosition();
            } else {
                goto out_peg_2502;
            }
        }
        result_peg_2501.setValue((void*) "floor");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2501;
        stream.update(result_peg_2501.getPosition());
        
        
        return result_peg_2501;
        out_peg_2502:
        Result result_peg_2503(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("ceil"[i], stream.get(result_peg_2503.getPosition()))){
                result_peg_2503.nextPosition();
            } else {
                goto out_peg_2504;
            }
        }
        result_peg_2503.setValue((void*) "ceil");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2503;
        stream.update(result_peg_2503.getPosition());
        
        
        return result_peg_2503;
        out_peg_2504:
        Result result_peg_2505(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("exp"[i], stream.get(result_peg_2505.getPosition()))){
                result_peg_2505.nextPosition();
            } else {
                goto out_peg_2506;
            }
        }
        result_peg_2505.setValue((void*) "exp");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2505;
        stream.update(result_peg_2505.getPosition());
        
        
        return result_peg_2505;
        out_peg_2506:
        Result result_peg_2507(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("acos"[i], stream.get(result_peg_2507.getPosition()))){
                result_peg_2507.nextPosition();
            } else {
                goto out_peg_2508;
            }
        }
        result_peg_2507.setValue((void*) "acos");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2507;
        stream.update(result_peg_2507.getPosition());
        
        
        return result_peg_2507;
        out_peg_2508:
        Result result_peg_2509(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("asin"[i], stream.get(result_peg_2509.getPosition()))){
                result_peg_2509.nextPosition();
            } else {
                goto out_peg_2510;
            }
        }
        result_peg_2509.setValue((void*) "asin");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2509;
        stream.update(result_peg_2509.getPosition());
        
        
        return result_peg_2509;
        out_peg_2510:
        Result result_peg_2511(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("atan"[i], stream.get(result_peg_2511.getPosition()))){
                result_peg_2511.nextPosition();
            } else {
                goto out_peg_2512;
            }
        }
        result_peg_2511.setValue((void*) "atan");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2511;
        stream.update(result_peg_2511.getPosition());
        
        
        return result_peg_2511;
        out_peg_2512:
        Result result_peg_2513(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("tan"[i], stream.get(result_peg_2513.getPosition()))){
                result_peg_2513.nextPosition();
            } else {
                goto out_peg_2514;
            }
        }
        result_peg_2513.setValue((void*) "tan");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2513;
        stream.update(result_peg_2513.getPosition());
        
        
        return result_peg_2513;
        out_peg_2514:
        Result result_peg_2515(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("cos"[i], stream.get(result_peg_2515.getPosition()))){
                result_peg_2515.nextPosition();
            } else {
                goto out_peg_2516;
            }
        }
        result_peg_2515.setValue((void*) "cos");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2515;
        stream.update(result_peg_2515.getPosition());
        
        
        return result_peg_2515;
        out_peg_2516:
        Result result_peg_2517(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("sin"[i], stream.get(result_peg_2517.getPosition()))){
                result_peg_2517.nextPosition();
            } else {
                goto out_peg_2518;
            }
        }
        result_peg_2517.setValue((void*) "sin");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2517;
        stream.update(result_peg_2517.getPosition());
        
        
        return result_peg_2517;
        out_peg_2518:
        Result result_peg_2519(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("log"[i], stream.get(result_peg_2519.getPosition()))){
                result_peg_2519.nextPosition();
            } else {
                goto out_peg_2520;
            }
        }
        result_peg_2519.setValue((void*) "log");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2519;
        stream.update(result_peg_2519.getPosition());
        
        
        return result_peg_2519;
        out_peg_2520:
        Result result_peg_2521(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareCharCase("ln"[i], stream.get(result_peg_2521.getPosition()))){
                result_peg_2521.nextPosition();
            } else {
                goto out_peg_2522;
            }
        }
        result_peg_2521.setValue((void*) "ln");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2521;
        stream.update(result_peg_2521.getPosition());
        
        
        return result_peg_2521;
        out_peg_2522:
        Result result_peg_2523(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("sysfvar"[i], stream.get(result_peg_2523.getPosition()))){
                result_peg_2523.nextPosition();
            } else {
                goto out_peg_2524;
            }
        }
        result_peg_2523.setValue((void*) "sysfvar");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2523;
        stream.update(result_peg_2523.getPosition());
        
        
        return result_peg_2523;
        out_peg_2524:
        Result result_peg_2525(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("sysvar"[i], stream.get(result_peg_2525.getPosition()))){
                result_peg_2525.nextPosition();
            } else {
                goto out_peg_2526;
            }
        }
        result_peg_2525.setValue((void*) "sysvar");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2525;
        stream.update(result_peg_2525.getPosition());
        
        
        return result_peg_2525;
        out_peg_2526:
        Result result_peg_2527(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("var"[i], stream.get(result_peg_2527.getPosition()))){
                result_peg_2527.nextPosition();
            } else {
                goto out_peg_2528;
            }
        }
        result_peg_2527.setValue((void*) "var");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2527;
        stream.update(result_peg_2527.getPosition());
        
        
        return result_peg_2527;
        out_peg_2528:
        Result result_peg_2529(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numexplod"[i], stream.get(result_peg_2529.getPosition()))){
                result_peg_2529.nextPosition();
            } else {
                goto out_peg_2530;
            }
        }
        result_peg_2529.setValue((void*) "numexplod");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2529;
        stream.update(result_peg_2529.getPosition());
        
        
        return result_peg_2529;
        out_peg_2530:
        Result result_peg_2531(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2531.getPosition()))){
                result_peg_2531.nextPosition();
            } else {
                goto out_peg_2532;
            }
        }
        result_peg_2531.setValue((void*) "numhelper");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2531;
        stream.update(result_peg_2531.getPosition());
        
        
        return result_peg_2531;
        out_peg_2532:
        Result result_peg_2533(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numprojid"[i], stream.get(result_peg_2533.getPosition()))){
                result_peg_2533.nextPosition();
            } else {
                goto out_peg_2534;
            }
        }
        result_peg_2533.setValue((void*) "numprojid");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2533;
        stream.update(result_peg_2533.getPosition());
        
        
        return result_peg_2533;
        out_peg_2534:
        Result result_peg_2535(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("fvar"[i], stream.get(result_peg_2535.getPosition()))){
                result_peg_2535.nextPosition();
            } else {
                goto out_peg_2536;
            }
        }
        result_peg_2535.setValue((void*) "fvar");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2535;
        stream.update(result_peg_2535.getPosition());
        
        
        return result_peg_2535;
        out_peg_2536:
        Result result_peg_2537(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("ishelper"[i], stream.get(result_peg_2537.getPosition()))){
                result_peg_2537.nextPosition();
            } else {
                goto out_peg_2538;
            }
        }
        result_peg_2537.setValue((void*) "ishelper");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2537;
        stream.update(result_peg_2537.getPosition());
        
        
        return result_peg_2537;
        out_peg_2538:
        Result result_peg_2539(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numtarget"[i], stream.get(result_peg_2539.getPosition()))){
                result_peg_2539.nextPosition();
            } else {
                goto out_peg_2540;
            }
        }
        result_peg_2539.setValue((void*) "numtarget");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2539;
        stream.update(result_peg_2539.getPosition());
        
        
        return result_peg_2539;
        out_peg_2540:
        Result result_peg_2541(myposition);
        
        for (int i = 0; i < 12; i++){
            if (compareCharCase("animelemtime"[i], stream.get(result_peg_2541.getPosition()))){
                result_peg_2541.nextPosition();
            } else {
                goto out_peg_2542;
            }
        }
        result_peg_2541.setValue((void*) "animelemtime");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2541;
        stream.update(result_peg_2541.getPosition());
        
        
        return result_peg_2541;
        out_peg_2542:
        Result result_peg_2543(myposition);
        
        for (int i = 0; i < 10; i++){
            if (compareCharCase("animelemno"[i], stream.get(result_peg_2543.getPosition()))){
                result_peg_2543.nextPosition();
            } else {
                goto out_peg_2544;
            }
        }
        result_peg_2543.setValue((void*) "animelemno");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2543;
        stream.update(result_peg_2543.getPosition());
        
        
        return result_peg_2543;
        out_peg_2544:
        Result result_peg_2545(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("animexist"[i], stream.get(result_peg_2545.getPosition()))){
                result_peg_2545.nextPosition();
            } else {
                goto out_peg_2546;
            }
        }
        result_peg_2545.setValue((void*) "animexist");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2545;
        stream.update(result_peg_2545.getPosition());
        
        
        return result_peg_2545;
        out_peg_2546:
        Result result_peg_2547(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projguarded"[i], stream.get(result_peg_2547.getPosition()))){
                result_peg_2547.nextPosition();
            } else {
                goto out_peg_2548;
            }
        }
        result_peg_2547.setValue((void*) "projguarded");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2547;
        stream.update(result_peg_2547.getPosition());
        
        
        return result_peg_2547;
        out_peg_2548:
        Result result_peg_2549(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projhittime"[i], stream.get(result_peg_2549.getPosition()))){
                result_peg_2549.nextPosition();
            } else {
                goto out_peg_2550;
            }
        }
        result_peg_2549.setValue((void*) "projhittime");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2549;
        stream.update(result_peg_2549.getPosition());
        
        
        return result_peg_2549;
        out_peg_2550:
        Result result_peg_2551(myposition);
        
        for (int i = 0; i < 15; i++){
            if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2551.getPosition()))){
                result_peg_2551.nextPosition();
            } else {
                goto out_peg_2552;
            }
        }
        result_peg_2551.setValue((void*) "projcontacttime");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2551;
        stream.update(result_peg_2551.getPosition());
        
        
        return result_peg_2551;
        out_peg_2552:
        Result result_peg_2553(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2553.getPosition()))){
                result_peg_2553.nextPosition();
            } else {
                goto out_peg_2554;
            }
        }
        result_peg_2553.setValue((void*) "numhelper");
        
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = result_peg_2553;
        stream.update(result_peg_2553.getPosition());
        
        
        return result_peg_2553;
        out_peg_2554:
    
        if (column_peg_2490.chunk10 == 0){
            column_peg_2490.chunk10 = new Chunk10();
        }
        column_peg_2490.chunk10->chunk_function_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2556 = stream.getColumn(position);
    if (column_peg_2556.chunk10 != 0 && column_peg_2556.chunk10->chunk_function_rest.calculated()){
        return column_peg_2556.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2614(stream, "function_rest");
    int myposition = position;
    
    
    
    Result result_peg_2557(myposition);
        
        {
        
            {
                    
                    result_peg_2557.reset();
                    do{
                        Result result_peg_2569(result_peg_2557.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2569.getPosition()))){
                                    result_peg_2569.nextPosition();
                                } else {
                                    goto out_peg_2572;
                                }
                            }
                            result_peg_2569.setValue((void*) " ");
                                
                        }
                        goto success_peg_2570;
                        out_peg_2572:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2569.getPosition()))){
                                    result_peg_2569.nextPosition();
                                } else {
                                    goto out_peg_2574;
                                }
                            }
                            result_peg_2569.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2570;
                        out_peg_2574:
                        goto loop_peg_2568;
                        success_peg_2570:
                        ;
                        result_peg_2557.addResult(result_peg_2569);
                    } while (true);
                    loop_peg_2568:
                    ;
                            
                }
                goto success_peg_2559;
                goto out_peg_2575;
                success_peg_2559:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2557.getPosition()))){
                        result_peg_2557.nextPosition();
                    } else {
                        goto out_peg_2575;
                    }
                }
                result_peg_2557.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2557.reset();
                    do{
                        Result result_peg_2588(result_peg_2557.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2588.getPosition()))){
                                    result_peg_2588.nextPosition();
                                } else {
                                    goto out_peg_2591;
                                }
                            }
                            result_peg_2588.setValue((void*) " ");
                                
                        }
                        goto success_peg_2589;
                        out_peg_2591:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2588.getPosition()))){
                                    result_peg_2588.nextPosition();
                                } else {
                                    goto out_peg_2593;
                                }
                            }
                            result_peg_2588.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2589;
                        out_peg_2593:
                        goto loop_peg_2587;
                        success_peg_2589:
                        ;
                        result_peg_2557.addResult(result_peg_2588);
                    } while (true);
                    loop_peg_2587:
                    ;
                            
                }
                goto success_peg_2578;
                goto out_peg_2575;
                success_peg_2578:
                ;
            
            
            
            int save_peg_2595 = result_peg_2557.getPosition();
                
                result_peg_2557 = rule_all_compare(stream, result_peg_2557.getPosition());
                if (result_peg_2557.error()){
                    
                    result_peg_2557 = Result(save_peg_2595);
                    result_peg_2557.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_2557.reset();
                    do{
                        Result result_peg_2607(result_peg_2557.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2607.getPosition()))){
                                    result_peg_2607.nextPosition();
                                } else {
                                    goto out_peg_2610;
                                }
                            }
                            result_peg_2607.setValue((void*) " ");
                                
                        }
                        goto success_peg_2608;
                        out_peg_2610:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2607.getPosition()))){
                                    result_peg_2607.nextPosition();
                                } else {
                                    goto out_peg_2612;
                                }
                            }
                            result_peg_2607.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2608;
                        out_peg_2612:
                        goto loop_peg_2606;
                        success_peg_2608:
                        ;
                        result_peg_2557.addResult(result_peg_2607);
                    } while (true);
                    loop_peg_2606:
                    ;
                            
                }
                goto success_peg_2597;
                goto out_peg_2575;
                success_peg_2597:
                ;
            
            
            
            result_peg_2557 = rule_value(stream, result_peg_2557.getPosition());
                if (result_peg_2557.error()){
                    goto out_peg_2575;
                }
            
            
        }
        
        if (column_peg_2556.chunk10 == 0){
            column_peg_2556.chunk10 = new Chunk10();
        }
        column_peg_2556.chunk10->chunk_function_rest = result_peg_2557;
        stream.update(result_peg_2557.getPosition());
        
        
        return result_peg_2557;
        out_peg_2575:
        Result result_peg_2613(myposition);
        
        
        
        if (column_peg_2556.chunk10 == 0){
            column_peg_2556.chunk10 = new Chunk10();
        }
        column_peg_2556.chunk10->chunk_function_rest = result_peg_2613;
        stream.update(result_peg_2613.getPosition());
        
        
        return result_peg_2613;
    
        if (column_peg_2556.chunk10 == 0){
            column_peg_2556.chunk10 = new Chunk10();
        }
        column_peg_2556.chunk10->chunk_function_rest = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2615 = stream.getColumn(position);
    if (column_peg_2615.chunk10 != 0 && column_peg_2615.chunk10->chunk_keys.calculated()){
        return column_peg_2615.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_2618(stream, "keys");
    int myposition = position;
    
    
    
    Result result_peg_2616(myposition);
        
        result_peg_2616 = rule_key_value_list(stream, result_peg_2616.getPosition());
        if (result_peg_2616.error()){
            goto out_peg_2617;
        }
        
        if (column_peg_2615.chunk10 == 0){
            column_peg_2615.chunk10 = new Chunk10();
        }
        column_peg_2615.chunk10->chunk_keys = result_peg_2616;
        stream.update(result_peg_2616.getPosition());
        
        
        return result_peg_2616;
        out_peg_2617:
    
        if (column_peg_2615.chunk10 == 0){
            column_peg_2615.chunk10 = new Chunk10();
        }
        column_peg_2615.chunk10->chunk_keys = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2619 = stream.getColumn(position);
    if (column_peg_2619.chunk10 != 0 && column_peg_2619.chunk10->chunk_key_value_list.calculated()){
        return column_peg_2619.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2663(stream, "key_value_list");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_2620(myposition);
        
        {
        
            result_peg_2620 = rule_key(stream, result_peg_2620.getPosition());
                if (result_peg_2620.error()){
                    goto out_peg_2622;
                }
                first = result_peg_2620.getValues();
            
            
            
            result_peg_2620.reset();
                do{
                    Result result_peg_2625(result_peg_2620.getPosition());
                    {
                    
                        {
                                
                                result_peg_2625.reset();
                                do{
                                    Result result_peg_2637(result_peg_2625.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2637.getPosition()))){
                                                result_peg_2637.nextPosition();
                                            } else {
                                                goto out_peg_2640;
                                            }
                                        }
                                        result_peg_2637.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2638;
                                    out_peg_2640:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2637.getPosition()))){
                                                result_peg_2637.nextPosition();
                                            } else {
                                                goto out_peg_2642;
                                            }
                                        }
                                        result_peg_2637.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2638;
                                    out_peg_2642:
                                    goto loop_peg_2636;
                                    success_peg_2638:
                                    ;
                                    result_peg_2625.addResult(result_peg_2637);
                                } while (true);
                                loop_peg_2636:
                                ;
                                        
                            }
                            goto success_peg_2627;
                            goto loop_peg_2624;
                            success_peg_2627:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2625.getPosition()))){
                                    result_peg_2625.nextPosition();
                                } else {
                                    goto loop_peg_2624;
                                }
                            }
                            result_peg_2625.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2625.reset();
                                do{
                                    Result result_peg_2655(result_peg_2625.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2655.getPosition()))){
                                                result_peg_2655.nextPosition();
                                            } else {
                                                goto out_peg_2658;
                                            }
                                        }
                                        result_peg_2655.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2656;
                                    out_peg_2658:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2655.getPosition()))){
                                                result_peg_2655.nextPosition();
                                            } else {
                                                goto out_peg_2660;
                                            }
                                        }
                                        result_peg_2655.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2656;
                                    out_peg_2660:
                                    goto loop_peg_2654;
                                    success_peg_2656:
                                    ;
                                    result_peg_2625.addResult(result_peg_2655);
                                } while (true);
                                loop_peg_2654:
                                ;
                                        
                            }
                            goto success_peg_2645;
                            goto loop_peg_2624;
                            success_peg_2645:
                            ;
                        
                        
                        
                        result_peg_2625 = rule_key(stream, result_peg_2625.getPosition());
                            if (result_peg_2625.error()){
                                goto loop_peg_2624;
                            }
                        
                        
                    }
                    result_peg_2620.addResult(result_peg_2625);
                } while (true);
                loop_peg_2624:
                ;
                rest = result_peg_2620.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyList(first, rest);
                    result_peg_2620.setValue(value);
                }
            
            
        }
        
        if (column_peg_2619.chunk10 == 0){
            column_peg_2619.chunk10 = new Chunk10();
        }
        column_peg_2619.chunk10->chunk_key_value_list = result_peg_2620;
        stream.update(result_peg_2620.getPosition());
        
        
        return result_peg_2620;
        out_peg_2622:
        Result result_peg_2661(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = makeEmptyKeyList();
                    result_peg_2661.setValue(value);
                }
            
            
        }
        
        if (column_peg_2619.chunk10 == 0){
            column_peg_2619.chunk10 = new Chunk10();
        }
        column_peg_2619.chunk10->chunk_key_value_list = result_peg_2661;
        stream.update(result_peg_2661.getPosition());
        
        
        return result_peg_2661;
    
        if (column_peg_2619.chunk10 == 0){
            column_peg_2619.chunk10 = new Chunk10();
        }
        column_peg_2619.chunk10->chunk_key_value_list = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2664 = stream.getColumn(position);
    if (column_peg_2664.chunk10 != 0 && column_peg_2664.chunk10->chunk_key.calculated()){
        return column_peg_2664.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_2672(stream, "key");
    int myposition = position;
    
    
    Value ok;
    Result result_peg_2665(myposition);
        
        {
        
            result_peg_2665 = rule_key_real(stream, result_peg_2665.getPosition());
                if (result_peg_2665.error()){
                    goto out_peg_2667;
                }
            
            Result result_peg_2666 = result_peg_2665;
            
            result_peg_2665 = rule_key_rest(stream, result_peg_2665.getPosition(), result_peg_2666.getValues());
                if (result_peg_2665.error()){
                    goto out_peg_2667;
                }
                ok = result_peg_2665.getValues();
            
            
            
            Result result_peg_2671(result_peg_2665);
                result_peg_2671 = rule_identifier(stream, result_peg_2671.getPosition());
                if (result_peg_2671.error()){
                    goto not_peg_2670;
                }
                goto out_peg_2667;
                not_peg_2670:
                result_peg_2665.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = ok;
                    result_peg_2665.setValue(value);
                }
            
            
        }
        
        if (column_peg_2664.chunk10 == 0){
            column_peg_2664.chunk10 = new Chunk10();
        }
        column_peg_2664.chunk10->chunk_key = result_peg_2665;
        stream.update(result_peg_2665.getPosition());
        
        
        return result_peg_2665;
        out_peg_2667:
    
        if (column_peg_2664.chunk10 == 0){
            column_peg_2664.chunk10 = new Chunk10();
        }
        column_peg_2664.chunk10->chunk_key = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2716(stream, "key_rest");
    int myposition = position;
    
    tail_peg_2675:
    Value another;
        Value new_left;
    Result result_peg_2674(myposition);
        {
        
            {
                    
                    result_peg_2674.reset();
                    do{
                        Result result_peg_2687(result_peg_2674.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2687.getPosition()))){
                                    result_peg_2687.nextPosition();
                                } else {
                                    goto out_peg_2690;
                                }
                            }
                            result_peg_2687.setValue((void*) " ");
                                
                        }
                        goto success_peg_2688;
                        out_peg_2690:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2687.getPosition()))){
                                    result_peg_2687.nextPosition();
                                } else {
                                    goto out_peg_2692;
                                }
                            }
                            result_peg_2687.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2688;
                        out_peg_2692:
                        goto loop_peg_2686;
                        success_peg_2688:
                        ;
                        result_peg_2674.addResult(result_peg_2687);
                    } while (true);
                    loop_peg_2686:
                    ;
                            
                }
                goto success_peg_2677;
                goto out_peg_2693;
                success_peg_2677:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2674.getPosition()))){
                        result_peg_2674.nextPosition();
                    } else {
                        goto out_peg_2693;
                    }
                }
                result_peg_2674.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_2674.reset();
                    do{
                        Result result_peg_2706(result_peg_2674.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2706.getPosition()))){
                                    result_peg_2706.nextPosition();
                                } else {
                                    goto out_peg_2709;
                                }
                            }
                            result_peg_2706.setValue((void*) " ");
                                
                        }
                        goto success_peg_2707;
                        out_peg_2709:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2706.getPosition()))){
                                    result_peg_2706.nextPosition();
                                } else {
                                    goto out_peg_2711;
                                }
                            }
                            result_peg_2706.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2707;
                        out_peg_2711:
                        goto loop_peg_2705;
                        success_peg_2707:
                        ;
                        result_peg_2674.addResult(result_peg_2706);
                    } while (true);
                    loop_peg_2705:
                    ;
                            
                }
                goto success_peg_2696;
                goto out_peg_2693;
                success_peg_2696:
                ;
            
            
            
            result_peg_2674 = rule_key_real(stream, result_peg_2674.getPosition());
                if (result_peg_2674.error()){
                    goto out_peg_2693;
                }
                another = result_peg_2674.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyCombined(left, another);
                    result_peg_2674.setValue(value);
                }
                new_left = result_peg_2674.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2674.getPosition();
        goto tail_peg_2675;
        out_peg_2693:
        Result result_peg_2714(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2714.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2714.getPosition());
        
        return result_peg_2714;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2717 = stream.getColumn(position);
    if (column_peg_2717.chunk11 != 0 && column_peg_2717.chunk11->chunk_key_real.calculated()){
        return column_peg_2717.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_2724(stream, "key_real");
    int myposition = position;
    
    
    Value mods;
        Value name;
    Result result_peg_2718(myposition);
        
        {
        
            result_peg_2718.reset();
                do{
                    Result result_peg_2721(result_peg_2718.getPosition());
                    result_peg_2721 = rule_key_modifier(stream, result_peg_2721.getPosition());
                    if (result_peg_2721.error()){
                        goto loop_peg_2720;
                    }
                    result_peg_2718.addResult(result_peg_2721);
                } while (true);
                loop_peg_2720:
                ;
                mods = result_peg_2718.getValues();
            
            
            
            result_peg_2718 = rule_key_name(stream, result_peg_2718.getPosition());
                if (result_peg_2718.error()){
                    goto out_peg_2723;
                }
                name = result_peg_2718.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = applyKeyModifiers(mods, makeKey(name));
                    result_peg_2718.setValue(value);
                }
            
            
        }
        
        if (column_peg_2717.chunk11 == 0){
            column_peg_2717.chunk11 = new Chunk11();
        }
        column_peg_2717.chunk11->chunk_key_real = result_peg_2718;
        stream.update(result_peg_2718.getPosition());
        
        
        return result_peg_2718;
        out_peg_2723:
    
        if (column_peg_2717.chunk11 == 0){
            column_peg_2717.chunk11 = new Chunk11();
        }
        column_peg_2717.chunk11->chunk_key_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2725 = stream.getColumn(position);
    if (column_peg_2725.chunk11 != 0 && column_peg_2725.chunk11->chunk_key_modifier.calculated()){
        return column_peg_2725.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2746(stream, "key_modifier");
    int myposition = position;
    
    
    Value num;
    Result result_peg_2726(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2726.getPosition()))){
                        result_peg_2726.nextPosition();
                    } else {
                        goto out_peg_2728;
                    }
                }
                result_peg_2726.setValue((void*) "~");
            
            
            
            result_peg_2726.reset();
                do{
                    Result result_peg_2731(result_peg_2726.getPosition());
                    {
                        
                        char letter_peg_2736 = stream.get(result_peg_2731.getPosition());
                        if (letter_peg_2736 != '\0' && strchr("0123456789", letter_peg_2736) != NULL){
                            result_peg_2731.nextPosition();
                            result_peg_2731.setValue((void*) (long) letter_peg_2736);
                        } else {
                            goto out_peg_2735;
                        }
                        
                    }
                    goto success_peg_2732;
                    out_peg_2735:
                    goto loop_peg_2730;
                    success_peg_2732:
                    ;
                    result_peg_2726.addResult(result_peg_2731);
                } while (true);
                loop_peg_2730:
                ;
                num = result_peg_2726.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new ReleaseKeyModifier((int) *parseDouble(num));
                    result_peg_2726.setValue(value);
                }
            
            
        }
        
        if (column_peg_2725.chunk11 == 0){
            column_peg_2725.chunk11 = new Chunk11();
        }
        column_peg_2725.chunk11->chunk_key_modifier = result_peg_2726;
        stream.update(result_peg_2726.getPosition());
        
        
        return result_peg_2726;
        out_peg_2728:
        Result result_peg_2737(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("$"[i], stream.get(result_peg_2737.getPosition()))){
                        result_peg_2737.nextPosition();
                    } else {
                        goto out_peg_2739;
                    }
                }
                result_peg_2737.setValue((void*) "$");
            
            
            
            {
                    Value value((void*) 0);
                    value = new DirectionKeyModifier();
                    result_peg_2737.setValue(value);
                }
            
            
        }
        
        if (column_peg_2725.chunk11 == 0){
            column_peg_2725.chunk11 = new Chunk11();
        }
        column_peg_2725.chunk11->chunk_key_modifier = result_peg_2737;
        stream.update(result_peg_2737.getPosition());
        
        
        return result_peg_2737;
        out_peg_2739:
        Result result_peg_2740(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_2740.getPosition()))){
                        result_peg_2740.nextPosition();
                    } else {
                        goto out_peg_2742;
                    }
                }
                result_peg_2740.setValue((void*) "/");
            
            
            
            {
                    Value value((void*) 0);
                    value = new HeldDownKeyModifier();
                    result_peg_2740.setValue(value);
                }
            
            
        }
        
        if (column_peg_2725.chunk11 == 0){
            column_peg_2725.chunk11 = new Chunk11();
        }
        column_peg_2725.chunk11->chunk_key_modifier = result_peg_2740;
        stream.update(result_peg_2740.getPosition());
        
        
        return result_peg_2740;
        out_peg_2742:
        Result result_peg_2743(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_2743.getPosition()))){
                        result_peg_2743.nextPosition();
                    } else {
                        goto out_peg_2745;
                    }
                }
                result_peg_2743.setValue((void*) ">");
            
            
            
            {
                    Value value((void*) 0);
                    value = new OnlyKeyModifier();
                    result_peg_2743.setValue(value);
                }
            
            
        }
        
        if (column_peg_2725.chunk11 == 0){
            column_peg_2725.chunk11 = new Chunk11();
        }
        column_peg_2725.chunk11->chunk_key_modifier = result_peg_2743;
        stream.update(result_peg_2743.getPosition());
        
        
        return result_peg_2743;
        out_peg_2745:
    
        if (column_peg_2725.chunk11 == 0){
            column_peg_2725.chunk11 = new Chunk11();
        }
        column_peg_2725.chunk11->chunk_key_modifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2747 = stream.getColumn(position);
    if (column_peg_2747.chunk11 != 0 && column_peg_2747.chunk11->chunk_key_name.calculated()){
        return column_peg_2747.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_2778(stream, "key_name");
    int myposition = position;
    
    
    
    Result result_peg_2748(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DB"[i], stream.get(result_peg_2748.getPosition()))){
                result_peg_2748.nextPosition();
            } else {
                goto out_peg_2749;
            }
        }
        result_peg_2748.setValue((void*) "DB");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2748;
        stream.update(result_peg_2748.getPosition());
        
        
        return result_peg_2748;
        out_peg_2749:
        Result result_peg_2750(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("B"[i], stream.get(result_peg_2750.getPosition()))){
                result_peg_2750.nextPosition();
            } else {
                goto out_peg_2751;
            }
        }
        result_peg_2750.setValue((void*) "B");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2750;
        stream.update(result_peg_2750.getPosition());
        
        
        return result_peg_2750;
        out_peg_2751:
        Result result_peg_2752(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DF"[i], stream.get(result_peg_2752.getPosition()))){
                result_peg_2752.nextPosition();
            } else {
                goto out_peg_2753;
            }
        }
        result_peg_2752.setValue((void*) "DF");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2752;
        stream.update(result_peg_2752.getPosition());
        
        
        return result_peg_2752;
        out_peg_2753:
        Result result_peg_2754(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("D"[i], stream.get(result_peg_2754.getPosition()))){
                result_peg_2754.nextPosition();
            } else {
                goto out_peg_2755;
            }
        }
        result_peg_2754.setValue((void*) "D");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2754;
        stream.update(result_peg_2754.getPosition());
        
        
        return result_peg_2754;
        out_peg_2755:
        Result result_peg_2756(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("F"[i], stream.get(result_peg_2756.getPosition()))){
                result_peg_2756.nextPosition();
            } else {
                goto out_peg_2757;
            }
        }
        result_peg_2756.setValue((void*) "F");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2756;
        stream.update(result_peg_2756.getPosition());
        
        
        return result_peg_2756;
        out_peg_2757:
        Result result_peg_2758(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UF"[i], stream.get(result_peg_2758.getPosition()))){
                result_peg_2758.nextPosition();
            } else {
                goto out_peg_2759;
            }
        }
        result_peg_2758.setValue((void*) "UF");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2758;
        stream.update(result_peg_2758.getPosition());
        
        
        return result_peg_2758;
        out_peg_2759:
        Result result_peg_2760(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UB"[i], stream.get(result_peg_2760.getPosition()))){
                result_peg_2760.nextPosition();
            } else {
                goto out_peg_2761;
            }
        }
        result_peg_2760.setValue((void*) "UB");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2760;
        stream.update(result_peg_2760.getPosition());
        
        
        return result_peg_2760;
        out_peg_2761:
        Result result_peg_2762(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("U"[i], stream.get(result_peg_2762.getPosition()))){
                result_peg_2762.nextPosition();
            } else {
                goto out_peg_2763;
            }
        }
        result_peg_2762.setValue((void*) "U");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2762;
        stream.update(result_peg_2762.getPosition());
        
        
        return result_peg_2762;
        out_peg_2763:
        Result result_peg_2764(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("a"[i], stream.get(result_peg_2764.getPosition()))){
                result_peg_2764.nextPosition();
            } else {
                goto out_peg_2765;
            }
        }
        result_peg_2764.setValue((void*) "a");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2764;
        stream.update(result_peg_2764.getPosition());
        
        
        return result_peg_2764;
        out_peg_2765:
        Result result_peg_2766(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("b"[i], stream.get(result_peg_2766.getPosition()))){
                result_peg_2766.nextPosition();
            } else {
                goto out_peg_2767;
            }
        }
        result_peg_2766.setValue((void*) "b");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2766;
        stream.update(result_peg_2766.getPosition());
        
        
        return result_peg_2766;
        out_peg_2767:
        Result result_peg_2768(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("c"[i], stream.get(result_peg_2768.getPosition()))){
                result_peg_2768.nextPosition();
            } else {
                goto out_peg_2769;
            }
        }
        result_peg_2768.setValue((void*) "c");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2768;
        stream.update(result_peg_2768.getPosition());
        
        
        return result_peg_2768;
        out_peg_2769:
        Result result_peg_2770(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("x"[i], stream.get(result_peg_2770.getPosition()))){
                result_peg_2770.nextPosition();
            } else {
                goto out_peg_2771;
            }
        }
        result_peg_2770.setValue((void*) "x");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2770;
        stream.update(result_peg_2770.getPosition());
        
        
        return result_peg_2770;
        out_peg_2771:
        Result result_peg_2772(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("y"[i], stream.get(result_peg_2772.getPosition()))){
                result_peg_2772.nextPosition();
            } else {
                goto out_peg_2773;
            }
        }
        result_peg_2772.setValue((void*) "y");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2772;
        stream.update(result_peg_2772.getPosition());
        
        
        return result_peg_2772;
        out_peg_2773:
        Result result_peg_2774(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("z"[i], stream.get(result_peg_2774.getPosition()))){
                result_peg_2774.nextPosition();
            } else {
                goto out_peg_2775;
            }
        }
        result_peg_2774.setValue((void*) "z");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2774;
        stream.update(result_peg_2774.getPosition());
        
        
        return result_peg_2774;
        out_peg_2775:
        Result result_peg_2776(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("s"[i], stream.get(result_peg_2776.getPosition()))){
                result_peg_2776.nextPosition();
            } else {
                goto out_peg_2777;
            }
        }
        result_peg_2776.setValue((void*) "s");
        
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = result_peg_2776;
        stream.update(result_peg_2776.getPosition());
        
        
        return result_peg_2776;
        out_peg_2777:
    
        if (column_peg_2747.chunk11 == 0){
            column_peg_2747.chunk11 = new Chunk11();
        }
        column_peg_2747.chunk11->chunk_key_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2779 = stream.getColumn(position);
    if (column_peg_2779.chunk11 != 0 && column_peg_2779.chunk11->chunk_value.calculated()){
        return column_peg_2779.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_2797(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_2780(myposition);
        
        result_peg_2780 = rule_float(stream, result_peg_2780.getPosition());
        if (result_peg_2780.error()){
            goto out_peg_2781;
        }
        
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = result_peg_2780;
        stream.update(result_peg_2780.getPosition());
        
        
        return result_peg_2780;
        out_peg_2781:
        Result result_peg_2782(myposition);
        
        result_peg_2782 = rule_integer(stream, result_peg_2782.getPosition());
        if (result_peg_2782.error()){
            goto out_peg_2783;
        }
        
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = result_peg_2782;
        stream.update(result_peg_2782.getPosition());
        
        
        return result_peg_2782;
        out_peg_2783:
        Result result_peg_2784(myposition);
        
        result_peg_2784 = rule_keyword(stream, result_peg_2784.getPosition());
        if (result_peg_2784.error()){
            goto out_peg_2785;
        }
        
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = result_peg_2784;
        stream.update(result_peg_2784.getPosition());
        
        
        return result_peg_2784;
        out_peg_2785:
        Result result_peg_2786(myposition);
        
        {
        
            Result result_peg_2789(result_peg_2786);
                result_peg_2789 = rule_keyword(stream, result_peg_2789.getPosition());
                if (result_peg_2789.error()){
                    goto not_peg_2788;
                }
                goto out_peg_2790;
                not_peg_2788:
                result_peg_2786.setValue((void*)0);
            
            
            
            result_peg_2786 = rule_identifier(stream, result_peg_2786.getPosition());
                if (result_peg_2786.error()){
                    goto out_peg_2790;
                }
            
            
        }
        
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = result_peg_2786;
        stream.update(result_peg_2786.getPosition());
        
        
        return result_peg_2786;
        out_peg_2790:
        Result result_peg_2791(myposition);
        
        result_peg_2791 = rule_range(stream, result_peg_2791.getPosition());
        if (result_peg_2791.error()){
            goto out_peg_2792;
        }
        
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = result_peg_2791;
        stream.update(result_peg_2791.getPosition());
        
        
        return result_peg_2791;
        out_peg_2792:
        Result result_peg_2793(myposition);
        
        result_peg_2793 = rule_string(stream, result_peg_2793.getPosition());
        if (result_peg_2793.error()){
            goto out_peg_2794;
        }
        
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = result_peg_2793;
        stream.update(result_peg_2793.getPosition());
        
        
        return result_peg_2793;
        out_peg_2794:
        Result result_peg_2795(myposition);
        
        result_peg_2795 = rule_hitflag(stream, result_peg_2795.getPosition());
        if (result_peg_2795.error()){
            goto out_peg_2796;
        }
        
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = result_peg_2795;
        stream.update(result_peg_2795.getPosition());
        
        
        return result_peg_2795;
        out_peg_2796:
    
        if (column_peg_2779.chunk11 == 0){
            column_peg_2779.chunk11 = new Chunk11();
        }
        column_peg_2779.chunk11->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2798 = stream.getColumn(position);
    if (column_peg_2798.chunk12 != 0 && column_peg_2798.chunk12->chunk_helper.calculated()){
        return column_peg_2798.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_2857(stream, "helper");
    int myposition = position;
    
    
    Value name;
        Value id;
    Result result_peg_2799(myposition);
        
        {
        
            result_peg_2799 = rule_helper__name(stream, result_peg_2799.getPosition());
                if (result_peg_2799.error()){
                    goto out_peg_2801;
                }
                name = result_peg_2799.getValues();
            
            
            
            {
                    
                    result_peg_2799.reset();
                    do{
                        Result result_peg_2813(result_peg_2799.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2813.getPosition()))){
                                    result_peg_2813.nextPosition();
                                } else {
                                    goto out_peg_2816;
                                }
                            }
                            result_peg_2813.setValue((void*) " ");
                                
                        }
                        goto success_peg_2814;
                        out_peg_2816:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2813.getPosition()))){
                                    result_peg_2813.nextPosition();
                                } else {
                                    goto out_peg_2818;
                                }
                            }
                            result_peg_2813.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2814;
                        out_peg_2818:
                        goto loop_peg_2812;
                        success_peg_2814:
                        ;
                        result_peg_2799.addResult(result_peg_2813);
                    } while (true);
                    loop_peg_2812:
                    ;
                            
                }
                goto success_peg_2803;
                goto out_peg_2801;
                success_peg_2803:
                ;
            
            
            
            int save_peg_2820 = result_peg_2799.getPosition();
                
                result_peg_2799 = rule_helper__expression(stream, result_peg_2799.getPosition());
                if (result_peg_2799.error()){
                    
                    result_peg_2799 = Result(save_peg_2820);
                    result_peg_2799.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_2799.reset();
                    do{
                        Result result_peg_2832(result_peg_2799.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2832.getPosition()))){
                                    result_peg_2832.nextPosition();
                                } else {
                                    goto out_peg_2835;
                                }
                            }
                            result_peg_2832.setValue((void*) " ");
                                
                        }
                        goto success_peg_2833;
                        out_peg_2835:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2832.getPosition()))){
                                    result_peg_2832.nextPosition();
                                } else {
                                    goto out_peg_2837;
                                }
                            }
                            result_peg_2832.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2833;
                        out_peg_2837:
                        goto loop_peg_2831;
                        success_peg_2833:
                        ;
                        result_peg_2799.addResult(result_peg_2832);
                    } while (true);
                    loop_peg_2831:
                    ;
                            
                }
                goto success_peg_2822;
                goto out_peg_2801;
                success_peg_2822:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2799.getPosition()))){
                        result_peg_2799.nextPosition();
                    } else {
                        goto out_peg_2801;
                    }
                }
                result_peg_2799.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2799.reset();
                    do{
                        Result result_peg_2850(result_peg_2799.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2850.getPosition()))){
                                    result_peg_2850.nextPosition();
                                } else {
                                    goto out_peg_2853;
                                }
                            }
                            result_peg_2850.setValue((void*) " ");
                                
                        }
                        goto success_peg_2851;
                        out_peg_2853:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2850.getPosition()))){
                                    result_peg_2850.nextPosition();
                                } else {
                                    goto out_peg_2855;
                                }
                            }
                            result_peg_2850.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2851;
                        out_peg_2855:
                        goto loop_peg_2849;
                        success_peg_2851:
                        ;
                        result_peg_2799.addResult(result_peg_2850);
                    } while (true);
                    loop_peg_2849:
                    ;
                            
                }
                goto success_peg_2840;
                goto out_peg_2801;
                success_peg_2840:
                ;
            
            
            
            result_peg_2799 = rule_helper__identifier(stream, result_peg_2799.getPosition());
                if (result_peg_2799.error()){
                    goto out_peg_2801;
                }
                id = result_peg_2799.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHelper(name, id);
                    result_peg_2799.setValue(value);
                }
            
            
        }
        
        if (column_peg_2798.chunk12 == 0){
            column_peg_2798.chunk12 = new Chunk12();
        }
        column_peg_2798.chunk12->chunk_helper = result_peg_2799;
        stream.update(result_peg_2799.getPosition());
        
        
        return result_peg_2799;
        out_peg_2801:
    
        if (column_peg_2798.chunk12 == 0){
            column_peg_2798.chunk12 = new Chunk12();
        }
        column_peg_2798.chunk12->chunk_helper = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_2858 = stream.getColumn(position);
    if (column_peg_2858.chunk12 != 0 && column_peg_2858.chunk12->chunk_helper__expression.calculated()){
        return column_peg_2858.chunk12->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_2897(stream, "helper__expression");
    int myposition = position;
    
    
    
    Result result_peg_2859(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2859.getPosition()))){
                        result_peg_2859.nextPosition();
                    } else {
                        goto out_peg_2861;
                    }
                }
                result_peg_2859.setValue((void*) "(");
            
            
            
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
            
            
            
            result_peg_2859 = rule_expr_c(stream, result_peg_2859.getPosition());
                if (result_peg_2859.error()){
                    goto out_peg_2861;
                }
            
            
            
            {
                    
                    result_peg_2859.reset();
                    do{
                        Result result_peg_2891(result_peg_2859.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2891.getPosition()))){
                                    result_peg_2891.nextPosition();
                                } else {
                                    goto out_peg_2894;
                                }
                            }
                            result_peg_2891.setValue((void*) " ");
                                
                        }
                        goto success_peg_2892;
                        out_peg_2894:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2891.getPosition()))){
                                    result_peg_2891.nextPosition();
                                } else {
                                    goto out_peg_2896;
                                }
                            }
                            result_peg_2891.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2892;
                        out_peg_2896:
                        goto loop_peg_2890;
                        success_peg_2892:
                        ;
                        result_peg_2859.addResult(result_peg_2891);
                    } while (true);
                    loop_peg_2890:
                    ;
                            
                }
                goto success_peg_2881;
                goto out_peg_2861;
                success_peg_2881:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2859.getPosition()))){
                        result_peg_2859.nextPosition();
                    } else {
                        goto out_peg_2861;
                    }
                }
                result_peg_2859.setValue((void*) ")");
            
            
        }
        
        if (column_peg_2858.chunk12 == 0){
            column_peg_2858.chunk12 = new Chunk12();
        }
        column_peg_2858.chunk12->chunk_helper__expression = result_peg_2859;
        stream.update(result_peg_2859.getPosition());
        
        
        return result_peg_2859;
        out_peg_2861:
    
        if (column_peg_2858.chunk12 == 0){
            column_peg_2858.chunk12 = new Chunk12();
        }
        column_peg_2858.chunk12->chunk_helper__expression = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_2898 = stream.getColumn(position);
    if (column_peg_2898.chunk12 != 0 && column_peg_2898.chunk12->chunk_helper__name.calculated()){
        return column_peg_2898.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_2915(stream, "helper__name");
    int myposition = position;
    
    
    
    Result result_peg_2899(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("parent"[i], stream.get(result_peg_2899.getPosition()))){
                result_peg_2899.nextPosition();
            } else {
                goto out_peg_2900;
            }
        }
        result_peg_2899.setValue((void*) "parent");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2899;
        stream.update(result_peg_2899.getPosition());
        
        
        return result_peg_2899;
        out_peg_2900:
        Result result_peg_2901(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("root"[i], stream.get(result_peg_2901.getPosition()))){
                result_peg_2901.nextPosition();
            } else {
                goto out_peg_2902;
            }
        }
        result_peg_2901.setValue((void*) "root");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2901;
        stream.update(result_peg_2901.getPosition());
        
        
        return result_peg_2901;
        out_peg_2902:
        Result result_peg_2903(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("helper"[i], stream.get(result_peg_2903.getPosition()))){
                result_peg_2903.nextPosition();
            } else {
                goto out_peg_2904;
            }
        }
        result_peg_2903.setValue((void*) "helper");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2903;
        stream.update(result_peg_2903.getPosition());
        
        
        return result_peg_2903;
        out_peg_2904:
        Result result_peg_2905(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("target"[i], stream.get(result_peg_2905.getPosition()))){
                result_peg_2905.nextPosition();
            } else {
                goto out_peg_2906;
            }
        }
        result_peg_2905.setValue((void*) "target");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2905;
        stream.update(result_peg_2905.getPosition());
        
        
        return result_peg_2905;
        out_peg_2906:
        Result result_peg_2907(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("partner"[i], stream.get(result_peg_2907.getPosition()))){
                result_peg_2907.nextPosition();
            } else {
                goto out_peg_2908;
            }
        }
        result_peg_2907.setValue((void*) "partner");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2907;
        stream.update(result_peg_2907.getPosition());
        
        
        return result_peg_2907;
        out_peg_2908:
        Result result_peg_2909(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("enemyNear"[i], stream.get(result_peg_2909.getPosition()))){
                result_peg_2909.nextPosition();
            } else {
                goto out_peg_2910;
            }
        }
        result_peg_2909.setValue((void*) "enemyNear");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2909;
        stream.update(result_peg_2909.getPosition());
        
        
        return result_peg_2909;
        out_peg_2910:
        Result result_peg_2911(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("enemy"[i], stream.get(result_peg_2911.getPosition()))){
                result_peg_2911.nextPosition();
            } else {
                goto out_peg_2912;
            }
        }
        result_peg_2911.setValue((void*) "enemy");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2911;
        stream.update(result_peg_2911.getPosition());
        
        
        return result_peg_2911;
        out_peg_2912:
        Result result_peg_2913(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("playerid"[i], stream.get(result_peg_2913.getPosition()))){
                result_peg_2913.nextPosition();
            } else {
                goto out_peg_2914;
            }
        }
        result_peg_2913.setValue((void*) "playerid");
        
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = result_peg_2913;
        stream.update(result_peg_2913.getPosition());
        
        
        return result_peg_2913;
        out_peg_2914:
    
        if (column_peg_2898.chunk12 == 0){
            column_peg_2898.chunk12 = new Chunk12();
        }
        column_peg_2898.chunk12->chunk_helper__name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_2916 = stream.getColumn(position);
    if (column_peg_2916.chunk12 != 0 && column_peg_2916.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_2916.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_2926(stream, "helper__identifier");
    int myposition = position;
    
    
    
    Result result_peg_2917(myposition);
        
        result_peg_2917 = rule_function(stream, result_peg_2917.getPosition());
        if (result_peg_2917.error()){
            goto out_peg_2918;
        }
        
        if (column_peg_2916.chunk12 == 0){
            column_peg_2916.chunk12 = new Chunk12();
        }
        column_peg_2916.chunk12->chunk_helper__identifier = result_peg_2917;
        stream.update(result_peg_2917.getPosition());
        
        
        return result_peg_2917;
        out_peg_2918:
        Result result_peg_2919(myposition);
        
        result_peg_2919 = rule_keyword(stream, result_peg_2919.getPosition());
        if (result_peg_2919.error()){
            goto out_peg_2920;
        }
        
        if (column_peg_2916.chunk12 == 0){
            column_peg_2916.chunk12 = new Chunk12();
        }
        column_peg_2916.chunk12->chunk_helper__identifier = result_peg_2919;
        stream.update(result_peg_2919.getPosition());
        
        
        return result_peg_2919;
        out_peg_2920:
        Result result_peg_2921(myposition);
        
        {
        
            Result result_peg_2924(result_peg_2921);
                result_peg_2924 = rule_keyword(stream, result_peg_2924.getPosition());
                if (result_peg_2924.error()){
                    goto not_peg_2923;
                }
                goto out_peg_2925;
                not_peg_2923:
                result_peg_2921.setValue((void*)0);
            
            
            
            result_peg_2921 = rule_identifier(stream, result_peg_2921.getPosition());
                if (result_peg_2921.error()){
                    goto out_peg_2925;
                }
            
            
        }
        
        if (column_peg_2916.chunk12 == 0){
            column_peg_2916.chunk12 = new Chunk12();
        }
        column_peg_2916.chunk12->chunk_helper__identifier = result_peg_2921;
        stream.update(result_peg_2921.getPosition());
        
        
        return result_peg_2921;
        out_peg_2925:
    
        if (column_peg_2916.chunk12 == 0){
            column_peg_2916.chunk12 = new Chunk12();
        }
        column_peg_2916.chunk12->chunk_helper__identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_2927 = stream.getColumn(position);
    if (column_peg_2927.chunk12 != 0 && column_peg_2927.chunk12->chunk_hitflag.calculated()){
        return column_peg_2927.chunk12->chunk_hitflag;
    }
    
    RuleTrace trace_peg_2952(stream, "hitflag");
    int myposition = position;
    
    
    
    Result result_peg_2928(myposition);
        
        {
        
            result_peg_2928.reset();
                do{
                    Result result_peg_2931(result_peg_2928.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("A"[i], stream.get(result_peg_2931.getPosition()))){
                                result_peg_2931.nextPosition();
                            } else {
                                goto out_peg_2934;
                            }
                        }
                        result_peg_2931.setValue((void*) "A");
                            
                    }
                    goto success_peg_2932;
                    out_peg_2934:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("M"[i], stream.get(result_peg_2931.getPosition()))){
                                result_peg_2931.nextPosition();
                            } else {
                                goto out_peg_2936;
                            }
                        }
                        result_peg_2931.setValue((void*) "M");
                            
                    }
                    goto success_peg_2932;
                    out_peg_2936:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("F"[i], stream.get(result_peg_2931.getPosition()))){
                                result_peg_2931.nextPosition();
                            } else {
                                goto out_peg_2938;
                            }
                        }
                        result_peg_2931.setValue((void*) "F");
                            
                    }
                    goto success_peg_2932;
                    out_peg_2938:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("D"[i], stream.get(result_peg_2931.getPosition()))){
                                result_peg_2931.nextPosition();
                            } else {
                                goto out_peg_2940;
                            }
                        }
                        result_peg_2931.setValue((void*) "D");
                            
                    }
                    goto success_peg_2932;
                    out_peg_2940:
                    goto loop_peg_2930;
                    success_peg_2932:
                    ;
                    result_peg_2928.addResult(result_peg_2931);
                } while (true);
                loop_peg_2930:
                if (result_peg_2928.matches() == 0){
                    goto out_peg_2941;
                }
            
            
            
            int save_peg_2943 = result_peg_2928.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2928.getPosition()))){
                            result_peg_2928.nextPosition();
                        } else {
                            goto out_peg_2946;
                        }
                    }
                    result_peg_2928.setValue((void*) "+");
                        
                }
                goto success_peg_2944;
                out_peg_2946:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2928.getPosition()))){
                            result_peg_2928.nextPosition();
                        } else {
                            goto out_peg_2948;
                        }
                    }
                    result_peg_2928.setValue((void*) "-");
                        
                }
                goto success_peg_2944;
                out_peg_2948:
                
                result_peg_2928 = Result(save_peg_2943);
                result_peg_2928.setValue((void*) 0);
                
                success_peg_2944:
                ;
            
            
            
            Result result_peg_2951(result_peg_2928);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2951.getPosition()))){
                        result_peg_2951.nextPosition();
                    } else {
                        goto not_peg_2950;
                    }
                }
                result_peg_2951.setValue((void*) ".");
                goto out_peg_2941;
                not_peg_2950:
                result_peg_2928.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSimpleIdentifier("A");
                    result_peg_2928.setValue(value);
                }
            
            
        }
        
        if (column_peg_2927.chunk12 == 0){
            column_peg_2927.chunk12 = new Chunk12();
        }
        column_peg_2927.chunk12->chunk_hitflag = result_peg_2928;
        stream.update(result_peg_2928.getPosition());
        
        
        return result_peg_2928;
        out_peg_2941:
    
        if (column_peg_2927.chunk12 == 0){
            column_peg_2927.chunk12 = new Chunk12();
        }
        column_peg_2927.chunk12->chunk_hitflag = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_2953 = stream.getColumn(position);
    if (column_peg_2953.chunk13 != 0 && column_peg_2953.chunk13->chunk_keyword.calculated()){
        return column_peg_2953.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_2960(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_2954(myposition);
        
        {
        
            result_peg_2954 = rule_keyword_real(stream, result_peg_2954.getPosition());
                if (result_peg_2954.error()){
                    goto out_peg_2956;
                }
            
            Result result_peg_2955 = result_peg_2954;
            
            Result result_peg_2959(result_peg_2954);
                result_peg_2959 = rule_alpha_digit(stream, result_peg_2959.getPosition());
                if (result_peg_2959.error()){
                    goto not_peg_2958;
                }
                goto out_peg_2956;
                not_peg_2958:
                result_peg_2954.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_2955.getValues();
                    result_peg_2954.setValue(value);
                }
            
            
        }
        
        if (column_peg_2953.chunk13 == 0){
            column_peg_2953.chunk13 = new Chunk13();
        }
        column_peg_2953.chunk13->chunk_keyword = result_peg_2954;
        stream.update(result_peg_2954.getPosition());
        
        
        return result_peg_2954;
        out_peg_2956:
    
        if (column_peg_2953.chunk13 == 0){
            column_peg_2953.chunk13 = new Chunk13();
        }
        column_peg_2953.chunk13->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_2961 = stream.getColumn(position);
    if (column_peg_2961.chunk13 != 0 && column_peg_2961.chunk13->chunk_keyword_real.calculated()){
        return column_peg_2961.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_3382(stream, "keyword_real");
    int myposition = position;
    
    
    
    Result result_peg_2962(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_2962.getPosition()))){
                        result_peg_2962.nextPosition();
                    } else {
                        goto out_peg_2964;
                    }
                }
                result_peg_2962.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_2962.reset();
                    do{
                        Result result_peg_2977(result_peg_2962.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2977.getPosition()))){
                                    result_peg_2977.nextPosition();
                                } else {
                                    goto out_peg_2980;
                                }
                            }
                            result_peg_2977.setValue((void*) " ");
                                
                        }
                        goto success_peg_2978;
                        out_peg_2980:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2977.getPosition()))){
                                    result_peg_2977.nextPosition();
                                } else {
                                    goto out_peg_2982;
                                }
                            }
                            result_peg_2977.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2978;
                        out_peg_2982:
                        goto loop_peg_2976;
                        success_peg_2978:
                        ;
                        result_peg_2962.addResult(result_peg_2977);
                    } while (true);
                    loop_peg_2976:
                    if (result_peg_2962.matches() == 0){
                        goto out_peg_2975;
                    }
                    
                }
                goto success_peg_2966;
                out_peg_2975:
                goto out_peg_2964;
                success_peg_2966:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_2962.getPosition()))){
                        result_peg_2962.nextPosition();
                    } else {
                        goto out_peg_2964;
                    }
                }
                result_peg_2962.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel y");
                    result_peg_2962.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_2962;
        stream.update(result_peg_2962.getPosition());
        
        
        return result_peg_2962;
        out_peg_2964:
        Result result_peg_2984(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_2984.getPosition()))){
                        result_peg_2984.nextPosition();
                    } else {
                        goto out_peg_2986;
                    }
                }
                result_peg_2984.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_2984.reset();
                    do{
                        Result result_peg_2999(result_peg_2984.getPosition());
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
                        result_peg_2984.addResult(result_peg_2999);
                    } while (true);
                    loop_peg_2998:
                    if (result_peg_2984.matches() == 0){
                        goto out_peg_2997;
                    }
                    
                }
                goto success_peg_2988;
                out_peg_2997:
                goto out_peg_2986;
                success_peg_2988:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_2984.getPosition()))){
                        result_peg_2984.nextPosition();
                    } else {
                        goto out_peg_2986;
                    }
                }
                result_peg_2984.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel x");
                    result_peg_2984.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_2984;
        stream.update(result_peg_2984.getPosition());
        
        
        return result_peg_2984;
        out_peg_2986:
        Result result_peg_3006(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_3006.getPosition()))){
                        result_peg_3006.nextPosition();
                    } else {
                        goto out_peg_3008;
                    }
                }
                result_peg_3006.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_3006.reset();
                    do{
                        Result result_peg_3021(result_peg_3006.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3021.getPosition()))){
                                    result_peg_3021.nextPosition();
                                } else {
                                    goto out_peg_3024;
                                }
                            }
                            result_peg_3021.setValue((void*) " ");
                                
                        }
                        goto success_peg_3022;
                        out_peg_3024:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3021.getPosition()))){
                                    result_peg_3021.nextPosition();
                                } else {
                                    goto out_peg_3026;
                                }
                            }
                            result_peg_3021.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3022;
                        out_peg_3026:
                        goto loop_peg_3020;
                        success_peg_3022:
                        ;
                        result_peg_3006.addResult(result_peg_3021);
                    } while (true);
                    loop_peg_3020:
                    if (result_peg_3006.matches() == 0){
                        goto out_peg_3019;
                    }
                    
                }
                goto success_peg_3010;
                out_peg_3019:
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
                    value = makeKeyword("pos y");
                    result_peg_3006.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3006;
        stream.update(result_peg_3006.getPosition());
        
        
        return result_peg_3006;
        out_peg_3008:
        Result result_peg_3028(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_3028.getPosition()))){
                        result_peg_3028.nextPosition();
                    } else {
                        goto out_peg_3030;
                    }
                }
                result_peg_3028.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_3028.reset();
                    do{
                        Result result_peg_3043(result_peg_3028.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3043.getPosition()))){
                                    result_peg_3043.nextPosition();
                                } else {
                                    goto out_peg_3046;
                                }
                            }
                            result_peg_3043.setValue((void*) " ");
                                
                        }
                        goto success_peg_3044;
                        out_peg_3046:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3043.getPosition()))){
                                    result_peg_3043.nextPosition();
                                } else {
                                    goto out_peg_3048;
                                }
                            }
                            result_peg_3043.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3044;
                        out_peg_3048:
                        goto loop_peg_3042;
                        success_peg_3044:
                        ;
                        result_peg_3028.addResult(result_peg_3043);
                    } while (true);
                    loop_peg_3042:
                    if (result_peg_3028.matches() == 0){
                        goto out_peg_3041;
                    }
                    
                }
                goto success_peg_3032;
                out_peg_3041:
                goto out_peg_3030;
                success_peg_3032:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3028.getPosition()))){
                        result_peg_3028.nextPosition();
                    } else {
                        goto out_peg_3030;
                    }
                }
                result_peg_3028.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos x");
                    result_peg_3028.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3028;
        stream.update(result_peg_3028.getPosition());
        
        
        return result_peg_3028;
        out_peg_3030:
        Result result_peg_3050(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_3050.getPosition()))){
                        result_peg_3050.nextPosition();
                    } else {
                        goto out_peg_3052;
                    }
                }
                result_peg_3050.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_3050.reset();
                    do{
                        Result result_peg_3065(result_peg_3050.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3065.getPosition()))){
                                    result_peg_3065.nextPosition();
                                } else {
                                    goto out_peg_3068;
                                }
                            }
                            result_peg_3065.setValue((void*) " ");
                                
                        }
                        goto success_peg_3066;
                        out_peg_3068:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3065.getPosition()))){
                                    result_peg_3065.nextPosition();
                                } else {
                                    goto out_peg_3070;
                                }
                            }
                            result_peg_3065.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3066;
                        out_peg_3070:
                        goto loop_peg_3064;
                        success_peg_3066:
                        ;
                        result_peg_3050.addResult(result_peg_3065);
                    } while (true);
                    loop_peg_3064:
                    if (result_peg_3050.matches() == 0){
                        goto out_peg_3063;
                    }
                    
                }
                goto success_peg_3054;
                out_peg_3063:
                goto out_peg_3052;
                success_peg_3054:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3050.getPosition()))){
                        result_peg_3050.nextPosition();
                    } else {
                        goto out_peg_3052;
                    }
                }
                result_peg_3050.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist x");
                    result_peg_3050.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3050;
        stream.update(result_peg_3050.getPosition());
        
        
        return result_peg_3050;
        out_peg_3052:
        Result result_peg_3072(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_3072.getPosition()))){
                        result_peg_3072.nextPosition();
                    } else {
                        goto out_peg_3074;
                    }
                }
                result_peg_3072.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_3072.reset();
                    do{
                        Result result_peg_3087(result_peg_3072.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3087.getPosition()))){
                                    result_peg_3087.nextPosition();
                                } else {
                                    goto out_peg_3090;
                                }
                            }
                            result_peg_3087.setValue((void*) " ");
                                
                        }
                        goto success_peg_3088;
                        out_peg_3090:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3087.getPosition()))){
                                    result_peg_3087.nextPosition();
                                } else {
                                    goto out_peg_3092;
                                }
                            }
                            result_peg_3087.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3088;
                        out_peg_3092:
                        goto loop_peg_3086;
                        success_peg_3088:
                        ;
                        result_peg_3072.addResult(result_peg_3087);
                    } while (true);
                    loop_peg_3086:
                    if (result_peg_3072.matches() == 0){
                        goto out_peg_3085;
                    }
                    
                }
                goto success_peg_3076;
                out_peg_3085:
                goto out_peg_3074;
                success_peg_3076:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3072.getPosition()))){
                        result_peg_3072.nextPosition();
                    } else {
                        goto out_peg_3074;
                    }
                }
                result_peg_3072.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist y");
                    result_peg_3072.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3072;
        stream.update(result_peg_3072.getPosition());
        
        
        return result_peg_3072;
        out_peg_3074:
        Result result_peg_3094(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3094.getPosition()))){
                        result_peg_3094.nextPosition();
                    } else {
                        goto out_peg_3096;
                    }
                }
                result_peg_3094.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3094.reset();
                    do{
                        Result result_peg_3109(result_peg_3094.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3109.getPosition()))){
                                    result_peg_3109.nextPosition();
                                } else {
                                    goto out_peg_3112;
                                }
                            }
                            result_peg_3109.setValue((void*) " ");
                                
                        }
                        goto success_peg_3110;
                        out_peg_3112:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3109.getPosition()))){
                                    result_peg_3109.nextPosition();
                                } else {
                                    goto out_peg_3114;
                                }
                            }
                            result_peg_3109.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3110;
                        out_peg_3114:
                        goto loop_peg_3108;
                        success_peg_3110:
                        ;
                        result_peg_3094.addResult(result_peg_3109);
                    } while (true);
                    loop_peg_3108:
                    if (result_peg_3094.matches() == 0){
                        goto out_peg_3107;
                    }
                    
                }
                goto success_peg_3098;
                out_peg_3107:
                goto out_peg_3096;
                success_peg_3098:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3094.getPosition()))){
                        result_peg_3094.nextPosition();
                    } else {
                        goto out_peg_3096;
                    }
                }
                result_peg_3094.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist x");
                    result_peg_3094.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3094;
        stream.update(result_peg_3094.getPosition());
        
        
        return result_peg_3094;
        out_peg_3096:
        Result result_peg_3116(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3116.getPosition()))){
                        result_peg_3116.nextPosition();
                    } else {
                        goto out_peg_3118;
                    }
                }
                result_peg_3116.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3116.reset();
                    do{
                        Result result_peg_3131(result_peg_3116.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3131.getPosition()))){
                                    result_peg_3131.nextPosition();
                                } else {
                                    goto out_peg_3134;
                                }
                            }
                            result_peg_3131.setValue((void*) " ");
                                
                        }
                        goto success_peg_3132;
                        out_peg_3134:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3131.getPosition()))){
                                    result_peg_3131.nextPosition();
                                } else {
                                    goto out_peg_3136;
                                }
                            }
                            result_peg_3131.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3132;
                        out_peg_3136:
                        goto loop_peg_3130;
                        success_peg_3132:
                        ;
                        result_peg_3116.addResult(result_peg_3131);
                    } while (true);
                    loop_peg_3130:
                    if (result_peg_3116.matches() == 0){
                        goto out_peg_3129;
                    }
                    
                }
                goto success_peg_3120;
                out_peg_3129:
                goto out_peg_3118;
                success_peg_3120:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3116.getPosition()))){
                        result_peg_3116.nextPosition();
                    } else {
                        goto out_peg_3118;
                    }
                }
                result_peg_3116.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist y");
                    result_peg_3116.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3116;
        stream.update(result_peg_3116.getPosition());
        
        
        return result_peg_3116;
        out_peg_3118:
        Result result_peg_3138(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3138.getPosition()))){
                        result_peg_3138.nextPosition();
                    } else {
                        goto out_peg_3140;
                    }
                }
                result_peg_3138.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3138.reset();
                    do{
                        Result result_peg_3153(result_peg_3138.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3153.getPosition()))){
                                    result_peg_3153.nextPosition();
                                } else {
                                    goto out_peg_3156;
                                }
                            }
                            result_peg_3153.setValue((void*) " ");
                                
                        }
                        goto success_peg_3154;
                        out_peg_3156:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3153.getPosition()))){
                                    result_peg_3153.nextPosition();
                                } else {
                                    goto out_peg_3158;
                                }
                            }
                            result_peg_3153.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3154;
                        out_peg_3158:
                        goto loop_peg_3152;
                        success_peg_3154:
                        ;
                        result_peg_3138.addResult(result_peg_3153);
                    } while (true);
                    loop_peg_3152:
                    if (result_peg_3138.matches() == 0){
                        goto out_peg_3151;
                    }
                    
                }
                goto success_peg_3142;
                out_peg_3151:
                goto out_peg_3140;
                success_peg_3142:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3138.getPosition()))){
                        result_peg_3138.nextPosition();
                    } else {
                        goto out_peg_3140;
                    }
                }
                result_peg_3138.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist x");
                    result_peg_3138.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3138;
        stream.update(result_peg_3138.getPosition());
        
        
        return result_peg_3138;
        out_peg_3140:
        Result result_peg_3160(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3160.getPosition()))){
                        result_peg_3160.nextPosition();
                    } else {
                        goto out_peg_3162;
                    }
                }
                result_peg_3160.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3160.reset();
                    do{
                        Result result_peg_3175(result_peg_3160.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3175.getPosition()))){
                                    result_peg_3175.nextPosition();
                                } else {
                                    goto out_peg_3178;
                                }
                            }
                            result_peg_3175.setValue((void*) " ");
                                
                        }
                        goto success_peg_3176;
                        out_peg_3178:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3175.getPosition()))){
                                    result_peg_3175.nextPosition();
                                } else {
                                    goto out_peg_3180;
                                }
                            }
                            result_peg_3175.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3176;
                        out_peg_3180:
                        goto loop_peg_3174;
                        success_peg_3176:
                        ;
                        result_peg_3160.addResult(result_peg_3175);
                    } while (true);
                    loop_peg_3174:
                    if (result_peg_3160.matches() == 0){
                        goto out_peg_3173;
                    }
                    
                }
                goto success_peg_3164;
                out_peg_3173:
                goto out_peg_3162;
                success_peg_3164:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3160.getPosition()))){
                        result_peg_3160.nextPosition();
                    } else {
                        goto out_peg_3162;
                    }
                }
                result_peg_3160.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist y");
                    result_peg_3160.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3160;
        stream.update(result_peg_3160.getPosition());
        
        
        return result_peg_3160;
        out_peg_3162:
        Result result_peg_3182(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3182.getPosition()))){
                        result_peg_3182.nextPosition();
                    } else {
                        goto out_peg_3184;
                    }
                }
                result_peg_3182.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3182.reset();
                    do{
                        Result result_peg_3197(result_peg_3182.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3197.getPosition()))){
                                    result_peg_3197.nextPosition();
                                } else {
                                    goto out_peg_3200;
                                }
                            }
                            result_peg_3197.setValue((void*) " ");
                                
                        }
                        goto success_peg_3198;
                        out_peg_3200:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3197.getPosition()))){
                                    result_peg_3197.nextPosition();
                                } else {
                                    goto out_peg_3202;
                                }
                            }
                            result_peg_3197.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3198;
                        out_peg_3202:
                        goto loop_peg_3196;
                        success_peg_3198:
                        ;
                        result_peg_3182.addResult(result_peg_3197);
                    } while (true);
                    loop_peg_3196:
                    if (result_peg_3182.matches() == 0){
                        goto out_peg_3195;
                    }
                    
                }
                goto success_peg_3186;
                out_peg_3195:
                goto out_peg_3184;
                success_peg_3186:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3182.getPosition()))){
                        result_peg_3182.nextPosition();
                    } else {
                        goto out_peg_3184;
                    }
                }
                result_peg_3182.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist x");
                    result_peg_3182.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3182;
        stream.update(result_peg_3182.getPosition());
        
        
        return result_peg_3182;
        out_peg_3184:
        Result result_peg_3204(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3204.getPosition()))){
                        result_peg_3204.nextPosition();
                    } else {
                        goto out_peg_3206;
                    }
                }
                result_peg_3204.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3204.reset();
                    do{
                        Result result_peg_3219(result_peg_3204.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3219.getPosition()))){
                                    result_peg_3219.nextPosition();
                                } else {
                                    goto out_peg_3222;
                                }
                            }
                            result_peg_3219.setValue((void*) " ");
                                
                        }
                        goto success_peg_3220;
                        out_peg_3222:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3219.getPosition()))){
                                    result_peg_3219.nextPosition();
                                } else {
                                    goto out_peg_3224;
                                }
                            }
                            result_peg_3219.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3220;
                        out_peg_3224:
                        goto loop_peg_3218;
                        success_peg_3220:
                        ;
                        result_peg_3204.addResult(result_peg_3219);
                    } while (true);
                    loop_peg_3218:
                    if (result_peg_3204.matches() == 0){
                        goto out_peg_3217;
                    }
                    
                }
                goto success_peg_3208;
                out_peg_3217:
                goto out_peg_3206;
                success_peg_3208:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3204.getPosition()))){
                        result_peg_3204.nextPosition();
                    } else {
                        goto out_peg_3206;
                    }
                }
                result_peg_3204.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist y");
                    result_peg_3204.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3204;
        stream.update(result_peg_3204.getPosition());
        
        
        return result_peg_3204;
        out_peg_3206:
        Result result_peg_3226(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3226.getPosition()))){
                        result_peg_3226.nextPosition();
                    } else {
                        goto out_peg_3228;
                    }
                }
                result_peg_3226.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3226.reset();
                    do{
                        Result result_peg_3241(result_peg_3226.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3241.getPosition()))){
                                    result_peg_3241.nextPosition();
                                } else {
                                    goto out_peg_3244;
                                }
                            }
                            result_peg_3241.setValue((void*) " ");
                                
                        }
                        goto success_peg_3242;
                        out_peg_3244:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3241.getPosition()))){
                                    result_peg_3241.nextPosition();
                                } else {
                                    goto out_peg_3246;
                                }
                            }
                            result_peg_3241.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3242;
                        out_peg_3246:
                        goto loop_peg_3240;
                        success_peg_3242:
                        ;
                        result_peg_3226.addResult(result_peg_3241);
                    } while (true);
                    loop_peg_3240:
                    if (result_peg_3226.matches() == 0){
                        goto out_peg_3239;
                    }
                    
                }
                goto success_peg_3230;
                out_peg_3239:
                goto out_peg_3228;
                success_peg_3230:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3226.getPosition()))){
                        result_peg_3226.nextPosition();
                    } else {
                        goto out_peg_3228;
                    }
                }
                result_peg_3226.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3226.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3226;
        stream.update(result_peg_3226.getPosition());
        
        
        return result_peg_3226;
        out_peg_3228:
        Result result_peg_3248(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3248.getPosition()))){
                        result_peg_3248.nextPosition();
                    } else {
                        goto out_peg_3250;
                    }
                }
                result_peg_3248.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3248.reset();
                    do{
                        Result result_peg_3263(result_peg_3248.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3263.getPosition()))){
                                    result_peg_3263.nextPosition();
                                } else {
                                    goto out_peg_3266;
                                }
                            }
                            result_peg_3263.setValue((void*) " ");
                                
                        }
                        goto success_peg_3264;
                        out_peg_3266:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3263.getPosition()))){
                                    result_peg_3263.nextPosition();
                                } else {
                                    goto out_peg_3268;
                                }
                            }
                            result_peg_3263.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3264;
                        out_peg_3268:
                        goto loop_peg_3262;
                        success_peg_3264:
                        ;
                        result_peg_3248.addResult(result_peg_3263);
                    } while (true);
                    loop_peg_3262:
                    if (result_peg_3248.matches() == 0){
                        goto out_peg_3261;
                    }
                    
                }
                goto success_peg_3252;
                out_peg_3261:
                goto out_peg_3250;
                success_peg_3252:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3248.getPosition()))){
                        result_peg_3248.nextPosition();
                    } else {
                        goto out_peg_3250;
                    }
                }
                result_peg_3248.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos x");
                    result_peg_3248.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3248;
        stream.update(result_peg_3248.getPosition());
        
        
        return result_peg_3248;
        out_peg_3250:
        Result result_peg_3270(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3270.getPosition()))){
                        result_peg_3270.nextPosition();
                    } else {
                        goto out_peg_3272;
                    }
                }
                result_peg_3270.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3270.reset();
                    do{
                        Result result_peg_3285(result_peg_3270.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3285.getPosition()))){
                                    result_peg_3285.nextPosition();
                                } else {
                                    goto out_peg_3288;
                                }
                            }
                            result_peg_3285.setValue((void*) " ");
                                
                        }
                        goto success_peg_3286;
                        out_peg_3288:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3285.getPosition()))){
                                    result_peg_3285.nextPosition();
                                } else {
                                    goto out_peg_3290;
                                }
                            }
                            result_peg_3285.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3286;
                        out_peg_3290:
                        goto loop_peg_3284;
                        success_peg_3286:
                        ;
                        result_peg_3270.addResult(result_peg_3285);
                    } while (true);
                    loop_peg_3284:
                    if (result_peg_3270.matches() == 0){
                        goto out_peg_3283;
                    }
                    
                }
                goto success_peg_3274;
                out_peg_3283:
                goto out_peg_3272;
                success_peg_3274:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3270.getPosition()))){
                        result_peg_3270.nextPosition();
                    } else {
                        goto out_peg_3272;
                    }
                }
                result_peg_3270.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos y");
                    result_peg_3270.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3270;
        stream.update(result_peg_3270.getPosition());
        
        
        return result_peg_3270;
        out_peg_3272:
        Result result_peg_3292(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3292.getPosition()))){
                        result_peg_3292.nextPosition();
                    } else {
                        goto out_peg_3294;
                    }
                }
                result_peg_3292.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3292.reset();
                    do{
                        Result result_peg_3307(result_peg_3292.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3307.getPosition()))){
                                    result_peg_3307.nextPosition();
                                } else {
                                    goto out_peg_3310;
                                }
                            }
                            result_peg_3307.setValue((void*) " ");
                                
                        }
                        goto success_peg_3308;
                        out_peg_3310:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3307.getPosition()))){
                                    result_peg_3307.nextPosition();
                                } else {
                                    goto out_peg_3312;
                                }
                            }
                            result_peg_3307.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3308;
                        out_peg_3312:
                        goto loop_peg_3306;
                        success_peg_3308:
                        ;
                        result_peg_3292.addResult(result_peg_3307);
                    } while (true);
                    loop_peg_3306:
                    if (result_peg_3292.matches() == 0){
                        goto out_peg_3305;
                    }
                    
                }
                goto success_peg_3296;
                out_peg_3305:
                goto out_peg_3294;
                success_peg_3296:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3292.getPosition()))){
                        result_peg_3292.nextPosition();
                    } else {
                        goto out_peg_3294;
                    }
                }
                result_peg_3292.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3292.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3292;
        stream.update(result_peg_3292.getPosition());
        
        
        return result_peg_3292;
        out_peg_3294:
        Result result_peg_3314(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3314.getPosition()))){
                        result_peg_3314.nextPosition();
                    } else {
                        goto out_peg_3316;
                    }
                }
                result_peg_3314.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3314.reset();
                    do{
                        Result result_peg_3329(result_peg_3314.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3329.getPosition()))){
                                    result_peg_3329.nextPosition();
                                } else {
                                    goto out_peg_3332;
                                }
                            }
                            result_peg_3329.setValue((void*) " ");
                                
                        }
                        goto success_peg_3330;
                        out_peg_3332:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3329.getPosition()))){
                                    result_peg_3329.nextPosition();
                                } else {
                                    goto out_peg_3334;
                                }
                            }
                            result_peg_3329.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3330;
                        out_peg_3334:
                        goto loop_peg_3328;
                        success_peg_3330:
                        ;
                        result_peg_3314.addResult(result_peg_3329);
                    } while (true);
                    loop_peg_3328:
                    if (result_peg_3314.matches() == 0){
                        goto out_peg_3327;
                    }
                    
                }
                goto success_peg_3318;
                out_peg_3327:
                goto out_peg_3316;
                success_peg_3318:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3314.getPosition()))){
                        result_peg_3314.nextPosition();
                    } else {
                        goto out_peg_3316;
                    }
                }
                result_peg_3314.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist y");
                    result_peg_3314.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3314;
        stream.update(result_peg_3314.getPosition());
        
        
        return result_peg_3314;
        out_peg_3316:
        Result result_peg_3336(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3336.getPosition()))){
                        result_peg_3336.nextPosition();
                    } else {
                        goto out_peg_3338;
                    }
                }
                result_peg_3336.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3336.reset();
                    do{
                        Result result_peg_3351(result_peg_3336.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3351.getPosition()))){
                                    result_peg_3351.nextPosition();
                                } else {
                                    goto out_peg_3354;
                                }
                            }
                            result_peg_3351.setValue((void*) " ");
                                
                        }
                        goto success_peg_3352;
                        out_peg_3354:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3351.getPosition()))){
                                    result_peg_3351.nextPosition();
                                } else {
                                    goto out_peg_3356;
                                }
                            }
                            result_peg_3351.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3352;
                        out_peg_3356:
                        goto loop_peg_3350;
                        success_peg_3352:
                        ;
                        result_peg_3336.addResult(result_peg_3351);
                    } while (true);
                    loop_peg_3350:
                    if (result_peg_3336.matches() == 0){
                        goto out_peg_3349;
                    }
                    
                }
                goto success_peg_3340;
                out_peg_3349:
                goto out_peg_3338;
                success_peg_3340:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3336.getPosition()))){
                        result_peg_3336.nextPosition();
                    } else {
                        goto out_peg_3338;
                    }
                }
                result_peg_3336.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist x");
                    result_peg_3336.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3336;
        stream.update(result_peg_3336.getPosition());
        
        
        return result_peg_3336;
        out_peg_3338:
        Result result_peg_3358(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3358.getPosition()))){
                        result_peg_3358.nextPosition();
                    } else {
                        goto out_peg_3360;
                    }
                }
                result_peg_3358.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3358.reset();
                    do{
                        Result result_peg_3373(result_peg_3358.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3373.getPosition()))){
                                    result_peg_3373.nextPosition();
                                } else {
                                    goto out_peg_3376;
                                }
                            }
                            result_peg_3373.setValue((void*) " ");
                                
                        }
                        goto success_peg_3374;
                        out_peg_3376:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3373.getPosition()))){
                                    result_peg_3373.nextPosition();
                                } else {
                                    goto out_peg_3378;
                                }
                            }
                            result_peg_3373.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3374;
                        out_peg_3378:
                        goto loop_peg_3372;
                        success_peg_3374:
                        ;
                        result_peg_3358.addResult(result_peg_3373);
                    } while (true);
                    loop_peg_3372:
                    if (result_peg_3358.matches() == 0){
                        goto out_peg_3371;
                    }
                    
                }
                goto success_peg_3362;
                out_peg_3371:
                goto out_peg_3360;
                success_peg_3362:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3358.getPosition()))){
                        result_peg_3358.nextPosition();
                    } else {
                        goto out_peg_3360;
                    }
                }
                result_peg_3358.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist y");
                    result_peg_3358.setValue(value);
                }
            
            
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3358;
        stream.update(result_peg_3358.getPosition());
        
        
        return result_peg_3358;
        out_peg_3360:
        Result result_peg_3380(myposition);
        
        result_peg_3380 = rule_hitflag(stream, result_peg_3380.getPosition());
        if (result_peg_3380.error()){
            goto out_peg_3381;
        }
        
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = result_peg_3380;
        stream.update(result_peg_3380.getPosition());
        
        
        return result_peg_3380;
        out_peg_3381:
    
        if (column_peg_2961.chunk13 == 0){
            column_peg_2961.chunk13 = new Chunk13();
        }
        column_peg_2961.chunk13->chunk_keyword_real = errorResult;
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

        
