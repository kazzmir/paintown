

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
    
    RuleTrace trace_peg_561(stream, "assignment");
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
                if (compareCharCase("command"[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    goto out_peg_283;
                }
            }
            result_peg_279.setValue((void*) "command");
        
        
        
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
                Value value((void*) 0);
                std::ostringstream out;
                                   out << "command must be followed by some keys at line " << getCurrentLine(line) << " column " << getCurrentColumn(line);
                                   throw ParseException(out.str());
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
    Result result_peg_302(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_304 = stream.getLineInfo(result_peg_302.getPosition());
            line = &line_info_peg_304;
        
        
        
        for (int i = 0; i < 7; i++){
                if (compareCharCase("sparkno"[i], stream.get(result_peg_302.getPosition()))){
                    result_peg_302.nextPosition();
                } else {
                    goto out_peg_306;
                }
            }
            result_peg_302.setValue((void*) "sparkno");
        
        
        
        {
                
                result_peg_302.reset();
                do{
                    Result result_peg_318(result_peg_302.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_318.getPosition()))){
                                result_peg_318.nextPosition();
                            } else {
                                goto out_peg_321;
                            }
                        }
                        result_peg_318.setValue((void*) " ");
                            
                    }
                    goto success_peg_319;
                    out_peg_321:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_318.getPosition()))){
                                result_peg_318.nextPosition();
                            } else {
                                goto out_peg_323;
                            }
                        }
                        result_peg_318.setValue((void*) "\t");
                            
                    }
                    goto success_peg_319;
                    out_peg_323:
                    goto loop_peg_317;
                    success_peg_319:
                    ;
                    result_peg_302.addResult(result_peg_318);
                } while (true);
                loop_peg_317:
                ;
                        
            }
            goto success_peg_308;
            goto out_peg_306;
            success_peg_308:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_302.getPosition()))){
                    result_peg_302.nextPosition();
                } else {
                    goto out_peg_306;
                }
            }
            result_peg_302.setValue((void*) "=");
        
        
        
        {
                
                result_peg_302.reset();
                do{
                    Result result_peg_336(result_peg_302.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_336.getPosition()))){
                                result_peg_336.nextPosition();
                            } else {
                                goto out_peg_339;
                            }
                        }
                        result_peg_336.setValue((void*) " ");
                            
                    }
                    goto success_peg_337;
                    out_peg_339:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_336.getPosition()))){
                                result_peg_336.nextPosition();
                            } else {
                                goto out_peg_341;
                            }
                        }
                        result_peg_336.setValue((void*) "\t");
                            
                    }
                    goto success_peg_337;
                    out_peg_341:
                    goto loop_peg_335;
                    success_peg_337:
                    ;
                    result_peg_302.addResult(result_peg_336);
                } while (true);
                loop_peg_335:
                ;
                        
            }
            goto success_peg_326;
            goto out_peg_306;
            success_peg_326:
            ;
        
        
        
        int save_peg_343 = result_peg_302.getPosition();
            
            for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_302.getPosition()))){
                    result_peg_302.nextPosition();
                } else {
                    
                    result_peg_302 = Result(save_peg_343);
                    result_peg_302.setValue((void*) 0);
                    
                }
            }
            result_peg_302.setValue((void*) "s");
        
        
        
        result_peg_302 = rule_expr(stream, result_peg_302.getPosition());
            if (result_peg_302.error()){
                goto out_peg_306;
            }
            exp = result_peg_302.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("sparkno", exp);
                result_peg_302.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_302;
    stream.update(result_peg_302.getPosition());
    
    
    return result_peg_302;
    out_peg_306:
    Result result_peg_345(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_347 = stream.getLineInfo(result_peg_345.getPosition());
            line = &line_info_peg_347;
        
        
        
        result_peg_345 = rule_identifier(stream, result_peg_345.getPosition());
            if (result_peg_345.error()){
                goto out_peg_349;
            }
            name = result_peg_345.getValues();
        
        
        
        {
                
                result_peg_345.reset();
                do{
                    Result result_peg_361(result_peg_345.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_361.getPosition()))){
                                result_peg_361.nextPosition();
                            } else {
                                goto out_peg_364;
                            }
                        }
                        result_peg_361.setValue((void*) " ");
                            
                    }
                    goto success_peg_362;
                    out_peg_364:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_361.getPosition()))){
                                result_peg_361.nextPosition();
                            } else {
                                goto out_peg_366;
                            }
                        }
                        result_peg_361.setValue((void*) "\t");
                            
                    }
                    goto success_peg_362;
                    out_peg_366:
                    goto loop_peg_360;
                    success_peg_362:
                    ;
                    result_peg_345.addResult(result_peg_361);
                } while (true);
                loop_peg_360:
                ;
                        
            }
            goto success_peg_351;
            goto out_peg_349;
            success_peg_351:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_345.getPosition()))){
                    result_peg_345.nextPosition();
                } else {
                    goto out_peg_349;
                }
            }
            result_peg_345.setValue((void*) "=");
        
        
        
        {
                
                result_peg_345.reset();
                do{
                    Result result_peg_379(result_peg_345.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_379.getPosition()))){
                                result_peg_379.nextPosition();
                            } else {
                                goto out_peg_382;
                            }
                        }
                        result_peg_379.setValue((void*) " ");
                            
                    }
                    goto success_peg_380;
                    out_peg_382:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_379.getPosition()))){
                                result_peg_379.nextPosition();
                            } else {
                                goto out_peg_384;
                            }
                        }
                        result_peg_379.setValue((void*) "\t");
                            
                    }
                    goto success_peg_380;
                    out_peg_384:
                    goto loop_peg_378;
                    success_peg_380:
                    ;
                    result_peg_345.addResult(result_peg_379);
                } while (true);
                loop_peg_378:
                ;
                        
            }
            goto success_peg_369;
            goto out_peg_349;
            success_peg_369:
            ;
        
        
        
        result_peg_345 = rule_expr(stream, result_peg_345.getPosition());
            if (result_peg_345.error()){
                goto out_peg_349;
            }
            exp = result_peg_345.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
                result_peg_345.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_345;
    stream.update(result_peg_345.getPosition());
    
    
    return result_peg_345;
    out_peg_349:
    Result result_peg_386(myposition);
    
    {
    
        result_peg_386 = rule_identifier(stream, result_peg_386.getPosition());
            if (result_peg_386.error()){
                goto out_peg_388;
            }
            name = result_peg_386.getValues();
        
        
        
        {
                
                result_peg_386.reset();
                do{
                    Result result_peg_400(result_peg_386.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_400.getPosition()))){
                                result_peg_400.nextPosition();
                            } else {
                                goto out_peg_403;
                            }
                        }
                        result_peg_400.setValue((void*) " ");
                            
                    }
                    goto success_peg_401;
                    out_peg_403:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_400.getPosition()))){
                                result_peg_400.nextPosition();
                            } else {
                                goto out_peg_405;
                            }
                        }
                        result_peg_400.setValue((void*) "\t");
                            
                    }
                    goto success_peg_401;
                    out_peg_405:
                    goto loop_peg_399;
                    success_peg_401:
                    ;
                    result_peg_386.addResult(result_peg_400);
                } while (true);
                loop_peg_399:
                ;
                        
            }
            goto success_peg_390;
            goto out_peg_388;
            success_peg_390:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_386.getPosition()))){
                    result_peg_386.nextPosition();
                } else {
                    goto out_peg_388;
                }
            }
            result_peg_386.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_386.reset();
                do{
                    Result result_peg_418(result_peg_386.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_418.getPosition()))){
                                result_peg_418.nextPosition();
                            } else {
                                goto out_peg_421;
                            }
                        }
                        result_peg_418.setValue((void*) " ");
                            
                    }
                    goto success_peg_419;
                    out_peg_421:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_418.getPosition()))){
                                result_peg_418.nextPosition();
                            } else {
                                goto out_peg_423;
                            }
                        }
                        result_peg_418.setValue((void*) "\t");
                            
                    }
                    goto success_peg_419;
                    out_peg_423:
                    goto loop_peg_417;
                    success_peg_419:
                    ;
                    result_peg_386.addResult(result_peg_418);
                } while (true);
                loop_peg_417:
                ;
                        
            }
            goto success_peg_408;
            goto out_peg_388;
            success_peg_408:
            ;
        
        
        
        result_peg_386 = rule_expr(stream, result_peg_386.getPosition());
            if (result_peg_386.error()){
                goto out_peg_388;
            }
            exp = result_peg_386.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, negateExpression(exp));
                result_peg_386.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_386;
    stream.update(result_peg_386.getPosition());
    
    
    return result_peg_386;
    out_peg_388:
    Result result_peg_425(myposition);
    
    {
    
        result_peg_425 = rule_identifier(stream, result_peg_425.getPosition());
            if (result_peg_425.error()){
                goto out_peg_427;
            }
            name = result_peg_425.getValues();
        
        
        
        {
                
                result_peg_425.reset();
                do{
                    Result result_peg_439(result_peg_425.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_439.getPosition()))){
                                result_peg_439.nextPosition();
                            } else {
                                goto out_peg_442;
                            }
                        }
                        result_peg_439.setValue((void*) " ");
                            
                    }
                    goto success_peg_440;
                    out_peg_442:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_439.getPosition()))){
                                result_peg_439.nextPosition();
                            } else {
                                goto out_peg_444;
                            }
                        }
                        result_peg_439.setValue((void*) "\t");
                            
                    }
                    goto success_peg_440;
                    out_peg_444:
                    goto loop_peg_438;
                    success_peg_440:
                    ;
                    result_peg_425.addResult(result_peg_439);
                } while (true);
                loop_peg_438:
                ;
                        
            }
            goto success_peg_429;
            goto out_peg_427;
            success_peg_429:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_425.getPosition()))){
                    result_peg_425.nextPosition();
                } else {
                    goto out_peg_427;
                }
            }
            result_peg_425.setValue((void*) "=");
        
        
        
        {
                
                result_peg_425.reset();
                do{
                    Result result_peg_457(result_peg_425.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_457.getPosition()))){
                                result_peg_457.nextPosition();
                            } else {
                                goto out_peg_460;
                            }
                        }
                        result_peg_457.setValue((void*) " ");
                            
                    }
                    goto success_peg_458;
                    out_peg_460:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_457.getPosition()))){
                                result_peg_457.nextPosition();
                            } else {
                                goto out_peg_462;
                            }
                        }
                        result_peg_457.setValue((void*) "\t");
                            
                    }
                    goto success_peg_458;
                    out_peg_462:
                    goto loop_peg_456;
                    success_peg_458:
                    ;
                    result_peg_425.addResult(result_peg_457);
                } while (true);
                loop_peg_456:
                ;
                        
            }
            goto success_peg_447;
            goto out_peg_427;
            success_peg_447:
            ;
        
        
        
        Result result_peg_464(result_peg_425.getPosition());
            result_peg_464 = rule_line_end(stream, result_peg_464.getPosition());
            if (result_peg_464.error()){
                goto out_peg_427;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_425.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_425;
    stream.update(result_peg_425.getPosition());
    
    
    return result_peg_425;
    out_peg_427:
    Result result_peg_465(myposition);
    
    {
    
        result_peg_465 = rule_identifier(stream, result_peg_465.getPosition());
            if (result_peg_465.error()){
                goto out_peg_467;
            }
            name = result_peg_465.getValues();
        
        
        
        {
                
                result_peg_465.reset();
                do{
                    Result result_peg_479(result_peg_465.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_479.getPosition()))){
                                result_peg_479.nextPosition();
                            } else {
                                goto out_peg_482;
                            }
                        }
                        result_peg_479.setValue((void*) " ");
                            
                    }
                    goto success_peg_480;
                    out_peg_482:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_479.getPosition()))){
                                result_peg_479.nextPosition();
                            } else {
                                goto out_peg_484;
                            }
                        }
                        result_peg_479.setValue((void*) "\t");
                            
                    }
                    goto success_peg_480;
                    out_peg_484:
                    goto loop_peg_478;
                    success_peg_480:
                    ;
                    result_peg_465.addResult(result_peg_479);
                } while (true);
                loop_peg_478:
                ;
                        
            }
            goto success_peg_469;
            goto out_peg_467;
            success_peg_469:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_465.getPosition()))){
                    result_peg_465.nextPosition();
                } else {
                    goto out_peg_467;
                }
            }
            result_peg_465.setValue((void*) "(");
        
        
        
        {
                
                result_peg_465.reset();
                do{
                    Result result_peg_497(result_peg_465.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_497.getPosition()))){
                                result_peg_497.nextPosition();
                            } else {
                                goto out_peg_500;
                            }
                        }
                        result_peg_497.setValue((void*) " ");
                            
                    }
                    goto success_peg_498;
                    out_peg_500:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_497.getPosition()))){
                                result_peg_497.nextPosition();
                            } else {
                                goto out_peg_502;
                            }
                        }
                        result_peg_497.setValue((void*) "\t");
                            
                    }
                    goto success_peg_498;
                    out_peg_502:
                    goto loop_peg_496;
                    success_peg_498:
                    ;
                    result_peg_465.addResult(result_peg_497);
                } while (true);
                loop_peg_496:
                ;
                        
            }
            goto success_peg_487;
            goto out_peg_467;
            success_peg_487:
            ;
        
        
        
        result_peg_465 = rule_integer(stream, result_peg_465.getPosition());
            if (result_peg_465.error()){
                goto out_peg_467;
            }
            index = result_peg_465.getValues();
        
        
        
        {
                
                result_peg_465.reset();
                do{
                    Result result_peg_515(result_peg_465.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_515.getPosition()))){
                                result_peg_515.nextPosition();
                            } else {
                                goto out_peg_518;
                            }
                        }
                        result_peg_515.setValue((void*) " ");
                            
                    }
                    goto success_peg_516;
                    out_peg_518:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_515.getPosition()))){
                                result_peg_515.nextPosition();
                            } else {
                                goto out_peg_520;
                            }
                        }
                        result_peg_515.setValue((void*) "\t");
                            
                    }
                    goto success_peg_516;
                    out_peg_520:
                    goto loop_peg_514;
                    success_peg_516:
                    ;
                    result_peg_465.addResult(result_peg_515);
                } while (true);
                loop_peg_514:
                ;
                        
            }
            goto success_peg_505;
            goto out_peg_467;
            success_peg_505:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_465.getPosition()))){
                    result_peg_465.nextPosition();
                } else {
                    goto out_peg_467;
                }
            }
            result_peg_465.setValue((void*) ")");
        
        
        
        {
                
                result_peg_465.reset();
                do{
                    Result result_peg_533(result_peg_465.getPosition());
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
                    result_peg_465.addResult(result_peg_533);
                } while (true);
                loop_peg_532:
                ;
                        
            }
            goto success_peg_523;
            goto out_peg_467;
            success_peg_523:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_465.getPosition()))){
                    result_peg_465.nextPosition();
                } else {
                    goto out_peg_467;
                }
            }
            result_peg_465.setValue((void*) "=");
        
        
        
        {
                
                result_peg_465.reset();
                do{
                    Result result_peg_551(result_peg_465.getPosition());
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
                    result_peg_465.addResult(result_peg_551);
                } while (true);
                loop_peg_550:
                ;
                        
            }
            goto success_peg_541;
            goto out_peg_467;
            success_peg_541:
            ;
        
        
        
        result_peg_465 = rule_expr(stream, result_peg_465.getPosition());
            if (result_peg_465.error()){
                goto out_peg_467;
            }
            exp = result_peg_465.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(name, index, exp);
                result_peg_465.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_465;
    stream.update(result_peg_465.getPosition());
    
    
    return result_peg_465;
    out_peg_467:
    Result result_peg_558(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareChar("ctrl"[i], stream.get(result_peg_558.getPosition()))){
                    result_peg_558.nextPosition();
                } else {
                    goto out_peg_560;
                }
            }
            result_peg_558.setValue((void*) "ctrl");
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(makeSimpleIdentifier("ctrl"), makeNumber(1));
                result_peg_558.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_558;
    stream.update(result_peg_558.getPosition());
    
    
    return result_peg_558;
    out_peg_560:
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_562 = stream.getColumn(position);
    if (column_peg_562.chunk2 != 0 && column_peg_562.chunk2->chunk_identifier.calculated()){
        return column_peg_562.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_572(stream, "identifier");
    int myposition = position;
    
    Value line;
    Value first;
    Value rest;
    Result result_peg_563(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_565 = stream.getLineInfo(result_peg_563.getPosition());
            line = &line_info_peg_565;
        
        
        
        result_peg_563 = rule_name(stream, result_peg_563.getPosition());
            if (result_peg_563.error()){
                goto out_peg_567;
            }
            first = result_peg_563.getValues();
        
        
        
        result_peg_563.reset();
            do{
                Result result_peg_570(result_peg_563.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_570.getPosition()))){
                                result_peg_570.nextPosition();
                            } else {
                                goto loop_peg_569;
                            }
                        }
                        result_peg_570.setValue((void*) ".");
                    
                    
                    
                    result_peg_570 = rule_name(stream, result_peg_570.getPosition());
                        if (result_peg_570.error()){
                            goto loop_peg_569;
                        }
                    
                    
                }
                result_peg_563.addResult(result_peg_570);
            } while (true);
            loop_peg_569:
            ;
            rest = result_peg_563.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                result_peg_563.setValue(value);
            }
        
        
    }
    
    if (column_peg_562.chunk2 == 0){
        column_peg_562.chunk2 = new Chunk2();
    }
    column_peg_562.chunk2->chunk_identifier = result_peg_563;
    stream.update(result_peg_563.getPosition());
    
    
    return result_peg_563;
    out_peg_567:
    
    if (column_peg_562.chunk2 == 0){
        column_peg_562.chunk2 = new Chunk2();
    }
    column_peg_562.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_573 = stream.getColumn(position);
    if (column_peg_573.chunk2 != 0 && column_peg_573.chunk2->chunk_integer.calculated()){
        return column_peg_573.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_591(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_574(myposition);
    
    {
    
        int save_peg_576 = result_peg_574.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_574.getPosition()))){
                        result_peg_574.nextPosition();
                    } else {
                        goto out_peg_579;
                    }
                }
                result_peg_574.setValue((void*) "-");
                    
            }
            goto success_peg_577;
            out_peg_579:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_574.getPosition()))){
                        result_peg_574.nextPosition();
                    } else {
                        goto out_peg_581;
                    }
                }
                result_peg_574.setValue((void*) "+");
                    
            }
            goto success_peg_577;
            out_peg_581:
            
            result_peg_574 = Result(save_peg_576);
            result_peg_574.setValue((void*) 0);
            
            success_peg_577:
            ;
        
        Result result_peg_575 = result_peg_574;
        
        result_peg_574.reset();
            do{
                Result result_peg_584(result_peg_574.getPosition());
                {
                    
                    char letter_peg_589 = stream.get(result_peg_584.getPosition());
                    if (letter_peg_589 != '\0' && strchr("0123456789", letter_peg_589) != NULL){
                        result_peg_584.nextPosition();
                        result_peg_584.setValue((void*) (long) letter_peg_589);
                    } else {
                        goto out_peg_588;
                    }
                    
                }
                goto success_peg_585;
                out_peg_588:
                goto loop_peg_583;
                success_peg_585:
                ;
                result_peg_574.addResult(result_peg_584);
            } while (true);
            loop_peg_583:
            if (result_peg_574.matches() == 0){
                goto out_peg_590;
            }
        
        Result result_peg_582 = result_peg_574;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_575.getValues(), result_peg_582.getValues());
                result_peg_574.setValue(value);
            }
        
        
    }
    
    if (column_peg_573.chunk2 == 0){
        column_peg_573.chunk2 = new Chunk2();
    }
    column_peg_573.chunk2->chunk_integer = result_peg_574;
    stream.update(result_peg_574.getPosition());
    
    
    return result_peg_574;
    out_peg_590:
    
    if (column_peg_573.chunk2 == 0){
        column_peg_573.chunk2 = new Chunk2();
    }
    column_peg_573.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_592 = stream.getColumn(position);
    if (column_peg_592.chunk2 != 0 && column_peg_592.chunk2->chunk_float.calculated()){
        return column_peg_592.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_637(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
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
            ;
            left = result_peg_593.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_593.getPosition()))){
                    result_peg_593.nextPosition();
                } else {
                    goto out_peg_610;
                }
            }
            result_peg_593.setValue((void*) ".");
        
        
        
        result_peg_593.reset();
            do{
                Result result_peg_613(result_peg_593.getPosition());
                {
                    
                    char letter_peg_618 = stream.get(result_peg_613.getPosition());
                    if (letter_peg_618 != '\0' && strchr("0123456789", letter_peg_618) != NULL){
                        result_peg_613.nextPosition();
                        result_peg_613.setValue((void*) (long) letter_peg_618);
                    } else {
                        goto out_peg_617;
                    }
                    
                }
                goto success_peg_614;
                out_peg_617:
                goto loop_peg_612;
                success_peg_614:
                ;
                result_peg_593.addResult(result_peg_613);
            } while (true);
            loop_peg_612:
            if (result_peg_593.matches() == 0){
                goto out_peg_610;
            }
            right = result_peg_593.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_594.getValues(), parseDouble(left,right));
                result_peg_593.setValue(value);
            }
        
        
    }
    
    if (column_peg_592.chunk2 == 0){
        column_peg_592.chunk2 = new Chunk2();
    }
    column_peg_592.chunk2->chunk_float = result_peg_593;
    stream.update(result_peg_593.getPosition());
    
    
    return result_peg_593;
    out_peg_610:
    Result result_peg_619(myposition);
    
    {
    
        int save_peg_621 = result_peg_619.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_619.getPosition()))){
                        result_peg_619.nextPosition();
                    } else {
                        goto out_peg_624;
                    }
                }
                result_peg_619.setValue((void*) "-");
                    
            }
            goto success_peg_622;
            out_peg_624:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_619.getPosition()))){
                        result_peg_619.nextPosition();
                    } else {
                        goto out_peg_626;
                    }
                }
                result_peg_619.setValue((void*) "+");
                    
            }
            goto success_peg_622;
            out_peg_626:
            
            result_peg_619 = Result(save_peg_621);
            result_peg_619.setValue((void*) 0);
            
            success_peg_622:
            ;
        
        Result result_peg_620 = result_peg_619;
        
        result_peg_619.reset();
            do{
                Result result_peg_629(result_peg_619.getPosition());
                {
                    
                    char letter_peg_634 = stream.get(result_peg_629.getPosition());
                    if (letter_peg_634 != '\0' && strchr("0123456789", letter_peg_634) != NULL){
                        result_peg_629.nextPosition();
                        result_peg_629.setValue((void*) (long) letter_peg_634);
                    } else {
                        goto out_peg_633;
                    }
                    
                }
                goto success_peg_630;
                out_peg_633:
                goto loop_peg_628;
                success_peg_630:
                ;
                result_peg_619.addResult(result_peg_629);
            } while (true);
            loop_peg_628:
            if (result_peg_619.matches() == 0){
                goto out_peg_635;
            }
            left = result_peg_619.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_619.getPosition()))){
                    result_peg_619.nextPosition();
                } else {
                    goto out_peg_635;
                }
            }
            result_peg_619.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_620.getValues(), parseDouble(left));
                result_peg_619.setValue(value);
            }
        
        
    }
    
    if (column_peg_592.chunk2 == 0){
        column_peg_592.chunk2 = new Chunk2();
    }
    column_peg_592.chunk2->chunk_float = result_peg_619;
    stream.update(result_peg_619.getPosition());
    
    
    return result_peg_619;
    out_peg_635:
    
    if (column_peg_592.chunk2 == 0){
        column_peg_592.chunk2 = new Chunk2();
    }
    column_peg_592.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_638 = stream.getColumn(position);
    if (column_peg_638.chunk2 != 0 && column_peg_638.chunk2->chunk_string.calculated()){
        return column_peg_638.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_653(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_639(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_639.getPosition()))){
                    result_peg_639.nextPosition();
                } else {
                    goto out_peg_641;
                }
            }
            result_peg_639.setValue((void*) "\"");
        
        
        
        result_peg_639.reset();
            do{
                Result result_peg_644(result_peg_639.getPosition());
                {
                
                    Result result_peg_647(result_peg_644);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_647.getPosition()))){
                                result_peg_647.nextPosition();
                            } else {
                                goto not_peg_646;
                            }
                        }
                        result_peg_647.setValue((void*) "\"");
                        goto loop_peg_643;
                        not_peg_646:
                        result_peg_644.setValue((void*)0);
                    
                    
                    
                    Result result_peg_650(result_peg_644);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_650.getPosition()))){
                                result_peg_650.nextPosition();
                            } else {
                                goto not_peg_649;
                            }
                        }
                        result_peg_650.setValue((void*) "\n");
                        goto loop_peg_643;
                        not_peg_649:
                        result_peg_644.setValue((void*)0);
                    
                    
                    
                    char temp_peg_651 = stream.get(result_peg_644.getPosition());
                        if (temp_peg_651 != '\0'){
                            result_peg_644.setValue((void*) (long) temp_peg_651);
                            result_peg_644.nextPosition();
                        } else {
                            goto loop_peg_643;
                        }
                    
                    
                }
                result_peg_639.addResult(result_peg_644);
            } while (true);
            loop_peg_643:
            ;
            contents = result_peg_639.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_639.getPosition()))){
                    result_peg_639.nextPosition();
                } else {
                    goto out_peg_641;
                }
            }
            result_peg_639.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_639.setValue(value);
            }
        
        
    }
    
    if (column_peg_638.chunk2 == 0){
        column_peg_638.chunk2 = new Chunk2();
    }
    column_peg_638.chunk2->chunk_string = result_peg_639;
    stream.update(result_peg_639.getPosition());
    
    
    return result_peg_639;
    out_peg_641:
    
    if (column_peg_638.chunk2 == 0){
        column_peg_638.chunk2 = new Chunk2();
    }
    column_peg_638.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_654 = stream.getColumn(position);
    if (column_peg_654.chunk2 != 0 && column_peg_654.chunk2->chunk_range.calculated()){
        return column_peg_654.chunk2->chunk_range;
    }
    
    RuleTrace trace_peg_955(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_655(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_655.getPosition()))){
                    result_peg_655.nextPosition();
                } else {
                    goto out_peg_657;
                }
            }
            result_peg_655.setValue((void*) "[");
        
        
        
        {
                
                result_peg_655.reset();
                do{
                    Result result_peg_669(result_peg_655.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_669.getPosition()))){
                                result_peg_669.nextPosition();
                            } else {
                                goto out_peg_672;
                            }
                        }
                        result_peg_669.setValue((void*) " ");
                            
                    }
                    goto success_peg_670;
                    out_peg_672:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_669.getPosition()))){
                                result_peg_669.nextPosition();
                            } else {
                                goto out_peg_674;
                            }
                        }
                        result_peg_669.setValue((void*) "\t");
                            
                    }
                    goto success_peg_670;
                    out_peg_674:
                    goto loop_peg_668;
                    success_peg_670:
                    ;
                    result_peg_655.addResult(result_peg_669);
                } while (true);
                loop_peg_668:
                ;
                        
            }
            goto success_peg_659;
            goto out_peg_657;
            success_peg_659:
            ;
        
        
        
        result_peg_655 = rule_expr_c(stream, result_peg_655.getPosition());
            if (result_peg_655.error()){
                goto out_peg_657;
            }
            low = result_peg_655.getValues();
        
        
        
        {
                
                result_peg_655.reset();
                do{
                    Result result_peg_687(result_peg_655.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_687.getPosition()))){
                                result_peg_687.nextPosition();
                            } else {
                                goto out_peg_690;
                            }
                        }
                        result_peg_687.setValue((void*) " ");
                            
                    }
                    goto success_peg_688;
                    out_peg_690:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_687.getPosition()))){
                                result_peg_687.nextPosition();
                            } else {
                                goto out_peg_692;
                            }
                        }
                        result_peg_687.setValue((void*) "\t");
                            
                    }
                    goto success_peg_688;
                    out_peg_692:
                    goto loop_peg_686;
                    success_peg_688:
                    ;
                    result_peg_655.addResult(result_peg_687);
                } while (true);
                loop_peg_686:
                ;
                        
            }
            goto success_peg_677;
            goto out_peg_657;
            success_peg_677:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_655.getPosition()))){
                    result_peg_655.nextPosition();
                } else {
                    goto out_peg_657;
                }
            }
            result_peg_655.setValue((void*) ",");
        
        
        
        {
                
                result_peg_655.reset();
                do{
                    Result result_peg_705(result_peg_655.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_705.getPosition()))){
                                result_peg_705.nextPosition();
                            } else {
                                goto out_peg_708;
                            }
                        }
                        result_peg_705.setValue((void*) " ");
                            
                    }
                    goto success_peg_706;
                    out_peg_708:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_705.getPosition()))){
                                result_peg_705.nextPosition();
                            } else {
                                goto out_peg_710;
                            }
                        }
                        result_peg_705.setValue((void*) "\t");
                            
                    }
                    goto success_peg_706;
                    out_peg_710:
                    goto loop_peg_704;
                    success_peg_706:
                    ;
                    result_peg_655.addResult(result_peg_705);
                } while (true);
                loop_peg_704:
                ;
                        
            }
            goto success_peg_695;
            goto out_peg_657;
            success_peg_695:
            ;
        
        
        
        result_peg_655 = rule_expr_c(stream, result_peg_655.getPosition());
            if (result_peg_655.error()){
                goto out_peg_657;
            }
            high = result_peg_655.getValues();
        
        
        
        {
                
                result_peg_655.reset();
                do{
                    Result result_peg_723(result_peg_655.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_723.getPosition()))){
                                result_peg_723.nextPosition();
                            } else {
                                goto out_peg_726;
                            }
                        }
                        result_peg_723.setValue((void*) " ");
                            
                    }
                    goto success_peg_724;
                    out_peg_726:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_723.getPosition()))){
                                result_peg_723.nextPosition();
                            } else {
                                goto out_peg_728;
                            }
                        }
                        result_peg_723.setValue((void*) "\t");
                            
                    }
                    goto success_peg_724;
                    out_peg_728:
                    goto loop_peg_722;
                    success_peg_724:
                    ;
                    result_peg_655.addResult(result_peg_723);
                } while (true);
                loop_peg_722:
                ;
                        
            }
            goto success_peg_713;
            goto out_peg_657;
            success_peg_713:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_655.getPosition()))){
                    result_peg_655.nextPosition();
                } else {
                    goto out_peg_657;
                }
            }
            result_peg_655.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_655.setValue(value);
            }
        
        
    }
    
    if (column_peg_654.chunk2 == 0){
        column_peg_654.chunk2 = new Chunk2();
    }
    column_peg_654.chunk2->chunk_range = result_peg_655;
    stream.update(result_peg_655.getPosition());
    
    
    return result_peg_655;
    out_peg_657:
    Result result_peg_730(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_730.getPosition()))){
                    result_peg_730.nextPosition();
                } else {
                    goto out_peg_732;
                }
            }
            result_peg_730.setValue((void*) "(");
        
        
        
        {
                
                result_peg_730.reset();
                do{
                    Result result_peg_744(result_peg_730.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_744.getPosition()))){
                                result_peg_744.nextPosition();
                            } else {
                                goto out_peg_747;
                            }
                        }
                        result_peg_744.setValue((void*) " ");
                            
                    }
                    goto success_peg_745;
                    out_peg_747:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_744.getPosition()))){
                                result_peg_744.nextPosition();
                            } else {
                                goto out_peg_749;
                            }
                        }
                        result_peg_744.setValue((void*) "\t");
                            
                    }
                    goto success_peg_745;
                    out_peg_749:
                    goto loop_peg_743;
                    success_peg_745:
                    ;
                    result_peg_730.addResult(result_peg_744);
                } while (true);
                loop_peg_743:
                ;
                        
            }
            goto success_peg_734;
            goto out_peg_732;
            success_peg_734:
            ;
        
        
        
        result_peg_730 = rule_expr_c(stream, result_peg_730.getPosition());
            if (result_peg_730.error()){
                goto out_peg_732;
            }
            low = result_peg_730.getValues();
        
        
        
        {
                
                result_peg_730.reset();
                do{
                    Result result_peg_762(result_peg_730.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_762.getPosition()))){
                                result_peg_762.nextPosition();
                            } else {
                                goto out_peg_765;
                            }
                        }
                        result_peg_762.setValue((void*) " ");
                            
                    }
                    goto success_peg_763;
                    out_peg_765:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_762.getPosition()))){
                                result_peg_762.nextPosition();
                            } else {
                                goto out_peg_767;
                            }
                        }
                        result_peg_762.setValue((void*) "\t");
                            
                    }
                    goto success_peg_763;
                    out_peg_767:
                    goto loop_peg_761;
                    success_peg_763:
                    ;
                    result_peg_730.addResult(result_peg_762);
                } while (true);
                loop_peg_761:
                ;
                        
            }
            goto success_peg_752;
            goto out_peg_732;
            success_peg_752:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_730.getPosition()))){
                    result_peg_730.nextPosition();
                } else {
                    goto out_peg_732;
                }
            }
            result_peg_730.setValue((void*) ",");
        
        
        
        {
                
                result_peg_730.reset();
                do{
                    Result result_peg_780(result_peg_730.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_780.getPosition()))){
                                result_peg_780.nextPosition();
                            } else {
                                goto out_peg_783;
                            }
                        }
                        result_peg_780.setValue((void*) " ");
                            
                    }
                    goto success_peg_781;
                    out_peg_783:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_780.getPosition()))){
                                result_peg_780.nextPosition();
                            } else {
                                goto out_peg_785;
                            }
                        }
                        result_peg_780.setValue((void*) "\t");
                            
                    }
                    goto success_peg_781;
                    out_peg_785:
                    goto loop_peg_779;
                    success_peg_781:
                    ;
                    result_peg_730.addResult(result_peg_780);
                } while (true);
                loop_peg_779:
                ;
                        
            }
            goto success_peg_770;
            goto out_peg_732;
            success_peg_770:
            ;
        
        
        
        result_peg_730 = rule_expr_c(stream, result_peg_730.getPosition());
            if (result_peg_730.error()){
                goto out_peg_732;
            }
            high = result_peg_730.getValues();
        
        
        
        {
                
                result_peg_730.reset();
                do{
                    Result result_peg_798(result_peg_730.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_798.getPosition()))){
                                result_peg_798.nextPosition();
                            } else {
                                goto out_peg_801;
                            }
                        }
                        result_peg_798.setValue((void*) " ");
                            
                    }
                    goto success_peg_799;
                    out_peg_801:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_798.getPosition()))){
                                result_peg_798.nextPosition();
                            } else {
                                goto out_peg_803;
                            }
                        }
                        result_peg_798.setValue((void*) "\t");
                            
                    }
                    goto success_peg_799;
                    out_peg_803:
                    goto loop_peg_797;
                    success_peg_799:
                    ;
                    result_peg_730.addResult(result_peg_798);
                } while (true);
                loop_peg_797:
                ;
                        
            }
            goto success_peg_788;
            goto out_peg_732;
            success_peg_788:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_730.getPosition()))){
                    result_peg_730.nextPosition();
                } else {
                    goto out_peg_732;
                }
            }
            result_peg_730.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_730.setValue(value);
            }
        
        
    }
    
    if (column_peg_654.chunk2 == 0){
        column_peg_654.chunk2 = new Chunk2();
    }
    column_peg_654.chunk2->chunk_range = result_peg_730;
    stream.update(result_peg_730.getPosition());
    
    
    return result_peg_730;
    out_peg_732:
    Result result_peg_805(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_805.getPosition()))){
                    result_peg_805.nextPosition();
                } else {
                    goto out_peg_807;
                }
            }
            result_peg_805.setValue((void*) "(");
        
        
        
        {
                
                result_peg_805.reset();
                do{
                    Result result_peg_819(result_peg_805.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_819.getPosition()))){
                                result_peg_819.nextPosition();
                            } else {
                                goto out_peg_822;
                            }
                        }
                        result_peg_819.setValue((void*) " ");
                            
                    }
                    goto success_peg_820;
                    out_peg_822:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_819.getPosition()))){
                                result_peg_819.nextPosition();
                            } else {
                                goto out_peg_824;
                            }
                        }
                        result_peg_819.setValue((void*) "\t");
                            
                    }
                    goto success_peg_820;
                    out_peg_824:
                    goto loop_peg_818;
                    success_peg_820:
                    ;
                    result_peg_805.addResult(result_peg_819);
                } while (true);
                loop_peg_818:
                ;
                        
            }
            goto success_peg_809;
            goto out_peg_807;
            success_peg_809:
            ;
        
        
        
        result_peg_805 = rule_expr_c(stream, result_peg_805.getPosition());
            if (result_peg_805.error()){
                goto out_peg_807;
            }
            low = result_peg_805.getValues();
        
        
        
        {
                
                result_peg_805.reset();
                do{
                    Result result_peg_837(result_peg_805.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_837.getPosition()))){
                                result_peg_837.nextPosition();
                            } else {
                                goto out_peg_840;
                            }
                        }
                        result_peg_837.setValue((void*) " ");
                            
                    }
                    goto success_peg_838;
                    out_peg_840:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_837.getPosition()))){
                                result_peg_837.nextPosition();
                            } else {
                                goto out_peg_842;
                            }
                        }
                        result_peg_837.setValue((void*) "\t");
                            
                    }
                    goto success_peg_838;
                    out_peg_842:
                    goto loop_peg_836;
                    success_peg_838:
                    ;
                    result_peg_805.addResult(result_peg_837);
                } while (true);
                loop_peg_836:
                ;
                        
            }
            goto success_peg_827;
            goto out_peg_807;
            success_peg_827:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_805.getPosition()))){
                    result_peg_805.nextPosition();
                } else {
                    goto out_peg_807;
                }
            }
            result_peg_805.setValue((void*) ",");
        
        
        
        {
                
                result_peg_805.reset();
                do{
                    Result result_peg_855(result_peg_805.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_855.getPosition()))){
                                result_peg_855.nextPosition();
                            } else {
                                goto out_peg_858;
                            }
                        }
                        result_peg_855.setValue((void*) " ");
                            
                    }
                    goto success_peg_856;
                    out_peg_858:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_855.getPosition()))){
                                result_peg_855.nextPosition();
                            } else {
                                goto out_peg_860;
                            }
                        }
                        result_peg_855.setValue((void*) "\t");
                            
                    }
                    goto success_peg_856;
                    out_peg_860:
                    goto loop_peg_854;
                    success_peg_856:
                    ;
                    result_peg_805.addResult(result_peg_855);
                } while (true);
                loop_peg_854:
                ;
                        
            }
            goto success_peg_845;
            goto out_peg_807;
            success_peg_845:
            ;
        
        
        
        result_peg_805 = rule_expr_c(stream, result_peg_805.getPosition());
            if (result_peg_805.error()){
                goto out_peg_807;
            }
            high = result_peg_805.getValues();
        
        
        
        {
                
                result_peg_805.reset();
                do{
                    Result result_peg_873(result_peg_805.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_873.getPosition()))){
                                result_peg_873.nextPosition();
                            } else {
                                goto out_peg_876;
                            }
                        }
                        result_peg_873.setValue((void*) " ");
                            
                    }
                    goto success_peg_874;
                    out_peg_876:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_873.getPosition()))){
                                result_peg_873.nextPosition();
                            } else {
                                goto out_peg_878;
                            }
                        }
                        result_peg_873.setValue((void*) "\t");
                            
                    }
                    goto success_peg_874;
                    out_peg_878:
                    goto loop_peg_872;
                    success_peg_874:
                    ;
                    result_peg_805.addResult(result_peg_873);
                } while (true);
                loop_peg_872:
                ;
                        
            }
            goto success_peg_863;
            goto out_peg_807;
            success_peg_863:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_805.getPosition()))){
                    result_peg_805.nextPosition();
                } else {
                    goto out_peg_807;
                }
            }
            result_peg_805.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_805.setValue(value);
            }
        
        
    }
    
    if (column_peg_654.chunk2 == 0){
        column_peg_654.chunk2 = new Chunk2();
    }
    column_peg_654.chunk2->chunk_range = result_peg_805;
    stream.update(result_peg_805.getPosition());
    
    
    return result_peg_805;
    out_peg_807:
    Result result_peg_880(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_880.getPosition()))){
                    result_peg_880.nextPosition();
                } else {
                    goto out_peg_882;
                }
            }
            result_peg_880.setValue((void*) "[");
        
        
        
        {
                
                result_peg_880.reset();
                do{
                    Result result_peg_894(result_peg_880.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_894.getPosition()))){
                                result_peg_894.nextPosition();
                            } else {
                                goto out_peg_897;
                            }
                        }
                        result_peg_894.setValue((void*) " ");
                            
                    }
                    goto success_peg_895;
                    out_peg_897:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_894.getPosition()))){
                                result_peg_894.nextPosition();
                            } else {
                                goto out_peg_899;
                            }
                        }
                        result_peg_894.setValue((void*) "\t");
                            
                    }
                    goto success_peg_895;
                    out_peg_899:
                    goto loop_peg_893;
                    success_peg_895:
                    ;
                    result_peg_880.addResult(result_peg_894);
                } while (true);
                loop_peg_893:
                ;
                        
            }
            goto success_peg_884;
            goto out_peg_882;
            success_peg_884:
            ;
        
        
        
        result_peg_880 = rule_expr_c(stream, result_peg_880.getPosition());
            if (result_peg_880.error()){
                goto out_peg_882;
            }
            low = result_peg_880.getValues();
        
        
        
        {
                
                result_peg_880.reset();
                do{
                    Result result_peg_912(result_peg_880.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_912.getPosition()))){
                                result_peg_912.nextPosition();
                            } else {
                                goto out_peg_915;
                            }
                        }
                        result_peg_912.setValue((void*) " ");
                            
                    }
                    goto success_peg_913;
                    out_peg_915:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_912.getPosition()))){
                                result_peg_912.nextPosition();
                            } else {
                                goto out_peg_917;
                            }
                        }
                        result_peg_912.setValue((void*) "\t");
                            
                    }
                    goto success_peg_913;
                    out_peg_917:
                    goto loop_peg_911;
                    success_peg_913:
                    ;
                    result_peg_880.addResult(result_peg_912);
                } while (true);
                loop_peg_911:
                ;
                        
            }
            goto success_peg_902;
            goto out_peg_882;
            success_peg_902:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_880.getPosition()))){
                    result_peg_880.nextPosition();
                } else {
                    goto out_peg_882;
                }
            }
            result_peg_880.setValue((void*) ",");
        
        
        
        {
                
                result_peg_880.reset();
                do{
                    Result result_peg_930(result_peg_880.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_930.getPosition()))){
                                result_peg_930.nextPosition();
                            } else {
                                goto out_peg_933;
                            }
                        }
                        result_peg_930.setValue((void*) " ");
                            
                    }
                    goto success_peg_931;
                    out_peg_933:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_930.getPosition()))){
                                result_peg_930.nextPosition();
                            } else {
                                goto out_peg_935;
                            }
                        }
                        result_peg_930.setValue((void*) "\t");
                            
                    }
                    goto success_peg_931;
                    out_peg_935:
                    goto loop_peg_929;
                    success_peg_931:
                    ;
                    result_peg_880.addResult(result_peg_930);
                } while (true);
                loop_peg_929:
                ;
                        
            }
            goto success_peg_920;
            goto out_peg_882;
            success_peg_920:
            ;
        
        
        
        result_peg_880 = rule_expr_c(stream, result_peg_880.getPosition());
            if (result_peg_880.error()){
                goto out_peg_882;
            }
            high = result_peg_880.getValues();
        
        
        
        {
                
                result_peg_880.reset();
                do{
                    Result result_peg_948(result_peg_880.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_948.getPosition()))){
                                result_peg_948.nextPosition();
                            } else {
                                goto out_peg_951;
                            }
                        }
                        result_peg_948.setValue((void*) " ");
                            
                    }
                    goto success_peg_949;
                    out_peg_951:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_948.getPosition()))){
                                result_peg_948.nextPosition();
                            } else {
                                goto out_peg_953;
                            }
                        }
                        result_peg_948.setValue((void*) "\t");
                            
                    }
                    goto success_peg_949;
                    out_peg_953:
                    goto loop_peg_947;
                    success_peg_949:
                    ;
                    result_peg_880.addResult(result_peg_948);
                } while (true);
                loop_peg_947:
                ;
                        
            }
            goto success_peg_938;
            goto out_peg_882;
            success_peg_938:
            ;
        
        
        
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
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_880.setValue(value);
            }
        
        
    }
    
    if (column_peg_654.chunk2 == 0){
        column_peg_654.chunk2 = new Chunk2();
    }
    column_peg_654.chunk2->chunk_range = result_peg_880;
    stream.update(result_peg_880.getPosition());
    
    
    return result_peg_880;
    out_peg_882:
    
    if (column_peg_654.chunk2 == 0){
        column_peg_654.chunk2 = new Chunk2();
    }
    column_peg_654.chunk2->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_956 = stream.getColumn(position);
    if (column_peg_956.chunk3 != 0 && column_peg_956.chunk3->chunk_name.calculated()){
        return column_peg_956.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_968(stream, "name");
    int myposition = position;
    
    
    Result result_peg_957(myposition);
    
    {
    
        {
                
                char letter_peg_963 = stream.get(result_peg_957.getPosition());
                if (letter_peg_963 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_963) != NULL){
                    result_peg_957.nextPosition();
                    result_peg_957.setValue((void*) (long) letter_peg_963);
                } else {
                    goto out_peg_962;
                }
                
            }
            goto success_peg_959;
            out_peg_962:
            goto out_peg_964;
            success_peg_959:
            ;
        
        Result result_peg_958 = result_peg_957;
        
        result_peg_957.reset();
            do{
                Result result_peg_967(result_peg_957.getPosition());
                result_peg_967 = rule_alpha_digit(stream, result_peg_967.getPosition());
                if (result_peg_967.error()){
                    goto loop_peg_966;
                }
                result_peg_957.addResult(result_peg_967);
            } while (true);
            loop_peg_966:
            ;
        
        Result result_peg_965 = result_peg_957;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_958.getValues().getValue(),result_peg_965.getValues());
                result_peg_957.setValue(value);
            }
        
        
    }
    
    if (column_peg_956.chunk3 == 0){
        column_peg_956.chunk3 = new Chunk3();
    }
    column_peg_956.chunk3->chunk_name = result_peg_957;
    stream.update(result_peg_957.getPosition());
    
    
    return result_peg_957;
    out_peg_964:
    
    if (column_peg_956.chunk3 == 0){
        column_peg_956.chunk3 = new Chunk3();
    }
    column_peg_956.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_969 = stream.getColumn(position);
    if (column_peg_969.chunk3 != 0 && column_peg_969.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_969.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_984(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_970(myposition);
    
    {
        
        char letter_peg_975 = stream.get(result_peg_970.getPosition());
        if (letter_peg_975 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_975) != NULL){
            result_peg_970.nextPosition();
            result_peg_970.setValue((void*) (long) letter_peg_975);
        } else {
            goto out_peg_974;
        }
        
    }
    goto success_peg_971;
    out_peg_974:
    goto out_peg_976;
    success_peg_971:
    ;
    
    if (column_peg_969.chunk3 == 0){
        column_peg_969.chunk3 = new Chunk3();
    }
    column_peg_969.chunk3->chunk_alpha_digit = result_peg_970;
    stream.update(result_peg_970.getPosition());
    
    
    return result_peg_970;
    out_peg_976:
    Result result_peg_977(myposition);
    
    {
        
        char letter_peg_982 = stream.get(result_peg_977.getPosition());
        if (letter_peg_982 != '\0' && strchr("0123456789", letter_peg_982) != NULL){
            result_peg_977.nextPosition();
            result_peg_977.setValue((void*) (long) letter_peg_982);
        } else {
            goto out_peg_981;
        }
        
    }
    goto success_peg_978;
    out_peg_981:
    goto out_peg_983;
    success_peg_978:
    ;
    
    if (column_peg_969.chunk3 == 0){
        column_peg_969.chunk3 = new Chunk3();
    }
    column_peg_969.chunk3->chunk_alpha_digit = result_peg_977;
    stream.update(result_peg_977.getPosition());
    
    
    return result_peg_977;
    out_peg_983:
    
    if (column_peg_969.chunk3 == 0){
        column_peg_969.chunk3 = new Chunk3();
    }
    column_peg_969.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_985 = stream.getColumn(position);
    if (column_peg_985.chunk3 != 0 && column_peg_985.chunk3->chunk_valuelist.calculated()){
        return column_peg_985.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1103(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_986(myposition);
    
    {
    
        result_peg_986 = rule_expr_c(stream, result_peg_986.getPosition());
            if (result_peg_986.error()){
                goto out_peg_988;
            }
            first = result_peg_986.getValues();
        
        
        
        result_peg_986.reset();
            do{
                Result result_peg_991(result_peg_986.getPosition());
                {
                
                    {
                            
                            result_peg_991.reset();
                            do{
                                Result result_peg_1003(result_peg_991.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1003.getPosition()))){
                                            result_peg_1003.nextPosition();
                                        } else {
                                            goto out_peg_1006;
                                        }
                                    }
                                    result_peg_1003.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1004;
                                out_peg_1006:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1003.getPosition()))){
                                            result_peg_1003.nextPosition();
                                        } else {
                                            goto out_peg_1008;
                                        }
                                    }
                                    result_peg_1003.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1004;
                                out_peg_1008:
                                goto loop_peg_1002;
                                success_peg_1004:
                                ;
                                result_peg_991.addResult(result_peg_1003);
                            } while (true);
                            loop_peg_1002:
                            ;
                                    
                        }
                        goto success_peg_993;
                        goto loop_peg_990;
                        success_peg_993:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_991.getPosition()))){
                                result_peg_991.nextPosition();
                            } else {
                                goto loop_peg_990;
                            }
                        }
                        result_peg_991.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_991.reset();
                            do{
                                Result result_peg_1021(result_peg_991.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1021.getPosition()))){
                                            result_peg_1021.nextPosition();
                                        } else {
                                            goto out_peg_1024;
                                        }
                                    }
                                    result_peg_1021.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1022;
                                out_peg_1024:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1021.getPosition()))){
                                            result_peg_1021.nextPosition();
                                        } else {
                                            goto out_peg_1026;
                                        }
                                    }
                                    result_peg_1021.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1022;
                                out_peg_1026:
                                goto loop_peg_1020;
                                success_peg_1022:
                                ;
                                result_peg_991.addResult(result_peg_1021);
                            } while (true);
                            loop_peg_1020:
                            ;
                                    
                        }
                        goto success_peg_1011;
                        goto loop_peg_990;
                        success_peg_1011:
                        ;
                    
                    
                    
                    result_peg_991 = rule_expr_c(stream, result_peg_991.getPosition());
                        if (result_peg_991.error()){
                            goto loop_peg_990;
                        }
                    
                    
                }
                result_peg_986.addResult(result_peg_991);
            } while (true);
            loop_peg_990:
            if (result_peg_986.matches() == 0){
                goto out_peg_988;
            }
            rest = result_peg_986.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_986.setValue(value);
            }
        
        
    }
    
    if (column_peg_985.chunk3 == 0){
        column_peg_985.chunk3 = new Chunk3();
    }
    column_peg_985.chunk3->chunk_valuelist = result_peg_986;
    stream.update(result_peg_986.getPosition());
    
    
    return result_peg_986;
    out_peg_988:
    Result result_peg_1027(myposition);
    
    {
    
        {
                
                result_peg_1027.reset();
                do{
                    Result result_peg_1039(result_peg_1027.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1039.getPosition()))){
                                result_peg_1039.nextPosition();
                            } else {
                                goto out_peg_1042;
                            }
                        }
                        result_peg_1039.setValue((void*) " ");
                            
                    }
                    goto success_peg_1040;
                    out_peg_1042:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1039.getPosition()))){
                                result_peg_1039.nextPosition();
                            } else {
                                goto out_peg_1044;
                            }
                        }
                        result_peg_1039.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1040;
                    out_peg_1044:
                    goto loop_peg_1038;
                    success_peg_1040:
                    ;
                    result_peg_1027.addResult(result_peg_1039);
                } while (true);
                loop_peg_1038:
                ;
                        
            }
            goto success_peg_1029;
            goto out_peg_1045;
            success_peg_1029:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_1027.getPosition()))){
                    result_peg_1027.nextPosition();
                } else {
                    goto out_peg_1045;
                }
            }
            result_peg_1027.setValue((void*) ",");
        
        
        
        {
                
                result_peg_1027.reset();
                do{
                    Result result_peg_1058(result_peg_1027.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1058.getPosition()))){
                                result_peg_1058.nextPosition();
                            } else {
                                goto out_peg_1061;
                            }
                        }
                        result_peg_1058.setValue((void*) " ");
                            
                    }
                    goto success_peg_1059;
                    out_peg_1061:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1058.getPosition()))){
                                result_peg_1058.nextPosition();
                            } else {
                                goto out_peg_1063;
                            }
                        }
                        result_peg_1058.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1059;
                    out_peg_1063:
                    goto loop_peg_1057;
                    success_peg_1059:
                    ;
                    result_peg_1027.addResult(result_peg_1058);
                } while (true);
                loop_peg_1057:
                ;
                        
            }
            goto success_peg_1048;
            goto out_peg_1045;
            success_peg_1048:
            ;
        
        
        
        result_peg_1027 = rule_expr_c(stream, result_peg_1027.getPosition());
            if (result_peg_1027.error()){
                goto out_peg_1045;
            }
            first = result_peg_1027.getValues();
        
        
        
        result_peg_1027.reset();
            do{
                Result result_peg_1067(result_peg_1027.getPosition());
                {
                
                    {
                            
                            result_peg_1067.reset();
                            do{
                                Result result_peg_1079(result_peg_1067.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1079.getPosition()))){
                                            result_peg_1079.nextPosition();
                                        } else {
                                            goto out_peg_1082;
                                        }
                                    }
                                    result_peg_1079.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1080;
                                out_peg_1082:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1079.getPosition()))){
                                            result_peg_1079.nextPosition();
                                        } else {
                                            goto out_peg_1084;
                                        }
                                    }
                                    result_peg_1079.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1080;
                                out_peg_1084:
                                goto loop_peg_1078;
                                success_peg_1080:
                                ;
                                result_peg_1067.addResult(result_peg_1079);
                            } while (true);
                            loop_peg_1078:
                            ;
                                    
                        }
                        goto success_peg_1069;
                        goto loop_peg_1066;
                        success_peg_1069:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1067.getPosition()))){
                                result_peg_1067.nextPosition();
                            } else {
                                goto loop_peg_1066;
                            }
                        }
                        result_peg_1067.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_1067.reset();
                            do{
                                Result result_peg_1097(result_peg_1067.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1097.getPosition()))){
                                            result_peg_1097.nextPosition();
                                        } else {
                                            goto out_peg_1100;
                                        }
                                    }
                                    result_peg_1097.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1098;
                                out_peg_1100:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1097.getPosition()))){
                                            result_peg_1097.nextPosition();
                                        } else {
                                            goto out_peg_1102;
                                        }
                                    }
                                    result_peg_1097.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1098;
                                out_peg_1102:
                                goto loop_peg_1096;
                                success_peg_1098:
                                ;
                                result_peg_1067.addResult(result_peg_1097);
                            } while (true);
                            loop_peg_1096:
                            ;
                                    
                        }
                        goto success_peg_1087;
                        goto loop_peg_1066;
                        success_peg_1087:
                        ;
                    
                    
                    
                    result_peg_1067 = rule_expr_c(stream, result_peg_1067.getPosition());
                        if (result_peg_1067.error()){
                            goto loop_peg_1066;
                        }
                    
                    
                }
                result_peg_1027.addResult(result_peg_1067);
            } while (true);
            loop_peg_1066:
            ;
            rest = result_peg_1027.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_1027.setValue(value);
            }
        
        
    }
    
    if (column_peg_985.chunk3 == 0){
        column_peg_985.chunk3 = new Chunk3();
    }
    column_peg_985.chunk3->chunk_valuelist = result_peg_1027;
    stream.update(result_peg_1027.getPosition());
    
    
    return result_peg_1027;
    out_peg_1045:
    
    if (column_peg_985.chunk3 == 0){
        column_peg_985.chunk3 = new Chunk3();
    }
    column_peg_985.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_1104 = stream.getColumn(position);
    if (column_peg_1104.chunk3 != 0 && column_peg_1104.chunk3->chunk_expr.calculated()){
        return column_peg_1104.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_1131(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_1105(myposition);
    
    {
    
        result_peg_1105 = rule_expr_c(stream, result_peg_1105.getPosition());
            if (result_peg_1105.error()){
                goto out_peg_1107;
            }
        
        Result result_peg_1106 = result_peg_1105;
        
        Result result_peg_1110(result_peg_1105);
            {
            
                {
                        
                        result_peg_1110.reset();
                        do{
                            Result result_peg_1122(result_peg_1110.getPosition());
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(" "[i], stream.get(result_peg_1122.getPosition()))){
                                        result_peg_1122.nextPosition();
                                    } else {
                                        goto out_peg_1125;
                                    }
                                }
                                result_peg_1122.setValue((void*) " ");
                                    
                            }
                            goto success_peg_1123;
                            out_peg_1125:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\t"[i], stream.get(result_peg_1122.getPosition()))){
                                        result_peg_1122.nextPosition();
                                    } else {
                                        goto out_peg_1127;
                                    }
                                }
                                result_peg_1122.setValue((void*) "\t");
                                    
                            }
                            goto success_peg_1123;
                            out_peg_1127:
                            goto loop_peg_1121;
                            success_peg_1123:
                            ;
                            result_peg_1110.addResult(result_peg_1122);
                        } while (true);
                        loop_peg_1121:
                        ;
                                
                    }
                    goto success_peg_1112;
                    goto not_peg_1109;
                    success_peg_1112:
                    ;
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_1110.getPosition()))){
                            result_peg_1110.nextPosition();
                        } else {
                            goto not_peg_1109;
                        }
                    }
                    result_peg_1110.setValue((void*) ",");
                
                
            }
            goto out_peg_1107;
            not_peg_1109:
            result_peg_1105.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_1106.getValues();
                result_peg_1105.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk3 == 0){
        column_peg_1104.chunk3 = new Chunk3();
    }
    column_peg_1104.chunk3->chunk_expr = result_peg_1105;
    stream.update(result_peg_1105.getPosition());
    
    
    return result_peg_1105;
    out_peg_1107:
    Result result_peg_1128(myposition);
    
    {
    
        result_peg_1128 = rule_valuelist(stream, result_peg_1128.getPosition());
            if (result_peg_1128.error()){
                goto out_peg_1130;
            }
        
        Result result_peg_1129 = result_peg_1128;
        
        result_peg_1128 = rule_expr2_rest(stream, result_peg_1128.getPosition(), result_peg_1129.getValues());
            if (result_peg_1128.error()){
                goto out_peg_1130;
            }
        
        
    }
    
    if (column_peg_1104.chunk3 == 0){
        column_peg_1104.chunk3 = new Chunk3();
    }
    column_peg_1104.chunk3->chunk_expr = result_peg_1128;
    stream.update(result_peg_1128.getPosition());
    
    
    return result_peg_1128;
    out_peg_1130:
    
    if (column_peg_1104.chunk3 == 0){
        column_peg_1104.chunk3 = new Chunk3();
    }
    column_peg_1104.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_1132 = stream.getColumn(position);
    if (column_peg_1132.chunk3 != 0 && column_peg_1132.chunk3->chunk_expr_c.calculated()){
        return column_peg_1132.chunk3->chunk_expr_c;
    }
    
    RuleTrace trace_peg_1136(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_1133(myposition);
    
    {
    
        result_peg_1133 = rule_expr2(stream, result_peg_1133.getPosition());
            if (result_peg_1133.error()){
                goto out_peg_1135;
            }
            left = result_peg_1133.getValues();
        
        
        
        result_peg_1133 = rule_expr_rest(stream, result_peg_1133.getPosition(), left);
            if (result_peg_1133.error()){
                goto out_peg_1135;
            }
        
        
    }
    
    if (column_peg_1132.chunk3 == 0){
        column_peg_1132.chunk3 = new Chunk3();
    }
    column_peg_1132.chunk3->chunk_expr_c = result_peg_1133;
    stream.update(result_peg_1133.getPosition());
    
    
    return result_peg_1133;
    out_peg_1135:
    
    if (column_peg_1132.chunk3 == 0){
        column_peg_1132.chunk3 = new Chunk3();
    }
    column_peg_1132.chunk3->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_1137 = stream.getColumn(position);
    if (column_peg_1137.chunk4 != 0 && column_peg_1137.chunk4->chunk_expr2.calculated()){
        return column_peg_1137.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_1141(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_1138(myposition);
    
    {
    
        result_peg_1138 = rule_expr3(stream, result_peg_1138.getPosition());
            if (result_peg_1138.error()){
                goto out_peg_1140;
            }
            left = result_peg_1138.getValues();
        
        
        
        result_peg_1138 = rule_expr2_rest(stream, result_peg_1138.getPosition(), left);
            if (result_peg_1138.error()){
                goto out_peg_1140;
            }
        
        
    }
    
    if (column_peg_1137.chunk4 == 0){
        column_peg_1137.chunk4 = new Chunk4();
    }
    column_peg_1137.chunk4->chunk_expr2 = result_peg_1138;
    stream.update(result_peg_1138.getPosition());
    
    
    return result_peg_1138;
    out_peg_1140:
    
    if (column_peg_1137.chunk4 == 0){
        column_peg_1137.chunk4 = new Chunk4();
    }
    column_peg_1137.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_1142 = stream.getColumn(position);
    if (column_peg_1142.chunk4 != 0 && column_peg_1142.chunk4->chunk_expr3.calculated()){
        return column_peg_1142.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_1146(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_1143(myposition);
    
    {
    
        result_peg_1143 = rule_expr4(stream, result_peg_1143.getPosition());
            if (result_peg_1143.error()){
                goto out_peg_1145;
            }
            left = result_peg_1143.getValues();
        
        
        
        result_peg_1143 = rule_expr3_rest(stream, result_peg_1143.getPosition(), left);
            if (result_peg_1143.error()){
                goto out_peg_1145;
            }
        
        
    }
    
    if (column_peg_1142.chunk4 == 0){
        column_peg_1142.chunk4 = new Chunk4();
    }
    column_peg_1142.chunk4->chunk_expr3 = result_peg_1143;
    stream.update(result_peg_1143.getPosition());
    
    
    return result_peg_1143;
    out_peg_1145:
    
    if (column_peg_1142.chunk4 == 0){
        column_peg_1142.chunk4 = new Chunk4();
    }
    column_peg_1142.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_1147 = stream.getColumn(position);
    if (column_peg_1147.chunk4 != 0 && column_peg_1147.chunk4->chunk_expr4.calculated()){
        return column_peg_1147.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_1151(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_1148(myposition);
    
    {
    
        result_peg_1148 = rule_expr5(stream, result_peg_1148.getPosition());
            if (result_peg_1148.error()){
                goto out_peg_1150;
            }
            left = result_peg_1148.getValues();
        
        
        
        result_peg_1148 = rule_expr4_rest(stream, result_peg_1148.getPosition(), left);
            if (result_peg_1148.error()){
                goto out_peg_1150;
            }
        
        
    }
    
    if (column_peg_1147.chunk4 == 0){
        column_peg_1147.chunk4 = new Chunk4();
    }
    column_peg_1147.chunk4->chunk_expr4 = result_peg_1148;
    stream.update(result_peg_1148.getPosition());
    
    
    return result_peg_1148;
    out_peg_1150:
    
    if (column_peg_1147.chunk4 == 0){
        column_peg_1147.chunk4 = new Chunk4();
    }
    column_peg_1147.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_1152 = stream.getColumn(position);
    if (column_peg_1152.chunk4 != 0 && column_peg_1152.chunk4->chunk_expr5.calculated()){
        return column_peg_1152.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_1156(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_1153(myposition);
    
    {
    
        result_peg_1153 = rule_expr6(stream, result_peg_1153.getPosition());
            if (result_peg_1153.error()){
                goto out_peg_1155;
            }
            left = result_peg_1153.getValues();
        
        
        
        result_peg_1153 = rule_expr5_rest(stream, result_peg_1153.getPosition(), left);
            if (result_peg_1153.error()){
                goto out_peg_1155;
            }
        
        
    }
    
    if (column_peg_1152.chunk4 == 0){
        column_peg_1152.chunk4 = new Chunk4();
    }
    column_peg_1152.chunk4->chunk_expr5 = result_peg_1153;
    stream.update(result_peg_1153.getPosition());
    
    
    return result_peg_1153;
    out_peg_1155:
    
    if (column_peg_1152.chunk4 == 0){
        column_peg_1152.chunk4 = new Chunk4();
    }
    column_peg_1152.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_1157 = stream.getColumn(position);
    if (column_peg_1157.chunk4 != 0 && column_peg_1157.chunk4->chunk_expr6.calculated()){
        return column_peg_1157.chunk4->chunk_expr6;
    }
    
    RuleTrace trace_peg_1161(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_1158(myposition);
    
    {
    
        result_peg_1158 = rule_expr7(stream, result_peg_1158.getPosition());
            if (result_peg_1158.error()){
                goto out_peg_1160;
            }
            left = result_peg_1158.getValues();
        
        
        
        result_peg_1158 = rule_expr6_rest(stream, result_peg_1158.getPosition(), left);
            if (result_peg_1158.error()){
                goto out_peg_1160;
            }
        
        
    }
    
    if (column_peg_1157.chunk4 == 0){
        column_peg_1157.chunk4 = new Chunk4();
    }
    column_peg_1157.chunk4->chunk_expr6 = result_peg_1158;
    stream.update(result_peg_1158.getPosition());
    
    
    return result_peg_1158;
    out_peg_1160:
    
    if (column_peg_1157.chunk4 == 0){
        column_peg_1157.chunk4 = new Chunk4();
    }
    column_peg_1157.chunk4->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_1162 = stream.getColumn(position);
    if (column_peg_1162.chunk5 != 0 && column_peg_1162.chunk5->chunk_expr7.calculated()){
        return column_peg_1162.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_1166(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_1163(myposition);
    
    {
    
        result_peg_1163 = rule_expr8(stream, result_peg_1163.getPosition());
            if (result_peg_1163.error()){
                goto out_peg_1165;
            }
            left = result_peg_1163.getValues();
        
        
        
        result_peg_1163 = rule_expr7_rest(stream, result_peg_1163.getPosition(), left);
            if (result_peg_1163.error()){
                goto out_peg_1165;
            }
        
        
    }
    
    if (column_peg_1162.chunk5 == 0){
        column_peg_1162.chunk5 = new Chunk5();
    }
    column_peg_1162.chunk5->chunk_expr7 = result_peg_1163;
    stream.update(result_peg_1163.getPosition());
    
    
    return result_peg_1163;
    out_peg_1165:
    
    if (column_peg_1162.chunk5 == 0){
        column_peg_1162.chunk5 = new Chunk5();
    }
    column_peg_1162.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_1167 = stream.getColumn(position);
    if (column_peg_1167.chunk5 != 0 && column_peg_1167.chunk5->chunk_expr8.calculated()){
        return column_peg_1167.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_1171(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_1168(myposition);
    
    {
    
        result_peg_1168 = rule_expr9(stream, result_peg_1168.getPosition());
            if (result_peg_1168.error()){
                goto out_peg_1170;
            }
            left = result_peg_1168.getValues();
        
        
        
        result_peg_1168 = rule_expr8_rest(stream, result_peg_1168.getPosition(), left);
            if (result_peg_1168.error()){
                goto out_peg_1170;
            }
        
        
    }
    
    if (column_peg_1167.chunk5 == 0){
        column_peg_1167.chunk5 = new Chunk5();
    }
    column_peg_1167.chunk5->chunk_expr8 = result_peg_1168;
    stream.update(result_peg_1168.getPosition());
    
    
    return result_peg_1168;
    out_peg_1170:
    
    if (column_peg_1167.chunk5 == 0){
        column_peg_1167.chunk5 = new Chunk5();
    }
    column_peg_1167.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_1172 = stream.getColumn(position);
    if (column_peg_1172.chunk5 != 0 && column_peg_1172.chunk5->chunk_expr9.calculated()){
        return column_peg_1172.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_1176(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_1173(myposition);
    
    {
    
        result_peg_1173 = rule_expr10(stream, result_peg_1173.getPosition());
            if (result_peg_1173.error()){
                goto out_peg_1175;
            }
            left = result_peg_1173.getValues();
        
        
        
        result_peg_1173 = rule_expr9_rest(stream, result_peg_1173.getPosition(), left);
            if (result_peg_1173.error()){
                goto out_peg_1175;
            }
        
        
    }
    
    if (column_peg_1172.chunk5 == 0){
        column_peg_1172.chunk5 = new Chunk5();
    }
    column_peg_1172.chunk5->chunk_expr9 = result_peg_1173;
    stream.update(result_peg_1173.getPosition());
    
    
    return result_peg_1173;
    out_peg_1175:
    
    if (column_peg_1172.chunk5 == 0){
        column_peg_1172.chunk5 = new Chunk5();
    }
    column_peg_1172.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_1177 = stream.getColumn(position);
    if (column_peg_1177.chunk5 != 0 && column_peg_1177.chunk5->chunk_expr10.calculated()){
        return column_peg_1177.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_1181(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_1178(myposition);
    
    {
    
        result_peg_1178 = rule_expr11(stream, result_peg_1178.getPosition());
            if (result_peg_1178.error()){
                goto out_peg_1180;
            }
            left = result_peg_1178.getValues();
        
        
        
        result_peg_1178 = rule_expr10_rest(stream, result_peg_1178.getPosition(), left);
            if (result_peg_1178.error()){
                goto out_peg_1180;
            }
        
        
    }
    
    if (column_peg_1177.chunk5 == 0){
        column_peg_1177.chunk5 = new Chunk5();
    }
    column_peg_1177.chunk5->chunk_expr10 = result_peg_1178;
    stream.update(result_peg_1178.getPosition());
    
    
    return result_peg_1178;
    out_peg_1180:
    
    if (column_peg_1177.chunk5 == 0){
        column_peg_1177.chunk5 = new Chunk5();
    }
    column_peg_1177.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_1182 = stream.getColumn(position);
    if (column_peg_1182.chunk5 != 0 && column_peg_1182.chunk5->chunk_expr11.calculated()){
        return column_peg_1182.chunk5->chunk_expr11;
    }
    
    RuleTrace trace_peg_1186(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_1183(myposition);
    
    {
    
        result_peg_1183 = rule_expr12(stream, result_peg_1183.getPosition());
            if (result_peg_1183.error()){
                goto out_peg_1185;
            }
            left = result_peg_1183.getValues();
        
        
        
        result_peg_1183 = rule_expr11_rest(stream, result_peg_1183.getPosition(), left);
            if (result_peg_1183.error()){
                goto out_peg_1185;
            }
        
        
    }
    
    if (column_peg_1182.chunk5 == 0){
        column_peg_1182.chunk5 = new Chunk5();
    }
    column_peg_1182.chunk5->chunk_expr11 = result_peg_1183;
    stream.update(result_peg_1183.getPosition());
    
    
    return result_peg_1183;
    out_peg_1185:
    
    if (column_peg_1182.chunk5 == 0){
        column_peg_1182.chunk5 = new Chunk5();
    }
    column_peg_1182.chunk5->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_1187 = stream.getColumn(position);
    if (column_peg_1187.chunk6 != 0 && column_peg_1187.chunk6->chunk_expr12.calculated()){
        return column_peg_1187.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_1191(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_1188(myposition);
    
    {
    
        result_peg_1188 = rule_expr13(stream, result_peg_1188.getPosition());
            if (result_peg_1188.error()){
                goto out_peg_1190;
            }
            left = result_peg_1188.getValues();
        
        
        
        result_peg_1188 = rule_expr12_rest(stream, result_peg_1188.getPosition(), left);
            if (result_peg_1188.error()){
                goto out_peg_1190;
            }
        
        
    }
    
    if (column_peg_1187.chunk6 == 0){
        column_peg_1187.chunk6 = new Chunk6();
    }
    column_peg_1187.chunk6->chunk_expr12 = result_peg_1188;
    stream.update(result_peg_1188.getPosition());
    
    
    return result_peg_1188;
    out_peg_1190:
    
    if (column_peg_1187.chunk6 == 0){
        column_peg_1187.chunk6 = new Chunk6();
    }
    column_peg_1187.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_1192 = stream.getColumn(position);
    if (column_peg_1192.chunk6 != 0 && column_peg_1192.chunk6->chunk_expr13.calculated()){
        return column_peg_1192.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_1216(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_1193(myposition);
    
    {
    
        result_peg_1193.reset();
            do{
                Result result_peg_1196(result_peg_1193.getPosition());
                result_peg_1196 = rule_unary(stream, result_peg_1196.getPosition());
                if (result_peg_1196.error()){
                    goto loop_peg_1195;
                }
                result_peg_1193.addResult(result_peg_1196);
            } while (true);
            loop_peg_1195:
            ;
        
        Result result_peg_1194 = result_peg_1193;
        
        {
                
                result_peg_1193.reset();
                do{
                    Result result_peg_1208(result_peg_1193.getPosition());
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
                    result_peg_1193.addResult(result_peg_1208);
                } while (true);
                loop_peg_1207:
                ;
                        
            }
            goto success_peg_1198;
            goto out_peg_1214;
            success_peg_1198:
            ;
        
        
        
        result_peg_1193 = rule_expr13_real(stream, result_peg_1193.getPosition());
            if (result_peg_1193.error()){
                goto out_peg_1214;
            }
            exp = result_peg_1193.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_1194.getValues(), exp);
                result_peg_1193.setValue(value);
            }
        
        
    }
    
    if (column_peg_1192.chunk6 == 0){
        column_peg_1192.chunk6 = new Chunk6();
    }
    column_peg_1192.chunk6->chunk_expr13 = result_peg_1193;
    stream.update(result_peg_1193.getPosition());
    
    
    return result_peg_1193;
    out_peg_1214:
    
    if (column_peg_1192.chunk6 == 0){
        column_peg_1192.chunk6 = new Chunk6();
    }
    column_peg_1192.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1260(stream, "expr_rest");
    int myposition = position;
    tail_peg_1219:
    Value right;
    Value new_left;
    Result result_peg_1218(myposition);
    {
    
        {
                
                result_peg_1218.reset();
                do{
                    Result result_peg_1231(result_peg_1218.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1231.getPosition()))){
                                result_peg_1231.nextPosition();
                            } else {
                                goto out_peg_1234;
                            }
                        }
                        result_peg_1231.setValue((void*) " ");
                            
                    }
                    goto success_peg_1232;
                    out_peg_1234:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1231.getPosition()))){
                                result_peg_1231.nextPosition();
                            } else {
                                goto out_peg_1236;
                            }
                        }
                        result_peg_1231.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1232;
                    out_peg_1236:
                    goto loop_peg_1230;
                    success_peg_1232:
                    ;
                    result_peg_1218.addResult(result_peg_1231);
                } while (true);
                loop_peg_1230:
                ;
                        
            }
            goto success_peg_1221;
            goto out_peg_1237;
            success_peg_1221:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_1218.getPosition()))){
                    result_peg_1218.nextPosition();
                } else {
                    goto out_peg_1237;
                }
            }
            result_peg_1218.setValue((void*) "||");
        
        
        
        {
                
                result_peg_1218.reset();
                do{
                    Result result_peg_1250(result_peg_1218.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1250.getPosition()))){
                                result_peg_1250.nextPosition();
                            } else {
                                goto out_peg_1253;
                            }
                        }
                        result_peg_1250.setValue((void*) " ");
                            
                    }
                    goto success_peg_1251;
                    out_peg_1253:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1250.getPosition()))){
                                result_peg_1250.nextPosition();
                            } else {
                                goto out_peg_1255;
                            }
                        }
                        result_peg_1250.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1251;
                    out_peg_1255:
                    goto loop_peg_1249;
                    success_peg_1251:
                    ;
                    result_peg_1218.addResult(result_peg_1250);
                } while (true);
                loop_peg_1249:
                ;
                        
            }
            goto success_peg_1240;
            goto out_peg_1237;
            success_peg_1240:
            ;
        
        
        
        result_peg_1218 = rule_expr2(stream, result_peg_1218.getPosition());
            if (result_peg_1218.error()){
                goto out_peg_1237;
            }
            right = result_peg_1218.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_1218.setValue(value);
            }
            new_left = result_peg_1218.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1218.getPosition();
    goto tail_peg_1219;
    out_peg_1237:
    Result result_peg_1258(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1258.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1258.getPosition());
    
    return result_peg_1258;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1304(stream, "expr2_rest");
    int myposition = position;
    tail_peg_1263:
    Value right;
    Value new_left;
    Result result_peg_1262(myposition);
    {
    
        {
                
                result_peg_1262.reset();
                do{
                    Result result_peg_1275(result_peg_1262.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1275.getPosition()))){
                                result_peg_1275.nextPosition();
                            } else {
                                goto out_peg_1278;
                            }
                        }
                        result_peg_1275.setValue((void*) " ");
                            
                    }
                    goto success_peg_1276;
                    out_peg_1278:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1275.getPosition()))){
                                result_peg_1275.nextPosition();
                            } else {
                                goto out_peg_1280;
                            }
                        }
                        result_peg_1275.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1276;
                    out_peg_1280:
                    goto loop_peg_1274;
                    success_peg_1276:
                    ;
                    result_peg_1262.addResult(result_peg_1275);
                } while (true);
                loop_peg_1274:
                ;
                        
            }
            goto success_peg_1265;
            goto out_peg_1281;
            success_peg_1265:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_1262.getPosition()))){
                    result_peg_1262.nextPosition();
                } else {
                    goto out_peg_1281;
                }
            }
            result_peg_1262.setValue((void*) "^^");
        
        
        
        {
                
                result_peg_1262.reset();
                do{
                    Result result_peg_1294(result_peg_1262.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1294.getPosition()))){
                                result_peg_1294.nextPosition();
                            } else {
                                goto out_peg_1297;
                            }
                        }
                        result_peg_1294.setValue((void*) " ");
                            
                    }
                    goto success_peg_1295;
                    out_peg_1297:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1294.getPosition()))){
                                result_peg_1294.nextPosition();
                            } else {
                                goto out_peg_1299;
                            }
                        }
                        result_peg_1294.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1295;
                    out_peg_1299:
                    goto loop_peg_1293;
                    success_peg_1295:
                    ;
                    result_peg_1262.addResult(result_peg_1294);
                } while (true);
                loop_peg_1293:
                ;
                        
            }
            goto success_peg_1284;
            goto out_peg_1281;
            success_peg_1284:
            ;
        
        
        
        result_peg_1262 = rule_expr3(stream, result_peg_1262.getPosition());
            if (result_peg_1262.error()){
                goto out_peg_1281;
            }
            right = result_peg_1262.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_1262.setValue(value);
            }
            new_left = result_peg_1262.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1262.getPosition();
    goto tail_peg_1263;
    out_peg_1281:
    Result result_peg_1302(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1302.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1302.getPosition());
    
    return result_peg_1302;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1348(stream, "expr3_rest");
    int myposition = position;
    tail_peg_1307:
    Value right;
    Value new_left;
    Result result_peg_1306(myposition);
    {
    
        {
                
                result_peg_1306.reset();
                do{
                    Result result_peg_1319(result_peg_1306.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1319.getPosition()))){
                                result_peg_1319.nextPosition();
                            } else {
                                goto out_peg_1322;
                            }
                        }
                        result_peg_1319.setValue((void*) " ");
                            
                    }
                    goto success_peg_1320;
                    out_peg_1322:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1319.getPosition()))){
                                result_peg_1319.nextPosition();
                            } else {
                                goto out_peg_1324;
                            }
                        }
                        result_peg_1319.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1320;
                    out_peg_1324:
                    goto loop_peg_1318;
                    success_peg_1320:
                    ;
                    result_peg_1306.addResult(result_peg_1319);
                } while (true);
                loop_peg_1318:
                ;
                        
            }
            goto success_peg_1309;
            goto out_peg_1325;
            success_peg_1309:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_1306.getPosition()))){
                    result_peg_1306.nextPosition();
                } else {
                    goto out_peg_1325;
                }
            }
            result_peg_1306.setValue((void*) "&&");
        
        
        
        {
                
                result_peg_1306.reset();
                do{
                    Result result_peg_1338(result_peg_1306.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1338.getPosition()))){
                                result_peg_1338.nextPosition();
                            } else {
                                goto out_peg_1341;
                            }
                        }
                        result_peg_1338.setValue((void*) " ");
                            
                    }
                    goto success_peg_1339;
                    out_peg_1341:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1338.getPosition()))){
                                result_peg_1338.nextPosition();
                            } else {
                                goto out_peg_1343;
                            }
                        }
                        result_peg_1338.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1339;
                    out_peg_1343:
                    goto loop_peg_1337;
                    success_peg_1339:
                    ;
                    result_peg_1306.addResult(result_peg_1338);
                } while (true);
                loop_peg_1337:
                ;
                        
            }
            goto success_peg_1328;
            goto out_peg_1325;
            success_peg_1328:
            ;
        
        
        
        result_peg_1306 = rule_expr4(stream, result_peg_1306.getPosition());
            if (result_peg_1306.error()){
                goto out_peg_1325;
            }
            right = result_peg_1306.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_1306.setValue(value);
            }
            new_left = result_peg_1306.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1306.getPosition();
    goto tail_peg_1307;
    out_peg_1325:
    Result result_peg_1346(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1346.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1346.getPosition());
    
    return result_peg_1346;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1392(stream, "expr4_rest");
    int myposition = position;
    tail_peg_1351:
    Value right;
    Value new_left;
    Result result_peg_1350(myposition);
    {
    
        {
                
                result_peg_1350.reset();
                do{
                    Result result_peg_1363(result_peg_1350.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1363.getPosition()))){
                                result_peg_1363.nextPosition();
                            } else {
                                goto out_peg_1366;
                            }
                        }
                        result_peg_1363.setValue((void*) " ");
                            
                    }
                    goto success_peg_1364;
                    out_peg_1366:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1363.getPosition()))){
                                result_peg_1363.nextPosition();
                            } else {
                                goto out_peg_1368;
                            }
                        }
                        result_peg_1363.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1364;
                    out_peg_1368:
                    goto loop_peg_1362;
                    success_peg_1364:
                    ;
                    result_peg_1350.addResult(result_peg_1363);
                } while (true);
                loop_peg_1362:
                ;
                        
            }
            goto success_peg_1353;
            goto out_peg_1369;
            success_peg_1353:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_1350.getPosition()))){
                    result_peg_1350.nextPosition();
                } else {
                    goto out_peg_1369;
                }
            }
            result_peg_1350.setValue((void*) "|");
        
        
        
        {
                
                result_peg_1350.reset();
                do{
                    Result result_peg_1382(result_peg_1350.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1382.getPosition()))){
                                result_peg_1382.nextPosition();
                            } else {
                                goto out_peg_1385;
                            }
                        }
                        result_peg_1382.setValue((void*) " ");
                            
                    }
                    goto success_peg_1383;
                    out_peg_1385:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1382.getPosition()))){
                                result_peg_1382.nextPosition();
                            } else {
                                goto out_peg_1387;
                            }
                        }
                        result_peg_1382.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1383;
                    out_peg_1387:
                    goto loop_peg_1381;
                    success_peg_1383:
                    ;
                    result_peg_1350.addResult(result_peg_1382);
                } while (true);
                loop_peg_1381:
                ;
                        
            }
            goto success_peg_1372;
            goto out_peg_1369;
            success_peg_1372:
            ;
        
        
        
        result_peg_1350 = rule_expr5(stream, result_peg_1350.getPosition());
            if (result_peg_1350.error()){
                goto out_peg_1369;
            }
            right = result_peg_1350.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_1350.setValue(value);
            }
            new_left = result_peg_1350.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1350.getPosition();
    goto tail_peg_1351;
    out_peg_1369:
    Result result_peg_1390(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1390.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1390.getPosition());
    
    return result_peg_1390;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1436(stream, "expr5_rest");
    int myposition = position;
    tail_peg_1395:
    Value right;
    Value new_left;
    Result result_peg_1394(myposition);
    {
    
        {
                
                result_peg_1394.reset();
                do{
                    Result result_peg_1407(result_peg_1394.getPosition());
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
                    result_peg_1394.addResult(result_peg_1407);
                } while (true);
                loop_peg_1406:
                ;
                        
            }
            goto success_peg_1397;
            goto out_peg_1413;
            success_peg_1397:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_1394.getPosition()))){
                    result_peg_1394.nextPosition();
                } else {
                    goto out_peg_1413;
                }
            }
            result_peg_1394.setValue((void*) "^");
        
        
        
        {
                
                result_peg_1394.reset();
                do{
                    Result result_peg_1426(result_peg_1394.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1426.getPosition()))){
                                result_peg_1426.nextPosition();
                            } else {
                                goto out_peg_1429;
                            }
                        }
                        result_peg_1426.setValue((void*) " ");
                            
                    }
                    goto success_peg_1427;
                    out_peg_1429:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1426.getPosition()))){
                                result_peg_1426.nextPosition();
                            } else {
                                goto out_peg_1431;
                            }
                        }
                        result_peg_1426.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1427;
                    out_peg_1431:
                    goto loop_peg_1425;
                    success_peg_1427:
                    ;
                    result_peg_1394.addResult(result_peg_1426);
                } while (true);
                loop_peg_1425:
                ;
                        
            }
            goto success_peg_1416;
            goto out_peg_1413;
            success_peg_1416:
            ;
        
        
        
        result_peg_1394 = rule_expr6(stream, result_peg_1394.getPosition());
            if (result_peg_1394.error()){
                goto out_peg_1413;
            }
            right = result_peg_1394.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_1394.setValue(value);
            }
            new_left = result_peg_1394.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1394.getPosition();
    goto tail_peg_1395;
    out_peg_1413:
    Result result_peg_1434(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1434.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1434.getPosition());
    
    return result_peg_1434;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1480(stream, "expr6_rest");
    int myposition = position;
    tail_peg_1439:
    Value right;
    Value new_left;
    Result result_peg_1438(myposition);
    {
    
        {
                
                result_peg_1438.reset();
                do{
                    Result result_peg_1451(result_peg_1438.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1451.getPosition()))){
                                result_peg_1451.nextPosition();
                            } else {
                                goto out_peg_1454;
                            }
                        }
                        result_peg_1451.setValue((void*) " ");
                            
                    }
                    goto success_peg_1452;
                    out_peg_1454:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1451.getPosition()))){
                                result_peg_1451.nextPosition();
                            } else {
                                goto out_peg_1456;
                            }
                        }
                        result_peg_1451.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1452;
                    out_peg_1456:
                    goto loop_peg_1450;
                    success_peg_1452:
                    ;
                    result_peg_1438.addResult(result_peg_1451);
                } while (true);
                loop_peg_1450:
                ;
                        
            }
            goto success_peg_1441;
            goto out_peg_1457;
            success_peg_1441:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_1438.getPosition()))){
                    result_peg_1438.nextPosition();
                } else {
                    goto out_peg_1457;
                }
            }
            result_peg_1438.setValue((void*) "&");
        
        
        
        {
                
                result_peg_1438.reset();
                do{
                    Result result_peg_1470(result_peg_1438.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1470.getPosition()))){
                                result_peg_1470.nextPosition();
                            } else {
                                goto out_peg_1473;
                            }
                        }
                        result_peg_1470.setValue((void*) " ");
                            
                    }
                    goto success_peg_1471;
                    out_peg_1473:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1470.getPosition()))){
                                result_peg_1470.nextPosition();
                            } else {
                                goto out_peg_1475;
                            }
                        }
                        result_peg_1470.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1471;
                    out_peg_1475:
                    goto loop_peg_1469;
                    success_peg_1471:
                    ;
                    result_peg_1438.addResult(result_peg_1470);
                } while (true);
                loop_peg_1469:
                ;
                        
            }
            goto success_peg_1460;
            goto out_peg_1457;
            success_peg_1460:
            ;
        
        
        
        result_peg_1438 = rule_expr7(stream, result_peg_1438.getPosition());
            if (result_peg_1438.error()){
                goto out_peg_1457;
            }
            right = result_peg_1438.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_1438.setValue(value);
            }
            new_left = result_peg_1438.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1438.getPosition();
    goto tail_peg_1439;
    out_peg_1457:
    Result result_peg_1478(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1478.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1478.getPosition());
    
    return result_peg_1478;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1524(stream, "expr7_rest");
    int myposition = position;
    tail_peg_1483:
    Value right;
    Value new_left;
    Result result_peg_1482(myposition);
    {
    
        {
                
                result_peg_1482.reset();
                do{
                    Result result_peg_1495(result_peg_1482.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1495.getPosition()))){
                                result_peg_1495.nextPosition();
                            } else {
                                goto out_peg_1498;
                            }
                        }
                        result_peg_1495.setValue((void*) " ");
                            
                    }
                    goto success_peg_1496;
                    out_peg_1498:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1495.getPosition()))){
                                result_peg_1495.nextPosition();
                            } else {
                                goto out_peg_1500;
                            }
                        }
                        result_peg_1495.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1496;
                    out_peg_1500:
                    goto loop_peg_1494;
                    success_peg_1496:
                    ;
                    result_peg_1482.addResult(result_peg_1495);
                } while (true);
                loop_peg_1494:
                ;
                        
            }
            goto success_peg_1485;
            goto out_peg_1501;
            success_peg_1485:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_1482.getPosition()))){
                    result_peg_1482.nextPosition();
                } else {
                    goto out_peg_1501;
                }
            }
            result_peg_1482.setValue((void*) ":=");
        
        
        
        {
                
                result_peg_1482.reset();
                do{
                    Result result_peg_1514(result_peg_1482.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1514.getPosition()))){
                                result_peg_1514.nextPosition();
                            } else {
                                goto out_peg_1517;
                            }
                        }
                        result_peg_1514.setValue((void*) " ");
                            
                    }
                    goto success_peg_1515;
                    out_peg_1517:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1514.getPosition()))){
                                result_peg_1514.nextPosition();
                            } else {
                                goto out_peg_1519;
                            }
                        }
                        result_peg_1514.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1515;
                    out_peg_1519:
                    goto loop_peg_1513;
                    success_peg_1515:
                    ;
                    result_peg_1482.addResult(result_peg_1514);
                } while (true);
                loop_peg_1513:
                ;
                        
            }
            goto success_peg_1504;
            goto out_peg_1501;
            success_peg_1504:
            ;
        
        
        
        result_peg_1482 = rule_expr8(stream, result_peg_1482.getPosition());
            if (result_peg_1482.error()){
                goto out_peg_1501;
            }
            right = result_peg_1482.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_1482.setValue(value);
            }
            new_left = result_peg_1482.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1482.getPosition();
    goto tail_peg_1483;
    out_peg_1501:
    Result result_peg_1522(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1522.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1522.getPosition());
    
    return result_peg_1522;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1607(stream, "expr8_rest");
    int myposition = position;
    tail_peg_1527:
    Value right;
    Value new_left;
    Result result_peg_1526(myposition);
    {
    
        {
                
                result_peg_1526.reset();
                do{
                    Result result_peg_1539(result_peg_1526.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1539.getPosition()))){
                                result_peg_1539.nextPosition();
                            } else {
                                goto out_peg_1542;
                            }
                        }
                        result_peg_1539.setValue((void*) " ");
                            
                    }
                    goto success_peg_1540;
                    out_peg_1542:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1539.getPosition()))){
                                result_peg_1539.nextPosition();
                            } else {
                                goto out_peg_1544;
                            }
                        }
                        result_peg_1539.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1540;
                    out_peg_1544:
                    goto loop_peg_1538;
                    success_peg_1540:
                    ;
                    result_peg_1526.addResult(result_peg_1539);
                } while (true);
                loop_peg_1538:
                ;
                        
            }
            goto success_peg_1529;
            goto out_peg_1545;
            success_peg_1529:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1526.getPosition()))){
                    result_peg_1526.nextPosition();
                } else {
                    goto out_peg_1545;
                }
            }
            result_peg_1526.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1526.reset();
                do{
                    Result result_peg_1558(result_peg_1526.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1558.getPosition()))){
                                result_peg_1558.nextPosition();
                            } else {
                                goto out_peg_1561;
                            }
                        }
                        result_peg_1558.setValue((void*) " ");
                            
                    }
                    goto success_peg_1559;
                    out_peg_1561:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1558.getPosition()))){
                                result_peg_1558.nextPosition();
                            } else {
                                goto out_peg_1563;
                            }
                        }
                        result_peg_1558.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1559;
                    out_peg_1563:
                    goto loop_peg_1557;
                    success_peg_1559:
                    ;
                    result_peg_1526.addResult(result_peg_1558);
                } while (true);
                loop_peg_1557:
                ;
                        
            }
            goto success_peg_1548;
            goto out_peg_1545;
            success_peg_1548:
            ;
        
        
        
        result_peg_1526 = rule_expr9(stream, result_peg_1526.getPosition());
            if (result_peg_1526.error()){
                goto out_peg_1545;
            }
            right = result_peg_1526.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_1526.setValue(value);
            }
            new_left = result_peg_1526.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1526.getPosition();
    goto tail_peg_1527;
    out_peg_1545:
    Result result_peg_1566(myposition);
    {
    
        {
                
                result_peg_1566.reset();
                do{
                    Result result_peg_1578(result_peg_1566.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1578.getPosition()))){
                                result_peg_1578.nextPosition();
                            } else {
                                goto out_peg_1581;
                            }
                        }
                        result_peg_1578.setValue((void*) " ");
                            
                    }
                    goto success_peg_1579;
                    out_peg_1581:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1578.getPosition()))){
                                result_peg_1578.nextPosition();
                            } else {
                                goto out_peg_1583;
                            }
                        }
                        result_peg_1578.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1579;
                    out_peg_1583:
                    goto loop_peg_1577;
                    success_peg_1579:
                    ;
                    result_peg_1566.addResult(result_peg_1578);
                } while (true);
                loop_peg_1577:
                ;
                        
            }
            goto success_peg_1568;
            goto out_peg_1584;
            success_peg_1568:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_1566.getPosition()))){
                    result_peg_1566.nextPosition();
                } else {
                    goto out_peg_1584;
                }
            }
            result_peg_1566.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_1566.reset();
                do{
                    Result result_peg_1597(result_peg_1566.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1597.getPosition()))){
                                result_peg_1597.nextPosition();
                            } else {
                                goto out_peg_1600;
                            }
                        }
                        result_peg_1597.setValue((void*) " ");
                            
                    }
                    goto success_peg_1598;
                    out_peg_1600:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1597.getPosition()))){
                                result_peg_1597.nextPosition();
                            } else {
                                goto out_peg_1602;
                            }
                        }
                        result_peg_1597.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1598;
                    out_peg_1602:
                    goto loop_peg_1596;
                    success_peg_1598:
                    ;
                    result_peg_1566.addResult(result_peg_1597);
                } while (true);
                loop_peg_1596:
                ;
                        
            }
            goto success_peg_1587;
            goto out_peg_1584;
            success_peg_1587:
            ;
        
        
        
        result_peg_1566 = rule_expr9(stream, result_peg_1566.getPosition());
            if (result_peg_1566.error()){
                goto out_peg_1584;
            }
            right = result_peg_1566.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_1566.setValue(value);
            }
            new_left = result_peg_1566.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1566.getPosition();
    goto tail_peg_1527;
    out_peg_1584:
    Result result_peg_1605(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1605.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1605.getPosition());
    
    return result_peg_1605;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1651(stream, "expr9_rest");
    int myposition = position;
    tail_peg_1610:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_1609(myposition);
    {
    
        {
                
                result_peg_1609.reset();
                do{
                    Result result_peg_1622(result_peg_1609.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1622.getPosition()))){
                                result_peg_1622.nextPosition();
                            } else {
                                goto out_peg_1625;
                            }
                        }
                        result_peg_1622.setValue((void*) " ");
                            
                    }
                    goto success_peg_1623;
                    out_peg_1625:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1622.getPosition()))){
                                result_peg_1622.nextPosition();
                            } else {
                                goto out_peg_1627;
                            }
                        }
                        result_peg_1622.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1623;
                    out_peg_1627:
                    goto loop_peg_1621;
                    success_peg_1623:
                    ;
                    result_peg_1609.addResult(result_peg_1622);
                } while (true);
                loop_peg_1621:
                ;
                        
            }
            goto success_peg_1612;
            goto out_peg_1628;
            success_peg_1612:
            ;
        
        
        
        result_peg_1609 = rule_compare(stream, result_peg_1609.getPosition());
            if (result_peg_1609.error()){
                goto out_peg_1628;
            }
            maker = result_peg_1609.getValues();
        
        
        
        {
                
                result_peg_1609.reset();
                do{
                    Result result_peg_1641(result_peg_1609.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1641.getPosition()))){
                                result_peg_1641.nextPosition();
                            } else {
                                goto out_peg_1644;
                            }
                        }
                        result_peg_1641.setValue((void*) " ");
                            
                    }
                    goto success_peg_1642;
                    out_peg_1644:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1641.getPosition()))){
                                result_peg_1641.nextPosition();
                            } else {
                                goto out_peg_1646;
                            }
                        }
                        result_peg_1641.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1642;
                    out_peg_1646:
                    goto loop_peg_1640;
                    success_peg_1642:
                    ;
                    result_peg_1609.addResult(result_peg_1641);
                } while (true);
                loop_peg_1640:
                ;
                        
            }
            goto success_peg_1631;
            goto out_peg_1628;
            success_peg_1631:
            ;
        
        
        
        result_peg_1609 = rule_expr10(stream, result_peg_1609.getPosition());
            if (result_peg_1609.error()){
                goto out_peg_1628;
            }
            right = result_peg_1609.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_1609.setValue(value);
            }
            new_left = result_peg_1609.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1609.getPosition();
    goto tail_peg_1610;
    out_peg_1628:
    Result result_peg_1649(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1649.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1649.getPosition());
    
    return result_peg_1649;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1652 = stream.getColumn(position);
    if (column_peg_1652.chunk8 != 0 && column_peg_1652.chunk8->chunk_compare.calculated()){
        return column_peg_1652.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_1665(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_1653(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_1653.getPosition()))){
                    result_peg_1653.nextPosition();
                } else {
                    goto out_peg_1655;
                }
            }
            result_peg_1653.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_1653.setValue(value);
            }
        
        
    }
    
    if (column_peg_1652.chunk8 == 0){
        column_peg_1652.chunk8 = new Chunk8();
    }
    column_peg_1652.chunk8->chunk_compare = result_peg_1653;
    stream.update(result_peg_1653.getPosition());
    
    
    return result_peg_1653;
    out_peg_1655:
    Result result_peg_1656(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_1656.getPosition()))){
                    result_peg_1656.nextPosition();
                } else {
                    goto out_peg_1658;
                }
            }
            result_peg_1656.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_1656.setValue(value);
            }
        
        
    }
    
    if (column_peg_1652.chunk8 == 0){
        column_peg_1652.chunk8 = new Chunk8();
    }
    column_peg_1652.chunk8->chunk_compare = result_peg_1656;
    stream.update(result_peg_1656.getPosition());
    
    
    return result_peg_1656;
    out_peg_1658:
    Result result_peg_1659(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_1659.getPosition()))){
                    result_peg_1659.nextPosition();
                } else {
                    goto out_peg_1661;
                }
            }
            result_peg_1659.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_1659.setValue(value);
            }
        
        
    }
    
    if (column_peg_1652.chunk8 == 0){
        column_peg_1652.chunk8 = new Chunk8();
    }
    column_peg_1652.chunk8->chunk_compare = result_peg_1659;
    stream.update(result_peg_1659.getPosition());
    
    
    return result_peg_1659;
    out_peg_1661:
    Result result_peg_1662(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_1662.getPosition()))){
                    result_peg_1662.nextPosition();
                } else {
                    goto out_peg_1664;
                }
            }
            result_peg_1662.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_1662.setValue(value);
            }
        
        
    }
    
    if (column_peg_1652.chunk8 == 0){
        column_peg_1652.chunk8 = new Chunk8();
    }
    column_peg_1652.chunk8->chunk_compare = result_peg_1662;
    stream.update(result_peg_1662.getPosition());
    
    
    return result_peg_1662;
    out_peg_1664:
    
    if (column_peg_1652.chunk8 == 0){
        column_peg_1652.chunk8 = new Chunk8();
    }
    column_peg_1652.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1666 = stream.getColumn(position);
    if (column_peg_1666.chunk8 != 0 && column_peg_1666.chunk8->chunk_all_compare.calculated()){
        return column_peg_1666.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_1673(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_1667(myposition);
    
    result_peg_1667 = rule_compare(stream, result_peg_1667.getPosition());
    if (result_peg_1667.error()){
        goto out_peg_1668;
    }
    
    if (column_peg_1666.chunk8 == 0){
        column_peg_1666.chunk8 = new Chunk8();
    }
    column_peg_1666.chunk8->chunk_all_compare = result_peg_1667;
    stream.update(result_peg_1667.getPosition());
    
    
    return result_peg_1667;
    out_peg_1668:
    Result result_peg_1669(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_1669.getPosition()))){
            result_peg_1669.nextPosition();
        } else {
            goto out_peg_1670;
        }
    }
    result_peg_1669.setValue((void*) "=");
    
    if (column_peg_1666.chunk8 == 0){
        column_peg_1666.chunk8 = new Chunk8();
    }
    column_peg_1666.chunk8->chunk_all_compare = result_peg_1669;
    stream.update(result_peg_1669.getPosition());
    
    
    return result_peg_1669;
    out_peg_1670:
    Result result_peg_1671(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_1671.getPosition()))){
            result_peg_1671.nextPosition();
        } else {
            goto out_peg_1672;
        }
    }
    result_peg_1671.setValue((void*) "!=");
    
    if (column_peg_1666.chunk8 == 0){
        column_peg_1666.chunk8 = new Chunk8();
    }
    column_peg_1666.chunk8->chunk_all_compare = result_peg_1671;
    stream.update(result_peg_1671.getPosition());
    
    
    return result_peg_1671;
    out_peg_1672:
    
    if (column_peg_1666.chunk8 == 0){
        column_peg_1666.chunk8 = new Chunk8();
    }
    column_peg_1666.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1756(stream, "expr10_rest");
    int myposition = position;
    tail_peg_1676:
    Value right;
    Value new_left;
    Result result_peg_1675(myposition);
    {
    
        {
                
                result_peg_1675.reset();
                do{
                    Result result_peg_1688(result_peg_1675.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1688.getPosition()))){
                                result_peg_1688.nextPosition();
                            } else {
                                goto out_peg_1691;
                            }
                        }
                        result_peg_1688.setValue((void*) " ");
                            
                    }
                    goto success_peg_1689;
                    out_peg_1691:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1688.getPosition()))){
                                result_peg_1688.nextPosition();
                            } else {
                                goto out_peg_1693;
                            }
                        }
                        result_peg_1688.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1689;
                    out_peg_1693:
                    goto loop_peg_1687;
                    success_peg_1689:
                    ;
                    result_peg_1675.addResult(result_peg_1688);
                } while (true);
                loop_peg_1687:
                ;
                        
            }
            goto success_peg_1678;
            goto out_peg_1694;
            success_peg_1678:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_1675.getPosition()))){
                    result_peg_1675.nextPosition();
                } else {
                    goto out_peg_1694;
                }
            }
            result_peg_1675.setValue((void*) "+");
        
        
        
        {
                
                result_peg_1675.reset();
                do{
                    Result result_peg_1707(result_peg_1675.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1707.getPosition()))){
                                result_peg_1707.nextPosition();
                            } else {
                                goto out_peg_1710;
                            }
                        }
                        result_peg_1707.setValue((void*) " ");
                            
                    }
                    goto success_peg_1708;
                    out_peg_1710:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1707.getPosition()))){
                                result_peg_1707.nextPosition();
                            } else {
                                goto out_peg_1712;
                            }
                        }
                        result_peg_1707.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1708;
                    out_peg_1712:
                    goto loop_peg_1706;
                    success_peg_1708:
                    ;
                    result_peg_1675.addResult(result_peg_1707);
                } while (true);
                loop_peg_1706:
                ;
                        
            }
            goto success_peg_1697;
            goto out_peg_1694;
            success_peg_1697:
            ;
        
        
        
        result_peg_1675 = rule_expr11(stream, result_peg_1675.getPosition());
            if (result_peg_1675.error()){
                goto out_peg_1694;
            }
            right = result_peg_1675.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_1675.setValue(value);
            }
            new_left = result_peg_1675.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1675.getPosition();
    goto tail_peg_1676;
    out_peg_1694:
    Result result_peg_1715(myposition);
    {
    
        {
                
                result_peg_1715.reset();
                do{
                    Result result_peg_1727(result_peg_1715.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1727.getPosition()))){
                                result_peg_1727.nextPosition();
                            } else {
                                goto out_peg_1730;
                            }
                        }
                        result_peg_1727.setValue((void*) " ");
                            
                    }
                    goto success_peg_1728;
                    out_peg_1730:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1727.getPosition()))){
                                result_peg_1727.nextPosition();
                            } else {
                                goto out_peg_1732;
                            }
                        }
                        result_peg_1727.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1728;
                    out_peg_1732:
                    goto loop_peg_1726;
                    success_peg_1728:
                    ;
                    result_peg_1715.addResult(result_peg_1727);
                } while (true);
                loop_peg_1726:
                ;
                        
            }
            goto success_peg_1717;
            goto out_peg_1733;
            success_peg_1717:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1715.getPosition()))){
                    result_peg_1715.nextPosition();
                } else {
                    goto out_peg_1733;
                }
            }
            result_peg_1715.setValue((void*) "-");
        
        
        
        {
                
                result_peg_1715.reset();
                do{
                    Result result_peg_1746(result_peg_1715.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1746.getPosition()))){
                                result_peg_1746.nextPosition();
                            } else {
                                goto out_peg_1749;
                            }
                        }
                        result_peg_1746.setValue((void*) " ");
                            
                    }
                    goto success_peg_1747;
                    out_peg_1749:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1746.getPosition()))){
                                result_peg_1746.nextPosition();
                            } else {
                                goto out_peg_1751;
                            }
                        }
                        result_peg_1746.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1747;
                    out_peg_1751:
                    goto loop_peg_1745;
                    success_peg_1747:
                    ;
                    result_peg_1715.addResult(result_peg_1746);
                } while (true);
                loop_peg_1745:
                ;
                        
            }
            goto success_peg_1736;
            goto out_peg_1733;
            success_peg_1736:
            ;
        
        
        
        result_peg_1715 = rule_expr11(stream, result_peg_1715.getPosition());
            if (result_peg_1715.error()){
                goto out_peg_1733;
            }
            right = result_peg_1715.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_1715.setValue(value);
            }
            new_left = result_peg_1715.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1715.getPosition();
    goto tail_peg_1676;
    out_peg_1733:
    Result result_peg_1754(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1754.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1754.getPosition());
    
    return result_peg_1754;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1878(stream, "expr11_rest");
    int myposition = position;
    tail_peg_1759:
    Value right;
    Value new_left;
    Result result_peg_1758(myposition);
    {
    
        {
                
                result_peg_1758.reset();
                do{
                    Result result_peg_1771(result_peg_1758.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1771.getPosition()))){
                                result_peg_1771.nextPosition();
                            } else {
                                goto out_peg_1774;
                            }
                        }
                        result_peg_1771.setValue((void*) " ");
                            
                    }
                    goto success_peg_1772;
                    out_peg_1774:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1771.getPosition()))){
                                result_peg_1771.nextPosition();
                            } else {
                                goto out_peg_1776;
                            }
                        }
                        result_peg_1771.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1772;
                    out_peg_1776:
                    goto loop_peg_1770;
                    success_peg_1772:
                    ;
                    result_peg_1758.addResult(result_peg_1771);
                } while (true);
                loop_peg_1770:
                ;
                        
            }
            goto success_peg_1761;
            goto out_peg_1777;
            success_peg_1761:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_1758.getPosition()))){
                    result_peg_1758.nextPosition();
                } else {
                    goto out_peg_1777;
                }
            }
            result_peg_1758.setValue((void*) "*");
        
        
        
        {
                
                result_peg_1758.reset();
                do{
                    Result result_peg_1790(result_peg_1758.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1790.getPosition()))){
                                result_peg_1790.nextPosition();
                            } else {
                                goto out_peg_1793;
                            }
                        }
                        result_peg_1790.setValue((void*) " ");
                            
                    }
                    goto success_peg_1791;
                    out_peg_1793:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1790.getPosition()))){
                                result_peg_1790.nextPosition();
                            } else {
                                goto out_peg_1795;
                            }
                        }
                        result_peg_1790.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1791;
                    out_peg_1795:
                    goto loop_peg_1789;
                    success_peg_1791:
                    ;
                    result_peg_1758.addResult(result_peg_1790);
                } while (true);
                loop_peg_1789:
                ;
                        
            }
            goto success_peg_1780;
            goto out_peg_1777;
            success_peg_1780:
            ;
        
        
        
        result_peg_1758 = rule_expr12(stream, result_peg_1758.getPosition());
            if (result_peg_1758.error()){
                goto out_peg_1777;
            }
            right = result_peg_1758.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_1758.setValue(value);
            }
            new_left = result_peg_1758.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1758.getPosition();
    goto tail_peg_1759;
    out_peg_1777:
    Result result_peg_1798(myposition);
    {
    
        {
                
                result_peg_1798.reset();
                do{
                    Result result_peg_1810(result_peg_1798.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1810.getPosition()))){
                                result_peg_1810.nextPosition();
                            } else {
                                goto out_peg_1813;
                            }
                        }
                        result_peg_1810.setValue((void*) " ");
                            
                    }
                    goto success_peg_1811;
                    out_peg_1813:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1810.getPosition()))){
                                result_peg_1810.nextPosition();
                            } else {
                                goto out_peg_1815;
                            }
                        }
                        result_peg_1810.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1811;
                    out_peg_1815:
                    goto loop_peg_1809;
                    success_peg_1811:
                    ;
                    result_peg_1798.addResult(result_peg_1810);
                } while (true);
                loop_peg_1809:
                ;
                        
            }
            goto success_peg_1800;
            goto out_peg_1816;
            success_peg_1800:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_1798.getPosition()))){
                    result_peg_1798.nextPosition();
                } else {
                    goto out_peg_1816;
                }
            }
            result_peg_1798.setValue((void*) "/");
        
        
        
        {
                
                result_peg_1798.reset();
                do{
                    Result result_peg_1829(result_peg_1798.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1829.getPosition()))){
                                result_peg_1829.nextPosition();
                            } else {
                                goto out_peg_1832;
                            }
                        }
                        result_peg_1829.setValue((void*) " ");
                            
                    }
                    goto success_peg_1830;
                    out_peg_1832:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1829.getPosition()))){
                                result_peg_1829.nextPosition();
                            } else {
                                goto out_peg_1834;
                            }
                        }
                        result_peg_1829.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1830;
                    out_peg_1834:
                    goto loop_peg_1828;
                    success_peg_1830:
                    ;
                    result_peg_1798.addResult(result_peg_1829);
                } while (true);
                loop_peg_1828:
                ;
                        
            }
            goto success_peg_1819;
            goto out_peg_1816;
            success_peg_1819:
            ;
        
        
        
        result_peg_1798 = rule_expr12(stream, result_peg_1798.getPosition());
            if (result_peg_1798.error()){
                goto out_peg_1816;
            }
            right = result_peg_1798.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_1798.setValue(value);
            }
            new_left = result_peg_1798.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1798.getPosition();
    goto tail_peg_1759;
    out_peg_1816:
    Result result_peg_1837(myposition);
    {
    
        {
                
                result_peg_1837.reset();
                do{
                    Result result_peg_1849(result_peg_1837.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1849.getPosition()))){
                                result_peg_1849.nextPosition();
                            } else {
                                goto out_peg_1852;
                            }
                        }
                        result_peg_1849.setValue((void*) " ");
                            
                    }
                    goto success_peg_1850;
                    out_peg_1852:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1849.getPosition()))){
                                result_peg_1849.nextPosition();
                            } else {
                                goto out_peg_1854;
                            }
                        }
                        result_peg_1849.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1850;
                    out_peg_1854:
                    goto loop_peg_1848;
                    success_peg_1850:
                    ;
                    result_peg_1837.addResult(result_peg_1849);
                } while (true);
                loop_peg_1848:
                ;
                        
            }
            goto success_peg_1839;
            goto out_peg_1855;
            success_peg_1839:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_1837.getPosition()))){
                    result_peg_1837.nextPosition();
                } else {
                    goto out_peg_1855;
                }
            }
            result_peg_1837.setValue((void*) "%");
        
        
        
        {
                
                result_peg_1837.reset();
                do{
                    Result result_peg_1868(result_peg_1837.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1868.getPosition()))){
                                result_peg_1868.nextPosition();
                            } else {
                                goto out_peg_1871;
                            }
                        }
                        result_peg_1868.setValue((void*) " ");
                            
                    }
                    goto success_peg_1869;
                    out_peg_1871:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1868.getPosition()))){
                                result_peg_1868.nextPosition();
                            } else {
                                goto out_peg_1873;
                            }
                        }
                        result_peg_1868.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1869;
                    out_peg_1873:
                    goto loop_peg_1867;
                    success_peg_1869:
                    ;
                    result_peg_1837.addResult(result_peg_1868);
                } while (true);
                loop_peg_1867:
                ;
                        
            }
            goto success_peg_1858;
            goto out_peg_1855;
            success_peg_1858:
            ;
        
        
        
        result_peg_1837 = rule_expr12(stream, result_peg_1837.getPosition());
            if (result_peg_1837.error()){
                goto out_peg_1855;
            }
            right = result_peg_1837.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_1837.setValue(value);
            }
            new_left = result_peg_1837.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1837.getPosition();
    goto tail_peg_1759;
    out_peg_1855:
    Result result_peg_1876(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1876.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1876.getPosition());
    
    return result_peg_1876;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1922(stream, "expr12_rest");
    int myposition = position;
    tail_peg_1881:
    Value right;
    Value new_left;
    Result result_peg_1880(myposition);
    {
    
        {
                
                result_peg_1880.reset();
                do{
                    Result result_peg_1893(result_peg_1880.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1893.getPosition()))){
                                result_peg_1893.nextPosition();
                            } else {
                                goto out_peg_1896;
                            }
                        }
                        result_peg_1893.setValue((void*) " ");
                            
                    }
                    goto success_peg_1894;
                    out_peg_1896:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1893.getPosition()))){
                                result_peg_1893.nextPosition();
                            } else {
                                goto out_peg_1898;
                            }
                        }
                        result_peg_1893.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1894;
                    out_peg_1898:
                    goto loop_peg_1892;
                    success_peg_1894:
                    ;
                    result_peg_1880.addResult(result_peg_1893);
                } while (true);
                loop_peg_1892:
                ;
                        
            }
            goto success_peg_1883;
            goto out_peg_1899;
            success_peg_1883:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_1880.getPosition()))){
                    result_peg_1880.nextPosition();
                } else {
                    goto out_peg_1899;
                }
            }
            result_peg_1880.setValue((void*) "**");
        
        
        
        {
                
                result_peg_1880.reset();
                do{
                    Result result_peg_1912(result_peg_1880.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1912.getPosition()))){
                                result_peg_1912.nextPosition();
                            } else {
                                goto out_peg_1915;
                            }
                        }
                        result_peg_1912.setValue((void*) " ");
                            
                    }
                    goto success_peg_1913;
                    out_peg_1915:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1912.getPosition()))){
                                result_peg_1912.nextPosition();
                            } else {
                                goto out_peg_1917;
                            }
                        }
                        result_peg_1912.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1913;
                    out_peg_1917:
                    goto loop_peg_1911;
                    success_peg_1913:
                    ;
                    result_peg_1880.addResult(result_peg_1912);
                } while (true);
                loop_peg_1911:
                ;
                        
            }
            goto success_peg_1902;
            goto out_peg_1899;
            success_peg_1902:
            ;
        
        
        
        result_peg_1880 = rule_expr13(stream, result_peg_1880.getPosition());
            if (result_peg_1880.error()){
                goto out_peg_1899;
            }
            right = result_peg_1880.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_1880.setValue(value);
            }
            new_left = result_peg_1880.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1880.getPosition();
    goto tail_peg_1881;
    out_peg_1899:
    Result result_peg_1920(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1920.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1920.getPosition());
    
    return result_peg_1920;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_1923 = stream.getColumn(position);
    if (column_peg_1923.chunk9 != 0 && column_peg_1923.chunk9->chunk_unary.calculated()){
        return column_peg_1923.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_1952(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_1924(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_1924.getPosition()))){
                    result_peg_1924.nextPosition();
                } else {
                    goto out_peg_1926;
                }
            }
            result_peg_1924.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_1924.setValue(value);
            }
        
        
    }
    
    if (column_peg_1923.chunk9 == 0){
        column_peg_1923.chunk9 = new Chunk9();
    }
    column_peg_1923.chunk9->chunk_unary = result_peg_1924;
    stream.update(result_peg_1924.getPosition());
    
    
    return result_peg_1924;
    out_peg_1926:
    Result result_peg_1927(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1927.getPosition()))){
                    result_peg_1927.nextPosition();
                } else {
                    goto out_peg_1929;
                }
            }
            result_peg_1927.setValue((void*) "-");
        
        
        
        Result result_peg_1932(result_peg_1927);
            {
                
                char letter_peg_1937 = stream.get(result_peg_1932.getPosition());
                if (letter_peg_1937 != '\0' && strchr("0123456789", letter_peg_1937) != NULL){
                    result_peg_1932.nextPosition();
                    result_peg_1932.setValue((void*) (long) letter_peg_1937);
                } else {
                    goto out_peg_1936;
                }
                
            }
            goto success_peg_1933;
            out_peg_1936:
            goto not_peg_1931;
            success_peg_1933:
            ;
            goto out_peg_1929;
            not_peg_1931:
            result_peg_1927.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_1927.setValue(value);
            }
        
        
    }
    
    if (column_peg_1923.chunk9 == 0){
        column_peg_1923.chunk9 = new Chunk9();
    }
    column_peg_1923.chunk9->chunk_unary = result_peg_1927;
    stream.update(result_peg_1927.getPosition());
    
    
    return result_peg_1927;
    out_peg_1929:
    Result result_peg_1938(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_1938.getPosition()))){
                    result_peg_1938.nextPosition();
                } else {
                    goto out_peg_1940;
                }
            }
            result_peg_1938.setValue((void*) "~");
        
        
        
        Result result_peg_1943(result_peg_1938);
            result_peg_1943 = rule_key(stream, result_peg_1943.getPosition());
            if (result_peg_1943.error()){
                goto not_peg_1942;
            }
            goto out_peg_1940;
            not_peg_1942:
            result_peg_1938.setValue((void*)0);
        
        
        
        Result result_peg_1946(result_peg_1938);
            {
                
                char letter_peg_1951 = stream.get(result_peg_1946.getPosition());
                if (letter_peg_1951 != '\0' && strchr("0123456789", letter_peg_1951) != NULL){
                    result_peg_1946.nextPosition();
                    result_peg_1946.setValue((void*) (long) letter_peg_1951);
                } else {
                    goto out_peg_1950;
                }
                
            }
            goto success_peg_1947;
            out_peg_1950:
            goto not_peg_1945;
            success_peg_1947:
            ;
            goto out_peg_1940;
            not_peg_1945:
            result_peg_1938.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_1938.setValue(value);
            }
        
        
    }
    
    if (column_peg_1923.chunk9 == 0){
        column_peg_1923.chunk9 = new Chunk9();
    }
    column_peg_1923.chunk9->chunk_unary = result_peg_1938;
    stream.update(result_peg_1938.getPosition());
    
    
    return result_peg_1938;
    out_peg_1940:
    
    if (column_peg_1923.chunk9 == 0){
        column_peg_1923.chunk9 = new Chunk9();
    }
    column_peg_1923.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_1953 = stream.getColumn(position);
    if (column_peg_1953.chunk9 != 0 && column_peg_1953.chunk9->chunk_expr13_real.calculated()){
        return column_peg_1953.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_1999(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_1954(myposition);
    
    result_peg_1954 = rule_helper(stream, result_peg_1954.getPosition());
    if (result_peg_1954.error()){
        goto out_peg_1955;
    }
    
    if (column_peg_1953.chunk9 == 0){
        column_peg_1953.chunk9 = new Chunk9();
    }
    column_peg_1953.chunk9->chunk_expr13_real = result_peg_1954;
    stream.update(result_peg_1954.getPosition());
    
    
    return result_peg_1954;
    out_peg_1955:
    Result result_peg_1956(myposition);
    
    result_peg_1956 = rule_function(stream, result_peg_1956.getPosition());
    if (result_peg_1956.error()){
        goto out_peg_1957;
    }
    
    if (column_peg_1953.chunk9 == 0){
        column_peg_1953.chunk9 = new Chunk9();
    }
    column_peg_1953.chunk9->chunk_expr13_real = result_peg_1956;
    stream.update(result_peg_1956.getPosition());
    
    
    return result_peg_1956;
    out_peg_1957:
    Result result_peg_1958(myposition);
    
    result_peg_1958 = rule_value(stream, result_peg_1958.getPosition());
    if (result_peg_1958.error()){
        goto out_peg_1959;
    }
    
    if (column_peg_1953.chunk9 == 0){
        column_peg_1953.chunk9 = new Chunk9();
    }
    column_peg_1953.chunk9->chunk_expr13_real = result_peg_1958;
    stream.update(result_peg_1958.getPosition());
    
    
    return result_peg_1958;
    out_peg_1959:
    Result result_peg_1960(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1960.getPosition()))){
                    result_peg_1960.nextPosition();
                } else {
                    goto out_peg_1962;
                }
            }
            result_peg_1960.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1960.reset();
                do{
                    Result result_peg_1974(result_peg_1960.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1974.getPosition()))){
                                result_peg_1974.nextPosition();
                            } else {
                                goto out_peg_1977;
                            }
                        }
                        result_peg_1974.setValue((void*) " ");
                            
                    }
                    goto success_peg_1975;
                    out_peg_1977:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1974.getPosition()))){
                                result_peg_1974.nextPosition();
                            } else {
                                goto out_peg_1979;
                            }
                        }
                        result_peg_1974.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1975;
                    out_peg_1979:
                    goto loop_peg_1973;
                    success_peg_1975:
                    ;
                    result_peg_1960.addResult(result_peg_1974);
                } while (true);
                loop_peg_1973:
                ;
                        
            }
            goto success_peg_1964;
            goto out_peg_1962;
            success_peg_1964:
            ;
        
        
        
        result_peg_1960 = rule_expr(stream, result_peg_1960.getPosition());
            if (result_peg_1960.error()){
                goto out_peg_1962;
            }
            e = result_peg_1960.getValues();
        
        
        
        {
                
                result_peg_1960.reset();
                do{
                    Result result_peg_1992(result_peg_1960.getPosition());
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
                    result_peg_1960.addResult(result_peg_1992);
                } while (true);
                loop_peg_1991:
                ;
                        
            }
            goto success_peg_1982;
            goto out_peg_1962;
            success_peg_1982:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1960.getPosition()))){
                    result_peg_1960.nextPosition();
                } else {
                    goto out_peg_1962;
                }
            }
            result_peg_1960.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_1960.setValue(value);
            }
        
        
    }
    
    if (column_peg_1953.chunk9 == 0){
        column_peg_1953.chunk9 = new Chunk9();
    }
    column_peg_1953.chunk9->chunk_expr13_real = result_peg_1960;
    stream.update(result_peg_1960.getPosition());
    
    
    return result_peg_1960;
    out_peg_1962:
    
    if (column_peg_1953.chunk9 == 0){
        column_peg_1953.chunk9 = new Chunk9();
    }
    column_peg_1953.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_2000 = stream.getColumn(position);
    if (column_peg_2000.chunk9 != 0 && column_peg_2000.chunk9->chunk_function.calculated()){
        return column_peg_2000.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2216(stream, "function");
    int myposition = position;
    
    Value name;
    Value args;
    Value arg1;
    Result result_peg_2001(myposition);
    
    {
    
        result_peg_2001 = rule_function_name(stream, result_peg_2001.getPosition());
            if (result_peg_2001.error()){
                goto out_peg_2003;
            }
            name = result_peg_2001.getValues();
        
        
        
        {
                
                result_peg_2001.reset();
                do{
                    Result result_peg_2015(result_peg_2001.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2015.getPosition()))){
                                result_peg_2015.nextPosition();
                            } else {
                                goto out_peg_2018;
                            }
                        }
                        result_peg_2015.setValue((void*) " ");
                            
                    }
                    goto success_peg_2016;
                    out_peg_2018:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2015.getPosition()))){
                                result_peg_2015.nextPosition();
                            } else {
                                goto out_peg_2020;
                            }
                        }
                        result_peg_2015.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2016;
                    out_peg_2020:
                    goto loop_peg_2014;
                    success_peg_2016:
                    ;
                    result_peg_2001.addResult(result_peg_2015);
                } while (true);
                loop_peg_2014:
                ;
                        
            }
            goto success_peg_2005;
            goto out_peg_2003;
            success_peg_2005:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_2001.getPosition()))){
                    result_peg_2001.nextPosition();
                } else {
                    goto out_peg_2003;
                }
            }
            result_peg_2001.setValue((void*) "(");
        
        
        
        {
                
                result_peg_2001.reset();
                do{
                    Result result_peg_2033(result_peg_2001.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2033.getPosition()))){
                                result_peg_2033.nextPosition();
                            } else {
                                goto out_peg_2036;
                            }
                        }
                        result_peg_2033.setValue((void*) " ");
                            
                    }
                    goto success_peg_2034;
                    out_peg_2036:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2033.getPosition()))){
                                result_peg_2033.nextPosition();
                            } else {
                                goto out_peg_2038;
                            }
                        }
                        result_peg_2033.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2034;
                    out_peg_2038:
                    goto loop_peg_2032;
                    success_peg_2034:
                    ;
                    result_peg_2001.addResult(result_peg_2033);
                } while (true);
                loop_peg_2032:
                ;
                        
            }
            goto success_peg_2023;
            goto out_peg_2003;
            success_peg_2023:
            ;
        
        
        
        result_peg_2001 = rule_args(stream, result_peg_2001.getPosition());
            if (result_peg_2001.error()){
                goto out_peg_2003;
            }
            args = result_peg_2001.getValues();
        
        
        
        {
                
                result_peg_2001.reset();
                do{
                    Result result_peg_2051(result_peg_2001.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2051.getPosition()))){
                                result_peg_2051.nextPosition();
                            } else {
                                goto out_peg_2054;
                            }
                        }
                        result_peg_2051.setValue((void*) " ");
                            
                    }
                    goto success_peg_2052;
                    out_peg_2054:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2051.getPosition()))){
                                result_peg_2051.nextPosition();
                            } else {
                                goto out_peg_2056;
                            }
                        }
                        result_peg_2051.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2052;
                    out_peg_2056:
                    goto loop_peg_2050;
                    success_peg_2052:
                    ;
                    result_peg_2001.addResult(result_peg_2051);
                } while (true);
                loop_peg_2050:
                ;
                        
            }
            goto success_peg_2041;
            goto out_peg_2003;
            success_peg_2041:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_2001.getPosition()))){
                    result_peg_2001.nextPosition();
                } else {
                    goto out_peg_2003;
                }
            }
            result_peg_2001.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, args);
                result_peg_2001.setValue(value);
            }
        
        
    }
    
    if (column_peg_2000.chunk9 == 0){
        column_peg_2000.chunk9 = new Chunk9();
    }
    column_peg_2000.chunk9->chunk_function = result_peg_2001;
    stream.update(result_peg_2001.getPosition());
    
    
    return result_peg_2001;
    out_peg_2003:
    Result result_peg_2058(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("teammode"[i], stream.get(result_peg_2058.getPosition()))){
                    result_peg_2058.nextPosition();
                } else {
                    goto out_peg_2060;
                }
            }
            result_peg_2058.setValue((void*) "teammode");
            name = result_peg_2058.getValues();
        
        
        
        {
                
                result_peg_2058.reset();
                do{
                    Result result_peg_2072(result_peg_2058.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2072.getPosition()))){
                                result_peg_2072.nextPosition();
                            } else {
                                goto out_peg_2075;
                            }
                        }
                        result_peg_2072.setValue((void*) " ");
                            
                    }
                    goto success_peg_2073;
                    out_peg_2075:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2072.getPosition()))){
                                result_peg_2072.nextPosition();
                            } else {
                                goto out_peg_2077;
                            }
                        }
                        result_peg_2072.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2073;
                    out_peg_2077:
                    goto loop_peg_2071;
                    success_peg_2073:
                    ;
                    result_peg_2058.addResult(result_peg_2072);
                } while (true);
                loop_peg_2071:
                ;
                        
            }
            goto success_peg_2062;
            goto out_peg_2060;
            success_peg_2062:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_2058.getPosition()))){
                    result_peg_2058.nextPosition();
                } else {
                    goto out_peg_2060;
                }
            }
            result_peg_2058.setValue((void*) "=");
        
        
        
        {
                
                result_peg_2058.reset();
                do{
                    Result result_peg_2090(result_peg_2058.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2090.getPosition()))){
                                result_peg_2090.nextPosition();
                            } else {
                                goto out_peg_2093;
                            }
                        }
                        result_peg_2090.setValue((void*) " ");
                            
                    }
                    goto success_peg_2091;
                    out_peg_2093:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2090.getPosition()))){
                                result_peg_2090.nextPosition();
                            } else {
                                goto out_peg_2095;
                            }
                        }
                        result_peg_2090.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2091;
                    out_peg_2095:
                    goto loop_peg_2089;
                    success_peg_2091:
                    ;
                    result_peg_2058.addResult(result_peg_2090);
                } while (true);
                loop_peg_2089:
                ;
                        
            }
            goto success_peg_2080;
            goto out_peg_2060;
            success_peg_2080:
            ;
        
        
        
        result_peg_2058 = rule_identifier(stream, result_peg_2058.getPosition());
            if (result_peg_2058.error()){
                goto out_peg_2060;
            }
            arg1 = result_peg_2058.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction1(name, arg1);
                result_peg_2058.setValue(value);
            }
        
        
    }
    
    if (column_peg_2000.chunk9 == 0){
        column_peg_2000.chunk9 = new Chunk9();
    }
    column_peg_2000.chunk9->chunk_function = result_peg_2058;
    stream.update(result_peg_2058.getPosition());
    
    
    return result_peg_2058;
    out_peg_2060:
    Result result_peg_2097(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("teammode"[i], stream.get(result_peg_2097.getPosition()))){
                    result_peg_2097.nextPosition();
                } else {
                    goto out_peg_2099;
                }
            }
            result_peg_2097.setValue((void*) "teammode");
            name = result_peg_2097.getValues();
        
        
        
        {
                
                result_peg_2097.reset();
                do{
                    Result result_peg_2111(result_peg_2097.getPosition());
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
                    result_peg_2097.addResult(result_peg_2111);
                } while (true);
                loop_peg_2110:
                ;
                        
            }
            goto success_peg_2101;
            goto out_peg_2099;
            success_peg_2101:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_2097.getPosition()))){
                    result_peg_2097.nextPosition();
                } else {
                    goto out_peg_2099;
                }
            }
            result_peg_2097.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_2097.reset();
                do{
                    Result result_peg_2129(result_peg_2097.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2129.getPosition()))){
                                result_peg_2129.nextPosition();
                            } else {
                                goto out_peg_2132;
                            }
                        }
                        result_peg_2129.setValue((void*) " ");
                            
                    }
                    goto success_peg_2130;
                    out_peg_2132:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2129.getPosition()))){
                                result_peg_2129.nextPosition();
                            } else {
                                goto out_peg_2134;
                            }
                        }
                        result_peg_2129.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2130;
                    out_peg_2134:
                    goto loop_peg_2128;
                    success_peg_2130:
                    ;
                    result_peg_2097.addResult(result_peg_2129);
                } while (true);
                loop_peg_2128:
                ;
                        
            }
            goto success_peg_2119;
            goto out_peg_2099;
            success_peg_2119:
            ;
        
        
        
        result_peg_2097 = rule_identifier(stream, result_peg_2097.getPosition());
            if (result_peg_2097.error()){
                goto out_peg_2099;
            }
            arg1 = result_peg_2097.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction1("teammode!=", arg1);
                result_peg_2097.setValue(value);
            }
        
        
    }
    
    if (column_peg_2000.chunk9 == 0){
        column_peg_2000.chunk9 = new Chunk9();
    }
    column_peg_2000.chunk9->chunk_function = result_peg_2097;
    stream.update(result_peg_2097.getPosition());
    
    
    return result_peg_2097;
    out_peg_2099:
    Result result_peg_2136(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2136.getPosition()))){
                    result_peg_2136.nextPosition();
                } else {
                    goto out_peg_2138;
                }
            }
            result_peg_2136.setValue((void*) "animelem");
            name = result_peg_2136.getValues();
        
        
        
        {
                
                result_peg_2136.reset();
                do{
                    Result result_peg_2150(result_peg_2136.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2150.getPosition()))){
                                result_peg_2150.nextPosition();
                            } else {
                                goto out_peg_2153;
                            }
                        }
                        result_peg_2150.setValue((void*) " ");
                            
                    }
                    goto success_peg_2151;
                    out_peg_2153:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2150.getPosition()))){
                                result_peg_2150.nextPosition();
                            } else {
                                goto out_peg_2155;
                            }
                        }
                        result_peg_2150.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2151;
                    out_peg_2155:
                    goto loop_peg_2149;
                    success_peg_2151:
                    ;
                    result_peg_2136.addResult(result_peg_2150);
                } while (true);
                loop_peg_2149:
                ;
                        
            }
            goto success_peg_2140;
            goto out_peg_2138;
            success_peg_2140:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_2136.getPosition()))){
                    result_peg_2136.nextPosition();
                } else {
                    goto out_peg_2138;
                }
            }
            result_peg_2136.setValue((void*) "=");
        
        
        
        {
                
                result_peg_2136.reset();
                do{
                    Result result_peg_2168(result_peg_2136.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2168.getPosition()))){
                                result_peg_2168.nextPosition();
                            } else {
                                goto out_peg_2171;
                            }
                        }
                        result_peg_2168.setValue((void*) " ");
                            
                    }
                    goto success_peg_2169;
                    out_peg_2171:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2168.getPosition()))){
                                result_peg_2168.nextPosition();
                            } else {
                                goto out_peg_2173;
                            }
                        }
                        result_peg_2168.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2169;
                    out_peg_2173:
                    goto loop_peg_2167;
                    success_peg_2169:
                    ;
                    result_peg_2136.addResult(result_peg_2168);
                } while (true);
                loop_peg_2167:
                ;
                        
            }
            goto success_peg_2158;
            goto out_peg_2138;
            success_peg_2158:
            ;
        
        
        
        result_peg_2136 = rule_integer(stream, result_peg_2136.getPosition());
            if (result_peg_2136.error()){
                goto out_peg_2138;
            }
            arg1 = result_peg_2136.getValues();
        
        
        
        result_peg_2136 = rule_function_rest(stream, result_peg_2136.getPosition());
            if (result_peg_2136.error()){
                goto out_peg_2138;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1(name, arg1);
                result_peg_2136.setValue(value);
            }
        
        
    }
    
    if (column_peg_2000.chunk9 == 0){
        column_peg_2000.chunk9 = new Chunk9();
    }
    column_peg_2000.chunk9->chunk_function = result_peg_2136;
    stream.update(result_peg_2136.getPosition());
    
    
    return result_peg_2136;
    out_peg_2138:
    Result result_peg_2176(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2176.getPosition()))){
                    result_peg_2176.nextPosition();
                } else {
                    goto out_peg_2178;
                }
            }
            result_peg_2176.setValue((void*) "animelem");
            name = result_peg_2176.getValues();
        
        
        
        {
                
                result_peg_2176.reset();
                do{
                    Result result_peg_2190(result_peg_2176.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2190.getPosition()))){
                                result_peg_2190.nextPosition();
                            } else {
                                goto out_peg_2193;
                            }
                        }
                        result_peg_2190.setValue((void*) " ");
                            
                    }
                    goto success_peg_2191;
                    out_peg_2193:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2190.getPosition()))){
                                result_peg_2190.nextPosition();
                            } else {
                                goto out_peg_2195;
                            }
                        }
                        result_peg_2190.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2191;
                    out_peg_2195:
                    goto loop_peg_2189;
                    success_peg_2191:
                    ;
                    result_peg_2176.addResult(result_peg_2190);
                } while (true);
                loop_peg_2189:
                ;
                        
            }
            goto success_peg_2180;
            goto out_peg_2178;
            success_peg_2180:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_2176.getPosition()))){
                    result_peg_2176.nextPosition();
                } else {
                    goto out_peg_2178;
                }
            }
            result_peg_2176.setValue((void*) "<=");
        
        
        
        {
                
                result_peg_2176.reset();
                do{
                    Result result_peg_2208(result_peg_2176.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2208.getPosition()))){
                                result_peg_2208.nextPosition();
                            } else {
                                goto out_peg_2211;
                            }
                        }
                        result_peg_2208.setValue((void*) " ");
                            
                    }
                    goto success_peg_2209;
                    out_peg_2211:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2208.getPosition()))){
                                result_peg_2208.nextPosition();
                            } else {
                                goto out_peg_2213;
                            }
                        }
                        result_peg_2208.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2209;
                    out_peg_2213:
                    goto loop_peg_2207;
                    success_peg_2209:
                    ;
                    result_peg_2176.addResult(result_peg_2208);
                } while (true);
                loop_peg_2207:
                ;
                        
            }
            goto success_peg_2198;
            goto out_peg_2178;
            success_peg_2198:
            ;
        
        
        
        result_peg_2176 = rule_integer(stream, result_peg_2176.getPosition());
            if (result_peg_2176.error()){
                goto out_peg_2178;
            }
            arg1 = result_peg_2176.getValues();
        
        
        
        result_peg_2176 = rule_function_rest(stream, result_peg_2176.getPosition());
            if (result_peg_2176.error()){
                goto out_peg_2178;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1("animelem<=", arg1);
                result_peg_2176.setValue(value);
            }
        
        
    }
    
    if (column_peg_2000.chunk9 == 0){
        column_peg_2000.chunk9 = new Chunk9();
    }
    column_peg_2000.chunk9->chunk_function = result_peg_2176;
    stream.update(result_peg_2176.getPosition());
    
    
    return result_peg_2176;
    out_peg_2178:
    
    if (column_peg_2000.chunk9 == 0){
        column_peg_2000.chunk9 = new Chunk9();
    }
    column_peg_2000.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2217 = stream.getColumn(position);
    if (column_peg_2217.chunk9 != 0 && column_peg_2217.chunk9->chunk_args.calculated()){
        return column_peg_2217.chunk9->chunk_args;
    }
    
    RuleTrace trace_peg_2259(stream, "args");
    int myposition = position;
    
    Value expr1;
    Value expr_rest;
    Result result_peg_2218(myposition);
    
    {
    
        result_peg_2218 = rule_expr_c(stream, result_peg_2218.getPosition());
            if (result_peg_2218.error()){
                goto out_peg_2220;
            }
            expr1 = result_peg_2218.getValues();
        
        
        
        result_peg_2218.reset();
            do{
                Result result_peg_2223(result_peg_2218.getPosition());
                {
                
                    {
                            
                            result_peg_2223.reset();
                            do{
                                Result result_peg_2235(result_peg_2223.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2235.getPosition()))){
                                            result_peg_2235.nextPosition();
                                        } else {
                                            goto out_peg_2238;
                                        }
                                    }
                                    result_peg_2235.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2236;
                                out_peg_2238:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2235.getPosition()))){
                                            result_peg_2235.nextPosition();
                                        } else {
                                            goto out_peg_2240;
                                        }
                                    }
                                    result_peg_2235.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2236;
                                out_peg_2240:
                                goto loop_peg_2234;
                                success_peg_2236:
                                ;
                                result_peg_2223.addResult(result_peg_2235);
                            } while (true);
                            loop_peg_2234:
                            ;
                                    
                        }
                        goto success_peg_2225;
                        goto loop_peg_2222;
                        success_peg_2225:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2223.getPosition()))){
                                result_peg_2223.nextPosition();
                            } else {
                                goto loop_peg_2222;
                            }
                        }
                        result_peg_2223.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2223.reset();
                            do{
                                Result result_peg_2253(result_peg_2223.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2253.getPosition()))){
                                            result_peg_2253.nextPosition();
                                        } else {
                                            goto out_peg_2256;
                                        }
                                    }
                                    result_peg_2253.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2254;
                                out_peg_2256:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2253.getPosition()))){
                                            result_peg_2253.nextPosition();
                                        } else {
                                            goto out_peg_2258;
                                        }
                                    }
                                    result_peg_2253.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2254;
                                out_peg_2258:
                                goto loop_peg_2252;
                                success_peg_2254:
                                ;
                                result_peg_2223.addResult(result_peg_2253);
                            } while (true);
                            loop_peg_2252:
                            ;
                                    
                        }
                        goto success_peg_2243;
                        goto loop_peg_2222;
                        success_peg_2243:
                        ;
                    
                    
                    
                    result_peg_2223 = rule_expr_c(stream, result_peg_2223.getPosition());
                        if (result_peg_2223.error()){
                            goto loop_peg_2222;
                        }
                    
                    
                }
                result_peg_2218.addResult(result_peg_2223);
            } while (true);
            loop_peg_2222:
            ;
            expr_rest = result_peg_2218.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(expr1, expr_rest);
                result_peg_2218.setValue(value);
            }
        
        
    }
    
    if (column_peg_2217.chunk9 == 0){
        column_peg_2217.chunk9 = new Chunk9();
    }
    column_peg_2217.chunk9->chunk_args = result_peg_2218;
    stream.update(result_peg_2218.getPosition());
    
    
    return result_peg_2218;
    out_peg_2220:
    
    if (column_peg_2217.chunk9 == 0){
        column_peg_2217.chunk9 = new Chunk9();
    }
    column_peg_2217.chunk9->chunk_args = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2260 = stream.getColumn(position);
    if (column_peg_2260.chunk10 != 0 && column_peg_2260.chunk10->chunk_function_name.calculated()){
        return column_peg_2260.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_2325(stream, "function_name");
    int myposition = position;
    
    
    Result result_peg_2261(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("abs"[i], stream.get(result_peg_2261.getPosition()))){
            result_peg_2261.nextPosition();
        } else {
            goto out_peg_2262;
        }
    }
    result_peg_2261.setValue((void*) "abs");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2261;
    stream.update(result_peg_2261.getPosition());
    
    
    return result_peg_2261;
    out_peg_2262:
    Result result_peg_2263(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("const"[i], stream.get(result_peg_2263.getPosition()))){
            result_peg_2263.nextPosition();
        } else {
            goto out_peg_2264;
        }
    }
    result_peg_2263.setValue((void*) "const");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2263;
    stream.update(result_peg_2263.getPosition());
    
    
    return result_peg_2263;
    out_peg_2264:
    Result result_peg_2265(myposition);
    
    for (int i = 0; i < 13; i++){
        if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2265.getPosition()))){
            result_peg_2265.nextPosition();
        } else {
            goto out_peg_2266;
        }
    }
    result_peg_2265.setValue((void*) "selfanimexist");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2265;
    stream.update(result_peg_2265.getPosition());
    
    
    return result_peg_2265;
    out_peg_2266:
    Result result_peg_2267(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("ifelse"[i], stream.get(result_peg_2267.getPosition()))){
            result_peg_2267.nextPosition();
        } else {
            goto out_peg_2268;
        }
    }
    result_peg_2267.setValue((void*) "ifelse");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2267;
    stream.update(result_peg_2267.getPosition());
    
    
    return result_peg_2267;
    out_peg_2268:
    Result result_peg_2269(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("gethitvar"[i], stream.get(result_peg_2269.getPosition()))){
            result_peg_2269.nextPosition();
        } else {
            goto out_peg_2270;
        }
    }
    result_peg_2269.setValue((void*) "gethitvar");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2269;
    stream.update(result_peg_2269.getPosition());
    
    
    return result_peg_2269;
    out_peg_2270:
    Result result_peg_2271(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("floor"[i], stream.get(result_peg_2271.getPosition()))){
            result_peg_2271.nextPosition();
        } else {
            goto out_peg_2272;
        }
    }
    result_peg_2271.setValue((void*) "floor");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2271;
    stream.update(result_peg_2271.getPosition());
    
    
    return result_peg_2271;
    out_peg_2272:
    Result result_peg_2273(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("ceil"[i], stream.get(result_peg_2273.getPosition()))){
            result_peg_2273.nextPosition();
        } else {
            goto out_peg_2274;
        }
    }
    result_peg_2273.setValue((void*) "ceil");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2273;
    stream.update(result_peg_2273.getPosition());
    
    
    return result_peg_2273;
    out_peg_2274:
    Result result_peg_2275(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("acos"[i], stream.get(result_peg_2275.getPosition()))){
            result_peg_2275.nextPosition();
        } else {
            goto out_peg_2276;
        }
    }
    result_peg_2275.setValue((void*) "acos");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2275;
    stream.update(result_peg_2275.getPosition());
    
    
    return result_peg_2275;
    out_peg_2276:
    Result result_peg_2277(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("asin"[i], stream.get(result_peg_2277.getPosition()))){
            result_peg_2277.nextPosition();
        } else {
            goto out_peg_2278;
        }
    }
    result_peg_2277.setValue((void*) "asin");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2277;
    stream.update(result_peg_2277.getPosition());
    
    
    return result_peg_2277;
    out_peg_2278:
    Result result_peg_2279(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("atan"[i], stream.get(result_peg_2279.getPosition()))){
            result_peg_2279.nextPosition();
        } else {
            goto out_peg_2280;
        }
    }
    result_peg_2279.setValue((void*) "atan");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2279;
    stream.update(result_peg_2279.getPosition());
    
    
    return result_peg_2279;
    out_peg_2280:
    Result result_peg_2281(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("tan"[i], stream.get(result_peg_2281.getPosition()))){
            result_peg_2281.nextPosition();
        } else {
            goto out_peg_2282;
        }
    }
    result_peg_2281.setValue((void*) "tan");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2281;
    stream.update(result_peg_2281.getPosition());
    
    
    return result_peg_2281;
    out_peg_2282:
    Result result_peg_2283(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("cos"[i], stream.get(result_peg_2283.getPosition()))){
            result_peg_2283.nextPosition();
        } else {
            goto out_peg_2284;
        }
    }
    result_peg_2283.setValue((void*) "cos");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2283;
    stream.update(result_peg_2283.getPosition());
    
    
    return result_peg_2283;
    out_peg_2284:
    Result result_peg_2285(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("sin"[i], stream.get(result_peg_2285.getPosition()))){
            result_peg_2285.nextPosition();
        } else {
            goto out_peg_2286;
        }
    }
    result_peg_2285.setValue((void*) "sin");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2285;
    stream.update(result_peg_2285.getPosition());
    
    
    return result_peg_2285;
    out_peg_2286:
    Result result_peg_2287(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("log"[i], stream.get(result_peg_2287.getPosition()))){
            result_peg_2287.nextPosition();
        } else {
            goto out_peg_2288;
        }
    }
    result_peg_2287.setValue((void*) "log");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2287;
    stream.update(result_peg_2287.getPosition());
    
    
    return result_peg_2287;
    out_peg_2288:
    Result result_peg_2289(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("ln"[i], stream.get(result_peg_2289.getPosition()))){
            result_peg_2289.nextPosition();
        } else {
            goto out_peg_2290;
        }
    }
    result_peg_2289.setValue((void*) "ln");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2289;
    stream.update(result_peg_2289.getPosition());
    
    
    return result_peg_2289;
    out_peg_2290:
    Result result_peg_2291(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("sysfvar"[i], stream.get(result_peg_2291.getPosition()))){
            result_peg_2291.nextPosition();
        } else {
            goto out_peg_2292;
        }
    }
    result_peg_2291.setValue((void*) "sysfvar");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2291;
    stream.update(result_peg_2291.getPosition());
    
    
    return result_peg_2291;
    out_peg_2292:
    Result result_peg_2293(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("sysvar"[i], stream.get(result_peg_2293.getPosition()))){
            result_peg_2293.nextPosition();
        } else {
            goto out_peg_2294;
        }
    }
    result_peg_2293.setValue((void*) "sysvar");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2293;
    stream.update(result_peg_2293.getPosition());
    
    
    return result_peg_2293;
    out_peg_2294:
    Result result_peg_2295(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("var"[i], stream.get(result_peg_2295.getPosition()))){
            result_peg_2295.nextPosition();
        } else {
            goto out_peg_2296;
        }
    }
    result_peg_2295.setValue((void*) "var");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2295;
    stream.update(result_peg_2295.getPosition());
    
    
    return result_peg_2295;
    out_peg_2296:
    Result result_peg_2297(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numexplod"[i], stream.get(result_peg_2297.getPosition()))){
            result_peg_2297.nextPosition();
        } else {
            goto out_peg_2298;
        }
    }
    result_peg_2297.setValue((void*) "numexplod");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2297;
    stream.update(result_peg_2297.getPosition());
    
    
    return result_peg_2297;
    out_peg_2298:
    Result result_peg_2299(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2299.getPosition()))){
            result_peg_2299.nextPosition();
        } else {
            goto out_peg_2300;
        }
    }
    result_peg_2299.setValue((void*) "numhelper");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2299;
    stream.update(result_peg_2299.getPosition());
    
    
    return result_peg_2299;
    out_peg_2300:
    Result result_peg_2301(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numprojid"[i], stream.get(result_peg_2301.getPosition()))){
            result_peg_2301.nextPosition();
        } else {
            goto out_peg_2302;
        }
    }
    result_peg_2301.setValue((void*) "numprojid");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2301;
    stream.update(result_peg_2301.getPosition());
    
    
    return result_peg_2301;
    out_peg_2302:
    Result result_peg_2303(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("fvar"[i], stream.get(result_peg_2303.getPosition()))){
            result_peg_2303.nextPosition();
        } else {
            goto out_peg_2304;
        }
    }
    result_peg_2303.setValue((void*) "fvar");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2303;
    stream.update(result_peg_2303.getPosition());
    
    
    return result_peg_2303;
    out_peg_2304:
    Result result_peg_2305(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("ishelper"[i], stream.get(result_peg_2305.getPosition()))){
            result_peg_2305.nextPosition();
        } else {
            goto out_peg_2306;
        }
    }
    result_peg_2305.setValue((void*) "ishelper");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2305;
    stream.update(result_peg_2305.getPosition());
    
    
    return result_peg_2305;
    out_peg_2306:
    Result result_peg_2307(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numtarget"[i], stream.get(result_peg_2307.getPosition()))){
            result_peg_2307.nextPosition();
        } else {
            goto out_peg_2308;
        }
    }
    result_peg_2307.setValue((void*) "numtarget");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2307;
    stream.update(result_peg_2307.getPosition());
    
    
    return result_peg_2307;
    out_peg_2308:
    Result result_peg_2309(myposition);
    
    for (int i = 0; i < 12; i++){
        if (compareCharCase("animelemtime"[i], stream.get(result_peg_2309.getPosition()))){
            result_peg_2309.nextPosition();
        } else {
            goto out_peg_2310;
        }
    }
    result_peg_2309.setValue((void*) "animelemtime");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2309;
    stream.update(result_peg_2309.getPosition());
    
    
    return result_peg_2309;
    out_peg_2310:
    Result result_peg_2311(myposition);
    
    for (int i = 0; i < 10; i++){
        if (compareCharCase("animelemno"[i], stream.get(result_peg_2311.getPosition()))){
            result_peg_2311.nextPosition();
        } else {
            goto out_peg_2312;
        }
    }
    result_peg_2311.setValue((void*) "animelemno");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2311;
    stream.update(result_peg_2311.getPosition());
    
    
    return result_peg_2311;
    out_peg_2312:
    Result result_peg_2313(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("animexist"[i], stream.get(result_peg_2313.getPosition()))){
            result_peg_2313.nextPosition();
        } else {
            goto out_peg_2314;
        }
    }
    result_peg_2313.setValue((void*) "animexist");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2313;
    stream.update(result_peg_2313.getPosition());
    
    
    return result_peg_2313;
    out_peg_2314:
    Result result_peg_2315(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projguarded"[i], stream.get(result_peg_2315.getPosition()))){
            result_peg_2315.nextPosition();
        } else {
            goto out_peg_2316;
        }
    }
    result_peg_2315.setValue((void*) "projguarded");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2315;
    stream.update(result_peg_2315.getPosition());
    
    
    return result_peg_2315;
    out_peg_2316:
    Result result_peg_2317(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projhittime"[i], stream.get(result_peg_2317.getPosition()))){
            result_peg_2317.nextPosition();
        } else {
            goto out_peg_2318;
        }
    }
    result_peg_2317.setValue((void*) "projhittime");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2317;
    stream.update(result_peg_2317.getPosition());
    
    
    return result_peg_2317;
    out_peg_2318:
    Result result_peg_2319(myposition);
    
    for (int i = 0; i < 15; i++){
        if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2319.getPosition()))){
            result_peg_2319.nextPosition();
        } else {
            goto out_peg_2320;
        }
    }
    result_peg_2319.setValue((void*) "projcontacttime");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2319;
    stream.update(result_peg_2319.getPosition());
    
    
    return result_peg_2319;
    out_peg_2320:
    Result result_peg_2321(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projcontact"[i], stream.get(result_peg_2321.getPosition()))){
            result_peg_2321.nextPosition();
        } else {
            goto out_peg_2322;
        }
    }
    result_peg_2321.setValue((void*) "projcontact");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2321;
    stream.update(result_peg_2321.getPosition());
    
    
    return result_peg_2321;
    out_peg_2322:
    Result result_peg_2323(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2323.getPosition()))){
            result_peg_2323.nextPosition();
        } else {
            goto out_peg_2324;
        }
    }
    result_peg_2323.setValue((void*) "numhelper");
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = result_peg_2323;
    stream.update(result_peg_2323.getPosition());
    
    
    return result_peg_2323;
    out_peg_2324:
    
    if (column_peg_2260.chunk10 == 0){
        column_peg_2260.chunk10 = new Chunk10();
    }
    column_peg_2260.chunk10->chunk_function_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2326 = stream.getColumn(position);
    if (column_peg_2326.chunk10 != 0 && column_peg_2326.chunk10->chunk_function_rest.calculated()){
        return column_peg_2326.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2384(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_2327(myposition);
    
    {
    
        {
                
                result_peg_2327.reset();
                do{
                    Result result_peg_2339(result_peg_2327.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2339.getPosition()))){
                                result_peg_2339.nextPosition();
                            } else {
                                goto out_peg_2342;
                            }
                        }
                        result_peg_2339.setValue((void*) " ");
                            
                    }
                    goto success_peg_2340;
                    out_peg_2342:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2339.getPosition()))){
                                result_peg_2339.nextPosition();
                            } else {
                                goto out_peg_2344;
                            }
                        }
                        result_peg_2339.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2340;
                    out_peg_2344:
                    goto loop_peg_2338;
                    success_peg_2340:
                    ;
                    result_peg_2327.addResult(result_peg_2339);
                } while (true);
                loop_peg_2338:
                ;
                        
            }
            goto success_peg_2329;
            goto out_peg_2345;
            success_peg_2329:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2327.getPosition()))){
                    result_peg_2327.nextPosition();
                } else {
                    goto out_peg_2345;
                }
            }
            result_peg_2327.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2327.reset();
                do{
                    Result result_peg_2358(result_peg_2327.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2358.getPosition()))){
                                result_peg_2358.nextPosition();
                            } else {
                                goto out_peg_2361;
                            }
                        }
                        result_peg_2358.setValue((void*) " ");
                            
                    }
                    goto success_peg_2359;
                    out_peg_2361:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2358.getPosition()))){
                                result_peg_2358.nextPosition();
                            } else {
                                goto out_peg_2363;
                            }
                        }
                        result_peg_2358.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2359;
                    out_peg_2363:
                    goto loop_peg_2357;
                    success_peg_2359:
                    ;
                    result_peg_2327.addResult(result_peg_2358);
                } while (true);
                loop_peg_2357:
                ;
                        
            }
            goto success_peg_2348;
            goto out_peg_2345;
            success_peg_2348:
            ;
        
        
        
        int save_peg_2365 = result_peg_2327.getPosition();
            
            result_peg_2327 = rule_all_compare(stream, result_peg_2327.getPosition());
            if (result_peg_2327.error()){
                
                result_peg_2327 = Result(save_peg_2365);
                result_peg_2327.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2327.reset();
                do{
                    Result result_peg_2377(result_peg_2327.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2377.getPosition()))){
                                result_peg_2377.nextPosition();
                            } else {
                                goto out_peg_2380;
                            }
                        }
                        result_peg_2377.setValue((void*) " ");
                            
                    }
                    goto success_peg_2378;
                    out_peg_2380:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2377.getPosition()))){
                                result_peg_2377.nextPosition();
                            } else {
                                goto out_peg_2382;
                            }
                        }
                        result_peg_2377.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2378;
                    out_peg_2382:
                    goto loop_peg_2376;
                    success_peg_2378:
                    ;
                    result_peg_2327.addResult(result_peg_2377);
                } while (true);
                loop_peg_2376:
                ;
                        
            }
            goto success_peg_2367;
            goto out_peg_2345;
            success_peg_2367:
            ;
        
        
        
        result_peg_2327 = rule_value(stream, result_peg_2327.getPosition());
            if (result_peg_2327.error()){
                goto out_peg_2345;
            }
        
        
    }
    
    if (column_peg_2326.chunk10 == 0){
        column_peg_2326.chunk10 = new Chunk10();
    }
    column_peg_2326.chunk10->chunk_function_rest = result_peg_2327;
    stream.update(result_peg_2327.getPosition());
    
    
    return result_peg_2327;
    out_peg_2345:
    Result result_peg_2383(myposition);
    
    
    
    if (column_peg_2326.chunk10 == 0){
        column_peg_2326.chunk10 = new Chunk10();
    }
    column_peg_2326.chunk10->chunk_function_rest = result_peg_2383;
    stream.update(result_peg_2383.getPosition());
    
    
    return result_peg_2383;
    
    if (column_peg_2326.chunk10 == 0){
        column_peg_2326.chunk10 = new Chunk10();
    }
    column_peg_2326.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2385 = stream.getColumn(position);
    if (column_peg_2385.chunk10 != 0 && column_peg_2385.chunk10->chunk_keys.calculated()){
        return column_peg_2385.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_2388(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_2386(myposition);
    
    result_peg_2386 = rule_key_value_list(stream, result_peg_2386.getPosition());
    if (result_peg_2386.error()){
        goto out_peg_2387;
    }
    
    if (column_peg_2385.chunk10 == 0){
        column_peg_2385.chunk10 = new Chunk10();
    }
    column_peg_2385.chunk10->chunk_keys = result_peg_2386;
    stream.update(result_peg_2386.getPosition());
    
    
    return result_peg_2386;
    out_peg_2387:
    
    if (column_peg_2385.chunk10 == 0){
        column_peg_2385.chunk10 = new Chunk10();
    }
    column_peg_2385.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2389 = stream.getColumn(position);
    if (column_peg_2389.chunk10 != 0 && column_peg_2389.chunk10->chunk_key_value_list.calculated()){
        return column_peg_2389.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2431(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_2390(myposition);
    
    {
    
        result_peg_2390 = rule_key(stream, result_peg_2390.getPosition());
            if (result_peg_2390.error()){
                goto out_peg_2392;
            }
            first = result_peg_2390.getValues();
        
        
        
        result_peg_2390.reset();
            do{
                Result result_peg_2395(result_peg_2390.getPosition());
                {
                
                    {
                            
                            result_peg_2395.reset();
                            do{
                                Result result_peg_2407(result_peg_2395.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2407.getPosition()))){
                                            result_peg_2407.nextPosition();
                                        } else {
                                            goto out_peg_2410;
                                        }
                                    }
                                    result_peg_2407.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2408;
                                out_peg_2410:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2407.getPosition()))){
                                            result_peg_2407.nextPosition();
                                        } else {
                                            goto out_peg_2412;
                                        }
                                    }
                                    result_peg_2407.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2408;
                                out_peg_2412:
                                goto loop_peg_2406;
                                success_peg_2408:
                                ;
                                result_peg_2395.addResult(result_peg_2407);
                            } while (true);
                            loop_peg_2406:
                            ;
                                    
                        }
                        goto success_peg_2397;
                        goto loop_peg_2394;
                        success_peg_2397:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2395.getPosition()))){
                                result_peg_2395.nextPosition();
                            } else {
                                goto loop_peg_2394;
                            }
                        }
                        result_peg_2395.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2395.reset();
                            do{
                                Result result_peg_2425(result_peg_2395.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2425.getPosition()))){
                                            result_peg_2425.nextPosition();
                                        } else {
                                            goto out_peg_2428;
                                        }
                                    }
                                    result_peg_2425.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2426;
                                out_peg_2428:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2425.getPosition()))){
                                            result_peg_2425.nextPosition();
                                        } else {
                                            goto out_peg_2430;
                                        }
                                    }
                                    result_peg_2425.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2426;
                                out_peg_2430:
                                goto loop_peg_2424;
                                success_peg_2426:
                                ;
                                result_peg_2395.addResult(result_peg_2425);
                            } while (true);
                            loop_peg_2424:
                            ;
                                    
                        }
                        goto success_peg_2415;
                        goto loop_peg_2394;
                        success_peg_2415:
                        ;
                    
                    
                    
                    result_peg_2395 = rule_key(stream, result_peg_2395.getPosition());
                        if (result_peg_2395.error()){
                            goto loop_peg_2394;
                        }
                    
                    
                }
                result_peg_2390.addResult(result_peg_2395);
            } while (true);
            loop_peg_2394:
            ;
            rest = result_peg_2390.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_2390.setValue(value);
            }
        
        
    }
    
    if (column_peg_2389.chunk10 == 0){
        column_peg_2389.chunk10 = new Chunk10();
    }
    column_peg_2389.chunk10->chunk_key_value_list = result_peg_2390;
    stream.update(result_peg_2390.getPosition());
    
    
    return result_peg_2390;
    out_peg_2392:
    
    if (column_peg_2389.chunk10 == 0){
        column_peg_2389.chunk10 = new Chunk10();
    }
    column_peg_2389.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2432 = stream.getColumn(position);
    if (column_peg_2432.chunk10 != 0 && column_peg_2432.chunk10->chunk_key.calculated()){
        return column_peg_2432.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_2440(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_2433(myposition);
    
    {
    
        result_peg_2433 = rule_key_real(stream, result_peg_2433.getPosition());
            if (result_peg_2433.error()){
                goto out_peg_2435;
            }
        
        Result result_peg_2434 = result_peg_2433;
        
        result_peg_2433 = rule_key_rest(stream, result_peg_2433.getPosition(), result_peg_2434.getValues());
            if (result_peg_2433.error()){
                goto out_peg_2435;
            }
            ok = result_peg_2433.getValues();
        
        
        
        Result result_peg_2439(result_peg_2433);
            result_peg_2439 = rule_identifier(stream, result_peg_2439.getPosition());
            if (result_peg_2439.error()){
                goto not_peg_2438;
            }
            goto out_peg_2435;
            not_peg_2438:
            result_peg_2433.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_2433.setValue(value);
            }
        
        
    }
    
    if (column_peg_2432.chunk10 == 0){
        column_peg_2432.chunk10 = new Chunk10();
    }
    column_peg_2432.chunk10->chunk_key = result_peg_2433;
    stream.update(result_peg_2433.getPosition());
    
    
    return result_peg_2433;
    out_peg_2435:
    
    if (column_peg_2432.chunk10 == 0){
        column_peg_2432.chunk10 = new Chunk10();
    }
    column_peg_2432.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2484(stream, "key_rest");
    int myposition = position;
    tail_peg_2443:
    Value another;
    Value new_left;
    Result result_peg_2442(myposition);
    {
    
        {
                
                result_peg_2442.reset();
                do{
                    Result result_peg_2455(result_peg_2442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2455.getPosition()))){
                                result_peg_2455.nextPosition();
                            } else {
                                goto out_peg_2458;
                            }
                        }
                        result_peg_2455.setValue((void*) " ");
                            
                    }
                    goto success_peg_2456;
                    out_peg_2458:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2455.getPosition()))){
                                result_peg_2455.nextPosition();
                            } else {
                                goto out_peg_2460;
                            }
                        }
                        result_peg_2455.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2456;
                    out_peg_2460:
                    goto loop_peg_2454;
                    success_peg_2456:
                    ;
                    result_peg_2442.addResult(result_peg_2455);
                } while (true);
                loop_peg_2454:
                ;
                        
            }
            goto success_peg_2445;
            goto out_peg_2461;
            success_peg_2445:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_2442.getPosition()))){
                    result_peg_2442.nextPosition();
                } else {
                    goto out_peg_2461;
                }
            }
            result_peg_2442.setValue((void*) "+");
        
        
        
        {
                
                result_peg_2442.reset();
                do{
                    Result result_peg_2474(result_peg_2442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2474.getPosition()))){
                                result_peg_2474.nextPosition();
                            } else {
                                goto out_peg_2477;
                            }
                        }
                        result_peg_2474.setValue((void*) " ");
                            
                    }
                    goto success_peg_2475;
                    out_peg_2477:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2474.getPosition()))){
                                result_peg_2474.nextPosition();
                            } else {
                                goto out_peg_2479;
                            }
                        }
                        result_peg_2474.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2475;
                    out_peg_2479:
                    goto loop_peg_2473;
                    success_peg_2475:
                    ;
                    result_peg_2442.addResult(result_peg_2474);
                } while (true);
                loop_peg_2473:
                ;
                        
            }
            goto success_peg_2464;
            goto out_peg_2461;
            success_peg_2464:
            ;
        
        
        
        result_peg_2442 = rule_key_real(stream, result_peg_2442.getPosition());
            if (result_peg_2442.error()){
                goto out_peg_2461;
            }
            another = result_peg_2442.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_2442.setValue(value);
            }
            new_left = result_peg_2442.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_2442.getPosition();
    goto tail_peg_2443;
    out_peg_2461:
    Result result_peg_2482(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_2482.setValue(value);
            }
        
        
    }
    stream.update(result_peg_2482.getPosition());
    
    return result_peg_2482;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2485 = stream.getColumn(position);
    if (column_peg_2485.chunk11 != 0 && column_peg_2485.chunk11->chunk_key_real.calculated()){
        return column_peg_2485.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_2492(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_2486(myposition);
    
    {
    
        result_peg_2486.reset();
            do{
                Result result_peg_2489(result_peg_2486.getPosition());
                result_peg_2489 = rule_key_modifier(stream, result_peg_2489.getPosition());
                if (result_peg_2489.error()){
                    goto loop_peg_2488;
                }
                result_peg_2486.addResult(result_peg_2489);
            } while (true);
            loop_peg_2488:
            ;
            mods = result_peg_2486.getValues();
        
        
        
        result_peg_2486 = rule_key_name(stream, result_peg_2486.getPosition());
            if (result_peg_2486.error()){
                goto out_peg_2491;
            }
            name = result_peg_2486.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_2486.setValue(value);
            }
        
        
    }
    
    if (column_peg_2485.chunk11 == 0){
        column_peg_2485.chunk11 = new Chunk11();
    }
    column_peg_2485.chunk11->chunk_key_real = result_peg_2486;
    stream.update(result_peg_2486.getPosition());
    
    
    return result_peg_2486;
    out_peg_2491:
    
    if (column_peg_2485.chunk11 == 0){
        column_peg_2485.chunk11 = new Chunk11();
    }
    column_peg_2485.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2493 = stream.getColumn(position);
    if (column_peg_2493.chunk11 != 0 && column_peg_2493.chunk11->chunk_key_modifier.calculated()){
        return column_peg_2493.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2514(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_2494(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_2494.getPosition()))){
                    result_peg_2494.nextPosition();
                } else {
                    goto out_peg_2496;
                }
            }
            result_peg_2494.setValue((void*) "~");
        
        
        
        result_peg_2494.reset();
            do{
                Result result_peg_2499(result_peg_2494.getPosition());
                {
                    
                    char letter_peg_2504 = stream.get(result_peg_2499.getPosition());
                    if (letter_peg_2504 != '\0' && strchr("0123456789", letter_peg_2504) != NULL){
                        result_peg_2499.nextPosition();
                        result_peg_2499.setValue((void*) (long) letter_peg_2504);
                    } else {
                        goto out_peg_2503;
                    }
                    
                }
                goto success_peg_2500;
                out_peg_2503:
                goto loop_peg_2498;
                success_peg_2500:
                ;
                result_peg_2494.addResult(result_peg_2499);
            } while (true);
            loop_peg_2498:
            ;
            num = result_peg_2494.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_2494.setValue(value);
            }
        
        
    }
    
    if (column_peg_2493.chunk11 == 0){
        column_peg_2493.chunk11 = new Chunk11();
    }
    column_peg_2493.chunk11->chunk_key_modifier = result_peg_2494;
    stream.update(result_peg_2494.getPosition());
    
    
    return result_peg_2494;
    out_peg_2496:
    Result result_peg_2505(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_2505.getPosition()))){
                    result_peg_2505.nextPosition();
                } else {
                    goto out_peg_2507;
                }
            }
            result_peg_2505.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_2505.setValue(value);
            }
        
        
    }
    
    if (column_peg_2493.chunk11 == 0){
        column_peg_2493.chunk11 = new Chunk11();
    }
    column_peg_2493.chunk11->chunk_key_modifier = result_peg_2505;
    stream.update(result_peg_2505.getPosition());
    
    
    return result_peg_2505;
    out_peg_2507:
    Result result_peg_2508(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_2508.getPosition()))){
                    result_peg_2508.nextPosition();
                } else {
                    goto out_peg_2510;
                }
            }
            result_peg_2508.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_2508.setValue(value);
            }
        
        
    }
    
    if (column_peg_2493.chunk11 == 0){
        column_peg_2493.chunk11 = new Chunk11();
    }
    column_peg_2493.chunk11->chunk_key_modifier = result_peg_2508;
    stream.update(result_peg_2508.getPosition());
    
    
    return result_peg_2508;
    out_peg_2510:
    Result result_peg_2511(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_2511.getPosition()))){
                    result_peg_2511.nextPosition();
                } else {
                    goto out_peg_2513;
                }
            }
            result_peg_2511.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_2511.setValue(value);
            }
        
        
    }
    
    if (column_peg_2493.chunk11 == 0){
        column_peg_2493.chunk11 = new Chunk11();
    }
    column_peg_2493.chunk11->chunk_key_modifier = result_peg_2511;
    stream.update(result_peg_2511.getPosition());
    
    
    return result_peg_2511;
    out_peg_2513:
    
    if (column_peg_2493.chunk11 == 0){
        column_peg_2493.chunk11 = new Chunk11();
    }
    column_peg_2493.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2515 = stream.getColumn(position);
    if (column_peg_2515.chunk11 != 0 && column_peg_2515.chunk11->chunk_key_name.calculated()){
        return column_peg_2515.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_2546(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_2516(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_2516.getPosition()))){
            result_peg_2516.nextPosition();
        } else {
            goto out_peg_2517;
        }
    }
    result_peg_2516.setValue((void*) "DB");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2516;
    stream.update(result_peg_2516.getPosition());
    
    
    return result_peg_2516;
    out_peg_2517:
    Result result_peg_2518(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_2518.getPosition()))){
            result_peg_2518.nextPosition();
        } else {
            goto out_peg_2519;
        }
    }
    result_peg_2518.setValue((void*) "B");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2518;
    stream.update(result_peg_2518.getPosition());
    
    
    return result_peg_2518;
    out_peg_2519:
    Result result_peg_2520(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_2520.getPosition()))){
            result_peg_2520.nextPosition();
        } else {
            goto out_peg_2521;
        }
    }
    result_peg_2520.setValue((void*) "DF");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2520;
    stream.update(result_peg_2520.getPosition());
    
    
    return result_peg_2520;
    out_peg_2521:
    Result result_peg_2522(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_2522.getPosition()))){
            result_peg_2522.nextPosition();
        } else {
            goto out_peg_2523;
        }
    }
    result_peg_2522.setValue((void*) "D");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2522;
    stream.update(result_peg_2522.getPosition());
    
    
    return result_peg_2522;
    out_peg_2523:
    Result result_peg_2524(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_2524.getPosition()))){
            result_peg_2524.nextPosition();
        } else {
            goto out_peg_2525;
        }
    }
    result_peg_2524.setValue((void*) "F");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2524;
    stream.update(result_peg_2524.getPosition());
    
    
    return result_peg_2524;
    out_peg_2525:
    Result result_peg_2526(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_2526.getPosition()))){
            result_peg_2526.nextPosition();
        } else {
            goto out_peg_2527;
        }
    }
    result_peg_2526.setValue((void*) "UF");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2526;
    stream.update(result_peg_2526.getPosition());
    
    
    return result_peg_2526;
    out_peg_2527:
    Result result_peg_2528(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_2528.getPosition()))){
            result_peg_2528.nextPosition();
        } else {
            goto out_peg_2529;
        }
    }
    result_peg_2528.setValue((void*) "UB");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2528;
    stream.update(result_peg_2528.getPosition());
    
    
    return result_peg_2528;
    out_peg_2529:
    Result result_peg_2530(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_2530.getPosition()))){
            result_peg_2530.nextPosition();
        } else {
            goto out_peg_2531;
        }
    }
    result_peg_2530.setValue((void*) "U");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2530;
    stream.update(result_peg_2530.getPosition());
    
    
    return result_peg_2530;
    out_peg_2531:
    Result result_peg_2532(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_2532.getPosition()))){
            result_peg_2532.nextPosition();
        } else {
            goto out_peg_2533;
        }
    }
    result_peg_2532.setValue((void*) "a");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2532;
    stream.update(result_peg_2532.getPosition());
    
    
    return result_peg_2532;
    out_peg_2533:
    Result result_peg_2534(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_2534.getPosition()))){
            result_peg_2534.nextPosition();
        } else {
            goto out_peg_2535;
        }
    }
    result_peg_2534.setValue((void*) "b");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2534;
    stream.update(result_peg_2534.getPosition());
    
    
    return result_peg_2534;
    out_peg_2535:
    Result result_peg_2536(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_2536.getPosition()))){
            result_peg_2536.nextPosition();
        } else {
            goto out_peg_2537;
        }
    }
    result_peg_2536.setValue((void*) "c");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2536;
    stream.update(result_peg_2536.getPosition());
    
    
    return result_peg_2536;
    out_peg_2537:
    Result result_peg_2538(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_2538.getPosition()))){
            result_peg_2538.nextPosition();
        } else {
            goto out_peg_2539;
        }
    }
    result_peg_2538.setValue((void*) "x");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2538;
    stream.update(result_peg_2538.getPosition());
    
    
    return result_peg_2538;
    out_peg_2539:
    Result result_peg_2540(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_2540.getPosition()))){
            result_peg_2540.nextPosition();
        } else {
            goto out_peg_2541;
        }
    }
    result_peg_2540.setValue((void*) "y");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2540;
    stream.update(result_peg_2540.getPosition());
    
    
    return result_peg_2540;
    out_peg_2541:
    Result result_peg_2542(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_2542.getPosition()))){
            result_peg_2542.nextPosition();
        } else {
            goto out_peg_2543;
        }
    }
    result_peg_2542.setValue((void*) "z");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2542;
    stream.update(result_peg_2542.getPosition());
    
    
    return result_peg_2542;
    out_peg_2543:
    Result result_peg_2544(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_2544.getPosition()))){
            result_peg_2544.nextPosition();
        } else {
            goto out_peg_2545;
        }
    }
    result_peg_2544.setValue((void*) "s");
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = result_peg_2544;
    stream.update(result_peg_2544.getPosition());
    
    
    return result_peg_2544;
    out_peg_2545:
    
    if (column_peg_2515.chunk11 == 0){
        column_peg_2515.chunk11 = new Chunk11();
    }
    column_peg_2515.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2547 = stream.getColumn(position);
    if (column_peg_2547.chunk11 != 0 && column_peg_2547.chunk11->chunk_value.calculated()){
        return column_peg_2547.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_2565(stream, "value");
    int myposition = position;
    
    
    Result result_peg_2548(myposition);
    
    result_peg_2548 = rule_float(stream, result_peg_2548.getPosition());
    if (result_peg_2548.error()){
        goto out_peg_2549;
    }
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = result_peg_2548;
    stream.update(result_peg_2548.getPosition());
    
    
    return result_peg_2548;
    out_peg_2549:
    Result result_peg_2550(myposition);
    
    result_peg_2550 = rule_integer(stream, result_peg_2550.getPosition());
    if (result_peg_2550.error()){
        goto out_peg_2551;
    }
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = result_peg_2550;
    stream.update(result_peg_2550.getPosition());
    
    
    return result_peg_2550;
    out_peg_2551:
    Result result_peg_2552(myposition);
    
    result_peg_2552 = rule_keyword(stream, result_peg_2552.getPosition());
    if (result_peg_2552.error()){
        goto out_peg_2553;
    }
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = result_peg_2552;
    stream.update(result_peg_2552.getPosition());
    
    
    return result_peg_2552;
    out_peg_2553:
    Result result_peg_2554(myposition);
    
    {
    
        Result result_peg_2557(result_peg_2554);
            result_peg_2557 = rule_keyword(stream, result_peg_2557.getPosition());
            if (result_peg_2557.error()){
                goto not_peg_2556;
            }
            goto out_peg_2558;
            not_peg_2556:
            result_peg_2554.setValue((void*)0);
        
        
        
        result_peg_2554 = rule_identifier(stream, result_peg_2554.getPosition());
            if (result_peg_2554.error()){
                goto out_peg_2558;
            }
        
        
    }
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = result_peg_2554;
    stream.update(result_peg_2554.getPosition());
    
    
    return result_peg_2554;
    out_peg_2558:
    Result result_peg_2559(myposition);
    
    result_peg_2559 = rule_range(stream, result_peg_2559.getPosition());
    if (result_peg_2559.error()){
        goto out_peg_2560;
    }
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = result_peg_2559;
    stream.update(result_peg_2559.getPosition());
    
    
    return result_peg_2559;
    out_peg_2560:
    Result result_peg_2561(myposition);
    
    result_peg_2561 = rule_string(stream, result_peg_2561.getPosition());
    if (result_peg_2561.error()){
        goto out_peg_2562;
    }
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = result_peg_2561;
    stream.update(result_peg_2561.getPosition());
    
    
    return result_peg_2561;
    out_peg_2562:
    Result result_peg_2563(myposition);
    
    result_peg_2563 = rule_hitflag(stream, result_peg_2563.getPosition());
    if (result_peg_2563.error()){
        goto out_peg_2564;
    }
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = result_peg_2563;
    stream.update(result_peg_2563.getPosition());
    
    
    return result_peg_2563;
    out_peg_2564:
    
    if (column_peg_2547.chunk11 == 0){
        column_peg_2547.chunk11 = new Chunk11();
    }
    column_peg_2547.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2566 = stream.getColumn(position);
    if (column_peg_2566.chunk12 != 0 && column_peg_2566.chunk12->chunk_helper.calculated()){
        return column_peg_2566.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_2625(stream, "helper");
    int myposition = position;
    
    Value name;
    Value id;
    Result result_peg_2567(myposition);
    
    {
    
        result_peg_2567 = rule_helper__name(stream, result_peg_2567.getPosition());
            if (result_peg_2567.error()){
                goto out_peg_2569;
            }
            name = result_peg_2567.getValues();
        
        
        
        {
                
                result_peg_2567.reset();
                do{
                    Result result_peg_2581(result_peg_2567.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2581.getPosition()))){
                                result_peg_2581.nextPosition();
                            } else {
                                goto out_peg_2584;
                            }
                        }
                        result_peg_2581.setValue((void*) " ");
                            
                    }
                    goto success_peg_2582;
                    out_peg_2584:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2581.getPosition()))){
                                result_peg_2581.nextPosition();
                            } else {
                                goto out_peg_2586;
                            }
                        }
                        result_peg_2581.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2582;
                    out_peg_2586:
                    goto loop_peg_2580;
                    success_peg_2582:
                    ;
                    result_peg_2567.addResult(result_peg_2581);
                } while (true);
                loop_peg_2580:
                ;
                        
            }
            goto success_peg_2571;
            goto out_peg_2569;
            success_peg_2571:
            ;
        
        
        
        int save_peg_2588 = result_peg_2567.getPosition();
            
            result_peg_2567 = rule_helper__expression(stream, result_peg_2567.getPosition());
            if (result_peg_2567.error()){
                
                result_peg_2567 = Result(save_peg_2588);
                result_peg_2567.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2567.reset();
                do{
                    Result result_peg_2600(result_peg_2567.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2600.getPosition()))){
                                result_peg_2600.nextPosition();
                            } else {
                                goto out_peg_2603;
                            }
                        }
                        result_peg_2600.setValue((void*) " ");
                            
                    }
                    goto success_peg_2601;
                    out_peg_2603:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2600.getPosition()))){
                                result_peg_2600.nextPosition();
                            } else {
                                goto out_peg_2605;
                            }
                        }
                        result_peg_2600.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2601;
                    out_peg_2605:
                    goto loop_peg_2599;
                    success_peg_2601:
                    ;
                    result_peg_2567.addResult(result_peg_2600);
                } while (true);
                loop_peg_2599:
                ;
                        
            }
            goto success_peg_2590;
            goto out_peg_2569;
            success_peg_2590:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2567.getPosition()))){
                    result_peg_2567.nextPosition();
                } else {
                    goto out_peg_2569;
                }
            }
            result_peg_2567.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2567.reset();
                do{
                    Result result_peg_2618(result_peg_2567.getPosition());
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
                    result_peg_2567.addResult(result_peg_2618);
                } while (true);
                loop_peg_2617:
                ;
                        
            }
            goto success_peg_2608;
            goto out_peg_2569;
            success_peg_2608:
            ;
        
        
        
        result_peg_2567 = rule_helper__identifier(stream, result_peg_2567.getPosition());
            if (result_peg_2567.error()){
                goto out_peg_2569;
            }
            id = result_peg_2567.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeHelper(name, id);
                result_peg_2567.setValue(value);
            }
        
        
    }
    
    if (column_peg_2566.chunk12 == 0){
        column_peg_2566.chunk12 = new Chunk12();
    }
    column_peg_2566.chunk12->chunk_helper = result_peg_2567;
    stream.update(result_peg_2567.getPosition());
    
    
    return result_peg_2567;
    out_peg_2569:
    
    if (column_peg_2566.chunk12 == 0){
        column_peg_2566.chunk12 = new Chunk12();
    }
    column_peg_2566.chunk12->chunk_helper = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_2626 = stream.getColumn(position);
    if (column_peg_2626.chunk12 != 0 && column_peg_2626.chunk12->chunk_helper__expression.calculated()){
        return column_peg_2626.chunk12->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_2665(stream, "helper__expression");
    int myposition = position;
    
    
    Result result_peg_2627(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_2627.getPosition()))){
                    result_peg_2627.nextPosition();
                } else {
                    goto out_peg_2629;
                }
            }
            result_peg_2627.setValue((void*) "(");
        
        
        
        {
                
                result_peg_2627.reset();
                do{
                    Result result_peg_2641(result_peg_2627.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2641.getPosition()))){
                                result_peg_2641.nextPosition();
                            } else {
                                goto out_peg_2644;
                            }
                        }
                        result_peg_2641.setValue((void*) " ");
                            
                    }
                    goto success_peg_2642;
                    out_peg_2644:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2641.getPosition()))){
                                result_peg_2641.nextPosition();
                            } else {
                                goto out_peg_2646;
                            }
                        }
                        result_peg_2641.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2642;
                    out_peg_2646:
                    goto loop_peg_2640;
                    success_peg_2642:
                    ;
                    result_peg_2627.addResult(result_peg_2641);
                } while (true);
                loop_peg_2640:
                ;
                        
            }
            goto success_peg_2631;
            goto out_peg_2629;
            success_peg_2631:
            ;
        
        
        
        result_peg_2627 = rule_expr_c(stream, result_peg_2627.getPosition());
            if (result_peg_2627.error()){
                goto out_peg_2629;
            }
        
        
        
        {
                
                result_peg_2627.reset();
                do{
                    Result result_peg_2659(result_peg_2627.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2659.getPosition()))){
                                result_peg_2659.nextPosition();
                            } else {
                                goto out_peg_2662;
                            }
                        }
                        result_peg_2659.setValue((void*) " ");
                            
                    }
                    goto success_peg_2660;
                    out_peg_2662:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2659.getPosition()))){
                                result_peg_2659.nextPosition();
                            } else {
                                goto out_peg_2664;
                            }
                        }
                        result_peg_2659.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2660;
                    out_peg_2664:
                    goto loop_peg_2658;
                    success_peg_2660:
                    ;
                    result_peg_2627.addResult(result_peg_2659);
                } while (true);
                loop_peg_2658:
                ;
                        
            }
            goto success_peg_2649;
            goto out_peg_2629;
            success_peg_2649:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_2627.getPosition()))){
                    result_peg_2627.nextPosition();
                } else {
                    goto out_peg_2629;
                }
            }
            result_peg_2627.setValue((void*) ")");
        
        
    }
    
    if (column_peg_2626.chunk12 == 0){
        column_peg_2626.chunk12 = new Chunk12();
    }
    column_peg_2626.chunk12->chunk_helper__expression = result_peg_2627;
    stream.update(result_peg_2627.getPosition());
    
    
    return result_peg_2627;
    out_peg_2629:
    
    if (column_peg_2626.chunk12 == 0){
        column_peg_2626.chunk12 = new Chunk12();
    }
    column_peg_2626.chunk12->chunk_helper__expression = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_2666 = stream.getColumn(position);
    if (column_peg_2666.chunk12 != 0 && column_peg_2666.chunk12->chunk_helper__name.calculated()){
        return column_peg_2666.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_2683(stream, "helper__name");
    int myposition = position;
    
    
    Result result_peg_2667(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("parent"[i], stream.get(result_peg_2667.getPosition()))){
            result_peg_2667.nextPosition();
        } else {
            goto out_peg_2668;
        }
    }
    result_peg_2667.setValue((void*) "parent");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2667;
    stream.update(result_peg_2667.getPosition());
    
    
    return result_peg_2667;
    out_peg_2668:
    Result result_peg_2669(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("root"[i], stream.get(result_peg_2669.getPosition()))){
            result_peg_2669.nextPosition();
        } else {
            goto out_peg_2670;
        }
    }
    result_peg_2669.setValue((void*) "root");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2669;
    stream.update(result_peg_2669.getPosition());
    
    
    return result_peg_2669;
    out_peg_2670:
    Result result_peg_2671(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("helper"[i], stream.get(result_peg_2671.getPosition()))){
            result_peg_2671.nextPosition();
        } else {
            goto out_peg_2672;
        }
    }
    result_peg_2671.setValue((void*) "helper");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2671;
    stream.update(result_peg_2671.getPosition());
    
    
    return result_peg_2671;
    out_peg_2672:
    Result result_peg_2673(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("target"[i], stream.get(result_peg_2673.getPosition()))){
            result_peg_2673.nextPosition();
        } else {
            goto out_peg_2674;
        }
    }
    result_peg_2673.setValue((void*) "target");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2673;
    stream.update(result_peg_2673.getPosition());
    
    
    return result_peg_2673;
    out_peg_2674:
    Result result_peg_2675(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("partner"[i], stream.get(result_peg_2675.getPosition()))){
            result_peg_2675.nextPosition();
        } else {
            goto out_peg_2676;
        }
    }
    result_peg_2675.setValue((void*) "partner");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2675;
    stream.update(result_peg_2675.getPosition());
    
    
    return result_peg_2675;
    out_peg_2676:
    Result result_peg_2677(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("enemyNear"[i], stream.get(result_peg_2677.getPosition()))){
            result_peg_2677.nextPosition();
        } else {
            goto out_peg_2678;
        }
    }
    result_peg_2677.setValue((void*) "enemyNear");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2677;
    stream.update(result_peg_2677.getPosition());
    
    
    return result_peg_2677;
    out_peg_2678:
    Result result_peg_2679(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("enemy"[i], stream.get(result_peg_2679.getPosition()))){
            result_peg_2679.nextPosition();
        } else {
            goto out_peg_2680;
        }
    }
    result_peg_2679.setValue((void*) "enemy");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2679;
    stream.update(result_peg_2679.getPosition());
    
    
    return result_peg_2679;
    out_peg_2680:
    Result result_peg_2681(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("playerid"[i], stream.get(result_peg_2681.getPosition()))){
            result_peg_2681.nextPosition();
        } else {
            goto out_peg_2682;
        }
    }
    result_peg_2681.setValue((void*) "playerid");
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = result_peg_2681;
    stream.update(result_peg_2681.getPosition());
    
    
    return result_peg_2681;
    out_peg_2682:
    
    if (column_peg_2666.chunk12 == 0){
        column_peg_2666.chunk12 = new Chunk12();
    }
    column_peg_2666.chunk12->chunk_helper__name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_2684 = stream.getColumn(position);
    if (column_peg_2684.chunk12 != 0 && column_peg_2684.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_2684.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_2694(stream, "helper__identifier");
    int myposition = position;
    
    
    Result result_peg_2685(myposition);
    
    result_peg_2685 = rule_function(stream, result_peg_2685.getPosition());
    if (result_peg_2685.error()){
        goto out_peg_2686;
    }
    
    if (column_peg_2684.chunk12 == 0){
        column_peg_2684.chunk12 = new Chunk12();
    }
    column_peg_2684.chunk12->chunk_helper__identifier = result_peg_2685;
    stream.update(result_peg_2685.getPosition());
    
    
    return result_peg_2685;
    out_peg_2686:
    Result result_peg_2687(myposition);
    
    result_peg_2687 = rule_keyword(stream, result_peg_2687.getPosition());
    if (result_peg_2687.error()){
        goto out_peg_2688;
    }
    
    if (column_peg_2684.chunk12 == 0){
        column_peg_2684.chunk12 = new Chunk12();
    }
    column_peg_2684.chunk12->chunk_helper__identifier = result_peg_2687;
    stream.update(result_peg_2687.getPosition());
    
    
    return result_peg_2687;
    out_peg_2688:
    Result result_peg_2689(myposition);
    
    {
    
        Result result_peg_2692(result_peg_2689);
            result_peg_2692 = rule_keyword(stream, result_peg_2692.getPosition());
            if (result_peg_2692.error()){
                goto not_peg_2691;
            }
            goto out_peg_2693;
            not_peg_2691:
            result_peg_2689.setValue((void*)0);
        
        
        
        result_peg_2689 = rule_identifier(stream, result_peg_2689.getPosition());
            if (result_peg_2689.error()){
                goto out_peg_2693;
            }
        
        
    }
    
    if (column_peg_2684.chunk12 == 0){
        column_peg_2684.chunk12 = new Chunk12();
    }
    column_peg_2684.chunk12->chunk_helper__identifier = result_peg_2689;
    stream.update(result_peg_2689.getPosition());
    
    
    return result_peg_2689;
    out_peg_2693:
    
    if (column_peg_2684.chunk12 == 0){
        column_peg_2684.chunk12 = new Chunk12();
    }
    column_peg_2684.chunk12->chunk_helper__identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_2695 = stream.getColumn(position);
    if (column_peg_2695.chunk12 != 0 && column_peg_2695.chunk12->chunk_hitflag.calculated()){
        return column_peg_2695.chunk12->chunk_hitflag;
    }
    
    RuleTrace trace_peg_2717(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_2696(myposition);
    
    {
    
        result_peg_2696.reset();
            do{
                Result result_peg_2699(result_peg_2696.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("A"[i], stream.get(result_peg_2699.getPosition()))){
                            result_peg_2699.nextPosition();
                        } else {
                            goto out_peg_2702;
                        }
                    }
                    result_peg_2699.setValue((void*) "A");
                        
                }
                goto success_peg_2700;
                out_peg_2702:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("M"[i], stream.get(result_peg_2699.getPosition()))){
                            result_peg_2699.nextPosition();
                        } else {
                            goto out_peg_2704;
                        }
                    }
                    result_peg_2699.setValue((void*) "M");
                        
                }
                goto success_peg_2700;
                out_peg_2704:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("F"[i], stream.get(result_peg_2699.getPosition()))){
                            result_peg_2699.nextPosition();
                        } else {
                            goto out_peg_2706;
                        }
                    }
                    result_peg_2699.setValue((void*) "F");
                        
                }
                goto success_peg_2700;
                out_peg_2706:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("D"[i], stream.get(result_peg_2699.getPosition()))){
                            result_peg_2699.nextPosition();
                        } else {
                            goto out_peg_2708;
                        }
                    }
                    result_peg_2699.setValue((void*) "D");
                        
                }
                goto success_peg_2700;
                out_peg_2708:
                goto loop_peg_2698;
                success_peg_2700:
                ;
                result_peg_2696.addResult(result_peg_2699);
            } while (true);
            loop_peg_2698:
            if (result_peg_2696.matches() == 0){
                goto out_peg_2709;
            }
        
        
        
        int save_peg_2711 = result_peg_2696.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2696.getPosition()))){
                        result_peg_2696.nextPosition();
                    } else {
                        goto out_peg_2714;
                    }
                }
                result_peg_2696.setValue((void*) "+");
                    
            }
            goto success_peg_2712;
            out_peg_2714:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2696.getPosition()))){
                        result_peg_2696.nextPosition();
                    } else {
                        goto out_peg_2716;
                    }
                }
                result_peg_2696.setValue((void*) "-");
                    
            }
            goto success_peg_2712;
            out_peg_2716:
            
            result_peg_2696 = Result(save_peg_2711);
            result_peg_2696.setValue((void*) 0);
            
            success_peg_2712:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = makeSimpleIdentifier("A");
                result_peg_2696.setValue(value);
            }
        
        
    }
    
    if (column_peg_2695.chunk12 == 0){
        column_peg_2695.chunk12 = new Chunk12();
    }
    column_peg_2695.chunk12->chunk_hitflag = result_peg_2696;
    stream.update(result_peg_2696.getPosition());
    
    
    return result_peg_2696;
    out_peg_2709:
    
    if (column_peg_2695.chunk12 == 0){
        column_peg_2695.chunk12 = new Chunk12();
    }
    column_peg_2695.chunk12->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_2718 = stream.getColumn(position);
    if (column_peg_2718.chunk13 != 0 && column_peg_2718.chunk13->chunk_keyword.calculated()){
        return column_peg_2718.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_2725(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_2719(myposition);
    
    {
    
        result_peg_2719 = rule_keyword_real(stream, result_peg_2719.getPosition());
            if (result_peg_2719.error()){
                goto out_peg_2721;
            }
        
        Result result_peg_2720 = result_peg_2719;
        
        Result result_peg_2724(result_peg_2719);
            result_peg_2724 = rule_alpha_digit(stream, result_peg_2724.getPosition());
            if (result_peg_2724.error()){
                goto not_peg_2723;
            }
            goto out_peg_2721;
            not_peg_2723:
            result_peg_2719.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_2720.getValues();
                result_peg_2719.setValue(value);
            }
        
        
    }
    
    if (column_peg_2718.chunk13 == 0){
        column_peg_2718.chunk13 = new Chunk13();
    }
    column_peg_2718.chunk13->chunk_keyword = result_peg_2719;
    stream.update(result_peg_2719.getPosition());
    
    
    return result_peg_2719;
    out_peg_2721:
    
    if (column_peg_2718.chunk13 == 0){
        column_peg_2718.chunk13 = new Chunk13();
    }
    column_peg_2718.chunk13->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_2726 = stream.getColumn(position);
    if (column_peg_2726.chunk13 != 0 && column_peg_2726.chunk13->chunk_keyword_real.calculated()){
        return column_peg_2726.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_3128(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_2727(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2727.getPosition()))){
                    result_peg_2727.nextPosition();
                } else {
                    goto out_peg_2729;
                }
            }
            result_peg_2727.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2727.reset();
                do{
                    Result result_peg_2741(result_peg_2727.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2741.getPosition()))){
                                result_peg_2741.nextPosition();
                            } else {
                                goto out_peg_2744;
                            }
                        }
                        result_peg_2741.setValue((void*) " ");
                            
                    }
                    goto success_peg_2742;
                    out_peg_2744:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2741.getPosition()))){
                                result_peg_2741.nextPosition();
                            } else {
                                goto out_peg_2746;
                            }
                        }
                        result_peg_2741.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2742;
                    out_peg_2746:
                    goto loop_peg_2740;
                    success_peg_2742:
                    ;
                    result_peg_2727.addResult(result_peg_2741);
                } while (true);
                loop_peg_2740:
                ;
                        
            }
            goto success_peg_2731;
            goto out_peg_2729;
            success_peg_2731:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2727.getPosition()))){
                    result_peg_2727.nextPosition();
                } else {
                    goto out_peg_2729;
                }
            }
            result_peg_2727.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_2727.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2727;
    stream.update(result_peg_2727.getPosition());
    
    
    return result_peg_2727;
    out_peg_2729:
    Result result_peg_2748(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2748.getPosition()))){
                    result_peg_2748.nextPosition();
                } else {
                    goto out_peg_2750;
                }
            }
            result_peg_2748.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2748.reset();
                do{
                    Result result_peg_2762(result_peg_2748.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2762.getPosition()))){
                                result_peg_2762.nextPosition();
                            } else {
                                goto out_peg_2765;
                            }
                        }
                        result_peg_2762.setValue((void*) " ");
                            
                    }
                    goto success_peg_2763;
                    out_peg_2765:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2762.getPosition()))){
                                result_peg_2762.nextPosition();
                            } else {
                                goto out_peg_2767;
                            }
                        }
                        result_peg_2762.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2763;
                    out_peg_2767:
                    goto loop_peg_2761;
                    success_peg_2763:
                    ;
                    result_peg_2748.addResult(result_peg_2762);
                } while (true);
                loop_peg_2761:
                ;
                        
            }
            goto success_peg_2752;
            goto out_peg_2750;
            success_peg_2752:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2748.getPosition()))){
                    result_peg_2748.nextPosition();
                } else {
                    goto out_peg_2750;
                }
            }
            result_peg_2748.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_2748.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2748;
    stream.update(result_peg_2748.getPosition());
    
    
    return result_peg_2748;
    out_peg_2750:
    Result result_peg_2769(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2769.getPosition()))){
                    result_peg_2769.nextPosition();
                } else {
                    goto out_peg_2771;
                }
            }
            result_peg_2769.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2769.reset();
                do{
                    Result result_peg_2783(result_peg_2769.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2783.getPosition()))){
                                result_peg_2783.nextPosition();
                            } else {
                                goto out_peg_2786;
                            }
                        }
                        result_peg_2783.setValue((void*) " ");
                            
                    }
                    goto success_peg_2784;
                    out_peg_2786:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2783.getPosition()))){
                                result_peg_2783.nextPosition();
                            } else {
                                goto out_peg_2788;
                            }
                        }
                        result_peg_2783.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2784;
                    out_peg_2788:
                    goto loop_peg_2782;
                    success_peg_2784:
                    ;
                    result_peg_2769.addResult(result_peg_2783);
                } while (true);
                loop_peg_2782:
                ;
                        
            }
            goto success_peg_2773;
            goto out_peg_2771;
            success_peg_2773:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2769.getPosition()))){
                    result_peg_2769.nextPosition();
                } else {
                    goto out_peg_2771;
                }
            }
            result_peg_2769.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_2769.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2769;
    stream.update(result_peg_2769.getPosition());
    
    
    return result_peg_2769;
    out_peg_2771:
    Result result_peg_2790(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2790.getPosition()))){
                    result_peg_2790.nextPosition();
                } else {
                    goto out_peg_2792;
                }
            }
            result_peg_2790.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2790.reset();
                do{
                    Result result_peg_2804(result_peg_2790.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2804.getPosition()))){
                                result_peg_2804.nextPosition();
                            } else {
                                goto out_peg_2807;
                            }
                        }
                        result_peg_2804.setValue((void*) " ");
                            
                    }
                    goto success_peg_2805;
                    out_peg_2807:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2804.getPosition()))){
                                result_peg_2804.nextPosition();
                            } else {
                                goto out_peg_2809;
                            }
                        }
                        result_peg_2804.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2805;
                    out_peg_2809:
                    goto loop_peg_2803;
                    success_peg_2805:
                    ;
                    result_peg_2790.addResult(result_peg_2804);
                } while (true);
                loop_peg_2803:
                ;
                        
            }
            goto success_peg_2794;
            goto out_peg_2792;
            success_peg_2794:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2790.getPosition()))){
                    result_peg_2790.nextPosition();
                } else {
                    goto out_peg_2792;
                }
            }
            result_peg_2790.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_2790.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2790;
    stream.update(result_peg_2790.getPosition());
    
    
    return result_peg_2790;
    out_peg_2792:
    Result result_peg_2811(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2811.getPosition()))){
                    result_peg_2811.nextPosition();
                } else {
                    goto out_peg_2813;
                }
            }
            result_peg_2811.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2811.reset();
                do{
                    Result result_peg_2825(result_peg_2811.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2825.getPosition()))){
                                result_peg_2825.nextPosition();
                            } else {
                                goto out_peg_2828;
                            }
                        }
                        result_peg_2825.setValue((void*) " ");
                            
                    }
                    goto success_peg_2826;
                    out_peg_2828:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2825.getPosition()))){
                                result_peg_2825.nextPosition();
                            } else {
                                goto out_peg_2830;
                            }
                        }
                        result_peg_2825.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2826;
                    out_peg_2830:
                    goto loop_peg_2824;
                    success_peg_2826:
                    ;
                    result_peg_2811.addResult(result_peg_2825);
                } while (true);
                loop_peg_2824:
                ;
                        
            }
            goto success_peg_2815;
            goto out_peg_2813;
            success_peg_2815:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2811.getPosition()))){
                    result_peg_2811.nextPosition();
                } else {
                    goto out_peg_2813;
                }
            }
            result_peg_2811.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_2811.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2811;
    stream.update(result_peg_2811.getPosition());
    
    
    return result_peg_2811;
    out_peg_2813:
    Result result_peg_2832(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2832.getPosition()))){
                    result_peg_2832.nextPosition();
                } else {
                    goto out_peg_2834;
                }
            }
            result_peg_2832.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2832.reset();
                do{
                    Result result_peg_2846(result_peg_2832.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2846.getPosition()))){
                                result_peg_2846.nextPosition();
                            } else {
                                goto out_peg_2849;
                            }
                        }
                        result_peg_2846.setValue((void*) " ");
                            
                    }
                    goto success_peg_2847;
                    out_peg_2849:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2846.getPosition()))){
                                result_peg_2846.nextPosition();
                            } else {
                                goto out_peg_2851;
                            }
                        }
                        result_peg_2846.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2847;
                    out_peg_2851:
                    goto loop_peg_2845;
                    success_peg_2847:
                    ;
                    result_peg_2832.addResult(result_peg_2846);
                } while (true);
                loop_peg_2845:
                ;
                        
            }
            goto success_peg_2836;
            goto out_peg_2834;
            success_peg_2836:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2832.getPosition()))){
                    result_peg_2832.nextPosition();
                } else {
                    goto out_peg_2834;
                }
            }
            result_peg_2832.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_2832.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2832;
    stream.update(result_peg_2832.getPosition());
    
    
    return result_peg_2832;
    out_peg_2834:
    Result result_peg_2853(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2853.getPosition()))){
                    result_peg_2853.nextPosition();
                } else {
                    goto out_peg_2855;
                }
            }
            result_peg_2853.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2853.reset();
                do{
                    Result result_peg_2867(result_peg_2853.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2867.getPosition()))){
                                result_peg_2867.nextPosition();
                            } else {
                                goto out_peg_2870;
                            }
                        }
                        result_peg_2867.setValue((void*) " ");
                            
                    }
                    goto success_peg_2868;
                    out_peg_2870:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2867.getPosition()))){
                                result_peg_2867.nextPosition();
                            } else {
                                goto out_peg_2872;
                            }
                        }
                        result_peg_2867.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2868;
                    out_peg_2872:
                    goto loop_peg_2866;
                    success_peg_2868:
                    ;
                    result_peg_2853.addResult(result_peg_2867);
                } while (true);
                loop_peg_2866:
                ;
                        
            }
            goto success_peg_2857;
            goto out_peg_2855;
            success_peg_2857:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2853.getPosition()))){
                    result_peg_2853.nextPosition();
                } else {
                    goto out_peg_2855;
                }
            }
            result_peg_2853.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_2853.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2853;
    stream.update(result_peg_2853.getPosition());
    
    
    return result_peg_2853;
    out_peg_2855:
    Result result_peg_2874(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2874.getPosition()))){
                    result_peg_2874.nextPosition();
                } else {
                    goto out_peg_2876;
                }
            }
            result_peg_2874.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2874.reset();
                do{
                    Result result_peg_2888(result_peg_2874.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2888.getPosition()))){
                                result_peg_2888.nextPosition();
                            } else {
                                goto out_peg_2891;
                            }
                        }
                        result_peg_2888.setValue((void*) " ");
                            
                    }
                    goto success_peg_2889;
                    out_peg_2891:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2888.getPosition()))){
                                result_peg_2888.nextPosition();
                            } else {
                                goto out_peg_2893;
                            }
                        }
                        result_peg_2888.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2889;
                    out_peg_2893:
                    goto loop_peg_2887;
                    success_peg_2889:
                    ;
                    result_peg_2874.addResult(result_peg_2888);
                } while (true);
                loop_peg_2887:
                ;
                        
            }
            goto success_peg_2878;
            goto out_peg_2876;
            success_peg_2878:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2874.getPosition()))){
                    result_peg_2874.nextPosition();
                } else {
                    goto out_peg_2876;
                }
            }
            result_peg_2874.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_2874.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2874;
    stream.update(result_peg_2874.getPosition());
    
    
    return result_peg_2874;
    out_peg_2876:
    Result result_peg_2895(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2895.getPosition()))){
                    result_peg_2895.nextPosition();
                } else {
                    goto out_peg_2897;
                }
            }
            result_peg_2895.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2895.reset();
                do{
                    Result result_peg_2909(result_peg_2895.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2909.getPosition()))){
                                result_peg_2909.nextPosition();
                            } else {
                                goto out_peg_2912;
                            }
                        }
                        result_peg_2909.setValue((void*) " ");
                            
                    }
                    goto success_peg_2910;
                    out_peg_2912:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2909.getPosition()))){
                                result_peg_2909.nextPosition();
                            } else {
                                goto out_peg_2914;
                            }
                        }
                        result_peg_2909.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2910;
                    out_peg_2914:
                    goto loop_peg_2908;
                    success_peg_2910:
                    ;
                    result_peg_2895.addResult(result_peg_2909);
                } while (true);
                loop_peg_2908:
                ;
                        
            }
            goto success_peg_2899;
            goto out_peg_2897;
            success_peg_2899:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2895.getPosition()))){
                    result_peg_2895.nextPosition();
                } else {
                    goto out_peg_2897;
                }
            }
            result_peg_2895.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_2895.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2895;
    stream.update(result_peg_2895.getPosition());
    
    
    return result_peg_2895;
    out_peg_2897:
    Result result_peg_2916(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2916.getPosition()))){
                    result_peg_2916.nextPosition();
                } else {
                    goto out_peg_2918;
                }
            }
            result_peg_2916.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2916.reset();
                do{
                    Result result_peg_2930(result_peg_2916.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2930.getPosition()))){
                                result_peg_2930.nextPosition();
                            } else {
                                goto out_peg_2933;
                            }
                        }
                        result_peg_2930.setValue((void*) " ");
                            
                    }
                    goto success_peg_2931;
                    out_peg_2933:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2930.getPosition()))){
                                result_peg_2930.nextPosition();
                            } else {
                                goto out_peg_2935;
                            }
                        }
                        result_peg_2930.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2931;
                    out_peg_2935:
                    goto loop_peg_2929;
                    success_peg_2931:
                    ;
                    result_peg_2916.addResult(result_peg_2930);
                } while (true);
                loop_peg_2929:
                ;
                        
            }
            goto success_peg_2920;
            goto out_peg_2918;
            success_peg_2920:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2916.getPosition()))){
                    result_peg_2916.nextPosition();
                } else {
                    goto out_peg_2918;
                }
            }
            result_peg_2916.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_2916.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2916;
    stream.update(result_peg_2916.getPosition());
    
    
    return result_peg_2916;
    out_peg_2918:
    Result result_peg_2937(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2937.getPosition()))){
                    result_peg_2937.nextPosition();
                } else {
                    goto out_peg_2939;
                }
            }
            result_peg_2937.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2937.reset();
                do{
                    Result result_peg_2951(result_peg_2937.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2951.getPosition()))){
                                result_peg_2951.nextPosition();
                            } else {
                                goto out_peg_2954;
                            }
                        }
                        result_peg_2951.setValue((void*) " ");
                            
                    }
                    goto success_peg_2952;
                    out_peg_2954:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2951.getPosition()))){
                                result_peg_2951.nextPosition();
                            } else {
                                goto out_peg_2956;
                            }
                        }
                        result_peg_2951.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2952;
                    out_peg_2956:
                    goto loop_peg_2950;
                    success_peg_2952:
                    ;
                    result_peg_2937.addResult(result_peg_2951);
                } while (true);
                loop_peg_2950:
                ;
                        
            }
            goto success_peg_2941;
            goto out_peg_2939;
            success_peg_2941:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2937.getPosition()))){
                    result_peg_2937.nextPosition();
                } else {
                    goto out_peg_2939;
                }
            }
            result_peg_2937.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_2937.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2937;
    stream.update(result_peg_2937.getPosition());
    
    
    return result_peg_2937;
    out_peg_2939:
    Result result_peg_2958(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2958.getPosition()))){
                    result_peg_2958.nextPosition();
                } else {
                    goto out_peg_2960;
                }
            }
            result_peg_2958.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2958.reset();
                do{
                    Result result_peg_2972(result_peg_2958.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2972.getPosition()))){
                                result_peg_2972.nextPosition();
                            } else {
                                goto out_peg_2975;
                            }
                        }
                        result_peg_2972.setValue((void*) " ");
                            
                    }
                    goto success_peg_2973;
                    out_peg_2975:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2972.getPosition()))){
                                result_peg_2972.nextPosition();
                            } else {
                                goto out_peg_2977;
                            }
                        }
                        result_peg_2972.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2973;
                    out_peg_2977:
                    goto loop_peg_2971;
                    success_peg_2973:
                    ;
                    result_peg_2958.addResult(result_peg_2972);
                } while (true);
                loop_peg_2971:
                ;
                        
            }
            goto success_peg_2962;
            goto out_peg_2960;
            success_peg_2962:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2958.getPosition()))){
                    result_peg_2958.nextPosition();
                } else {
                    goto out_peg_2960;
                }
            }
            result_peg_2958.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_2958.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2958;
    stream.update(result_peg_2958.getPosition());
    
    
    return result_peg_2958;
    out_peg_2960:
    Result result_peg_2979(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_2979.getPosition()))){
                    result_peg_2979.nextPosition();
                } else {
                    goto out_peg_2981;
                }
            }
            result_peg_2979.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_2979.reset();
                do{
                    Result result_peg_2993(result_peg_2979.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2993.getPosition()))){
                                result_peg_2993.nextPosition();
                            } else {
                                goto out_peg_2996;
                            }
                        }
                        result_peg_2993.setValue((void*) " ");
                            
                    }
                    goto success_peg_2994;
                    out_peg_2996:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2993.getPosition()))){
                                result_peg_2993.nextPosition();
                            } else {
                                goto out_peg_2998;
                            }
                        }
                        result_peg_2993.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2994;
                    out_peg_2998:
                    goto loop_peg_2992;
                    success_peg_2994:
                    ;
                    result_peg_2979.addResult(result_peg_2993);
                } while (true);
                loop_peg_2992:
                ;
                        
            }
            goto success_peg_2983;
            goto out_peg_2981;
            success_peg_2983:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2979.getPosition()))){
                    result_peg_2979.nextPosition();
                } else {
                    goto out_peg_2981;
                }
            }
            result_peg_2979.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_2979.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_2979;
    stream.update(result_peg_2979.getPosition());
    
    
    return result_peg_2979;
    out_peg_2981:
    Result result_peg_3000(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_3000.getPosition()))){
                    result_peg_3000.nextPosition();
                } else {
                    goto out_peg_3002;
                }
            }
            result_peg_3000.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_3000.reset();
                do{
                    Result result_peg_3014(result_peg_3000.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3014.getPosition()))){
                                result_peg_3014.nextPosition();
                            } else {
                                goto out_peg_3017;
                            }
                        }
                        result_peg_3014.setValue((void*) " ");
                            
                    }
                    goto success_peg_3015;
                    out_peg_3017:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3014.getPosition()))){
                                result_peg_3014.nextPosition();
                            } else {
                                goto out_peg_3019;
                            }
                        }
                        result_peg_3014.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3015;
                    out_peg_3019:
                    goto loop_peg_3013;
                    success_peg_3015:
                    ;
                    result_peg_3000.addResult(result_peg_3014);
                } while (true);
                loop_peg_3013:
                ;
                        
            }
            goto success_peg_3004;
            goto out_peg_3002;
            success_peg_3004:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_3000.getPosition()))){
                    result_peg_3000.nextPosition();
                } else {
                    goto out_peg_3002;
                }
            }
            result_peg_3000.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_3000.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_3000;
    stream.update(result_peg_3000.getPosition());
    
    
    return result_peg_3000;
    out_peg_3002:
    Result result_peg_3021(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_3021.getPosition()))){
                    result_peg_3021.nextPosition();
                } else {
                    goto out_peg_3023;
                }
            }
            result_peg_3021.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_3021.reset();
                do{
                    Result result_peg_3035(result_peg_3021.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3035.getPosition()))){
                                result_peg_3035.nextPosition();
                            } else {
                                goto out_peg_3038;
                            }
                        }
                        result_peg_3035.setValue((void*) " ");
                            
                    }
                    goto success_peg_3036;
                    out_peg_3038:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3035.getPosition()))){
                                result_peg_3035.nextPosition();
                            } else {
                                goto out_peg_3040;
                            }
                        }
                        result_peg_3035.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3036;
                    out_peg_3040:
                    goto loop_peg_3034;
                    success_peg_3036:
                    ;
                    result_peg_3021.addResult(result_peg_3035);
                } while (true);
                loop_peg_3034:
                ;
                        
            }
            goto success_peg_3025;
            goto out_peg_3023;
            success_peg_3025:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_3021.getPosition()))){
                    result_peg_3021.nextPosition();
                } else {
                    goto out_peg_3023;
                }
            }
            result_peg_3021.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_3021.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_3021;
    stream.update(result_peg_3021.getPosition());
    
    
    return result_peg_3021;
    out_peg_3023:
    Result result_peg_3042(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_3042.getPosition()))){
                    result_peg_3042.nextPosition();
                } else {
                    goto out_peg_3044;
                }
            }
            result_peg_3042.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_3042.reset();
                do{
                    Result result_peg_3056(result_peg_3042.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3056.getPosition()))){
                                result_peg_3056.nextPosition();
                            } else {
                                goto out_peg_3059;
                            }
                        }
                        result_peg_3056.setValue((void*) " ");
                            
                    }
                    goto success_peg_3057;
                    out_peg_3059:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3056.getPosition()))){
                                result_peg_3056.nextPosition();
                            } else {
                                goto out_peg_3061;
                            }
                        }
                        result_peg_3056.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3057;
                    out_peg_3061:
                    goto loop_peg_3055;
                    success_peg_3057:
                    ;
                    result_peg_3042.addResult(result_peg_3056);
                } while (true);
                loop_peg_3055:
                ;
                        
            }
            goto success_peg_3046;
            goto out_peg_3044;
            success_peg_3046:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_3042.getPosition()))){
                    result_peg_3042.nextPosition();
                } else {
                    goto out_peg_3044;
                }
            }
            result_peg_3042.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_3042.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_3042;
    stream.update(result_peg_3042.getPosition());
    
    
    return result_peg_3042;
    out_peg_3044:
    Result result_peg_3063(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_3063.getPosition()))){
                    result_peg_3063.nextPosition();
                } else {
                    goto out_peg_3065;
                }
            }
            result_peg_3063.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_3063.reset();
                do{
                    Result result_peg_3077(result_peg_3063.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3077.getPosition()))){
                                result_peg_3077.nextPosition();
                            } else {
                                goto out_peg_3080;
                            }
                        }
                        result_peg_3077.setValue((void*) " ");
                            
                    }
                    goto success_peg_3078;
                    out_peg_3080:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3077.getPosition()))){
                                result_peg_3077.nextPosition();
                            } else {
                                goto out_peg_3082;
                            }
                        }
                        result_peg_3077.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3078;
                    out_peg_3082:
                    goto loop_peg_3076;
                    success_peg_3078:
                    ;
                    result_peg_3063.addResult(result_peg_3077);
                } while (true);
                loop_peg_3076:
                ;
                        
            }
            goto success_peg_3067;
            goto out_peg_3065;
            success_peg_3067:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_3063.getPosition()))){
                    result_peg_3063.nextPosition();
                } else {
                    goto out_peg_3065;
                }
            }
            result_peg_3063.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist y");
                result_peg_3063.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_3063;
    stream.update(result_peg_3063.getPosition());
    
    
    return result_peg_3063;
    out_peg_3065:
    Result result_peg_3084(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_3084.getPosition()))){
                    result_peg_3084.nextPosition();
                } else {
                    goto out_peg_3086;
                }
            }
            result_peg_3084.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_3084.reset();
                do{
                    Result result_peg_3098(result_peg_3084.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3098.getPosition()))){
                                result_peg_3098.nextPosition();
                            } else {
                                goto out_peg_3101;
                            }
                        }
                        result_peg_3098.setValue((void*) " ");
                            
                    }
                    goto success_peg_3099;
                    out_peg_3101:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3098.getPosition()))){
                                result_peg_3098.nextPosition();
                            } else {
                                goto out_peg_3103;
                            }
                        }
                        result_peg_3098.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3099;
                    out_peg_3103:
                    goto loop_peg_3097;
                    success_peg_3099:
                    ;
                    result_peg_3084.addResult(result_peg_3098);
                } while (true);
                loop_peg_3097:
                ;
                        
            }
            goto success_peg_3088;
            goto out_peg_3086;
            success_peg_3088:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_3084.getPosition()))){
                    result_peg_3084.nextPosition();
                } else {
                    goto out_peg_3086;
                }
            }
            result_peg_3084.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist x");
                result_peg_3084.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_3084;
    stream.update(result_peg_3084.getPosition());
    
    
    return result_peg_3084;
    out_peg_3086:
    Result result_peg_3105(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_3105.getPosition()))){
                    result_peg_3105.nextPosition();
                } else {
                    goto out_peg_3107;
                }
            }
            result_peg_3105.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_3105.reset();
                do{
                    Result result_peg_3119(result_peg_3105.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3119.getPosition()))){
                                result_peg_3119.nextPosition();
                            } else {
                                goto out_peg_3122;
                            }
                        }
                        result_peg_3119.setValue((void*) " ");
                            
                    }
                    goto success_peg_3120;
                    out_peg_3122:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3119.getPosition()))){
                                result_peg_3119.nextPosition();
                            } else {
                                goto out_peg_3124;
                            }
                        }
                        result_peg_3119.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3120;
                    out_peg_3124:
                    goto loop_peg_3118;
                    success_peg_3120:
                    ;
                    result_peg_3105.addResult(result_peg_3119);
                } while (true);
                loop_peg_3118:
                ;
                        
            }
            goto success_peg_3109;
            goto out_peg_3107;
            success_peg_3109:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_3105.getPosition()))){
                    result_peg_3105.nextPosition();
                } else {
                    goto out_peg_3107;
                }
            }
            result_peg_3105.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist y");
                result_peg_3105.setValue(value);
            }
        
        
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_3105;
    stream.update(result_peg_3105.getPosition());
    
    
    return result_peg_3105;
    out_peg_3107:
    Result result_peg_3126(myposition);
    
    result_peg_3126 = rule_hitflag(stream, result_peg_3126.getPosition());
    if (result_peg_3126.error()){
        goto out_peg_3127;
    }
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = result_peg_3126;
    stream.update(result_peg_3126.getPosition());
    
    
    return result_peg_3126;
    out_peg_3127:
    
    if (column_peg_2726.chunk13 == 0){
        column_peg_2726.chunk13 = new Chunk13();
    }
    column_peg_2726.chunk13->chunk_keyword_real = errorResult;
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

        
