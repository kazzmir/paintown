

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
    
    RuleTrace trace_peg_717(stream, "assignment");
    int myposition = position;
    
    
    Value all;
        Value line;
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
            
            
            
            for (int i = 0; i < 7; i++){
                    if (compareCharCase("sparkno"[i], stream.get(result_peg_474.getPosition()))){
                        result_peg_474.nextPosition();
                    } else {
                        goto out_peg_478;
                    }
                }
                result_peg_474.setValue((void*) "sparkno");
            
            
            
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
            
            
            
            result_peg_474 = rule_expr(stream, result_peg_474.getPosition());
                if (result_peg_474.error()){
                    goto out_peg_478;
                }
                exp = result_peg_474.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("sparkno", exp);
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
            
            
            
            result_peg_517 = rule_identifier(stream, result_peg_517.getPosition());
                if (result_peg_517.error()){
                    goto out_peg_521;
                }
                name = result_peg_517.getValues();
            
            
            
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
            
            
            
            result_peg_517 = rule_expr(stream, result_peg_517.getPosition());
                if (result_peg_517.error()){
                    goto out_peg_521;
                }
                exp = result_peg_517.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
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
        Result result_peg_558(myposition);
        
        {
        
            result_peg_558 = rule_identifier(stream, result_peg_558.getPosition());
                if (result_peg_558.error()){
                    goto out_peg_560;
                }
                name = result_peg_558.getValues();
            
            
            
            {
                    
                    result_peg_558.reset();
                    do{
                        Result result_peg_572(result_peg_558.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_572.getPosition()))){
                                    result_peg_572.nextPosition();
                                } else {
                                    goto out_peg_575;
                                }
                            }
                            result_peg_572.setValue((void*) " ");
                                
                        }
                        goto success_peg_573;
                        out_peg_575:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_572.getPosition()))){
                                    result_peg_572.nextPosition();
                                } else {
                                    goto out_peg_577;
                                }
                            }
                            result_peg_572.setValue((void*) "\t");
                                
                        }
                        goto success_peg_573;
                        out_peg_577:
                        goto loop_peg_571;
                        success_peg_573:
                        ;
                        result_peg_558.addResult(result_peg_572);
                    } while (true);
                    loop_peg_571:
                    ;
                            
                }
                goto success_peg_562;
                goto out_peg_560;
                success_peg_562:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_558.getPosition()))){
                        result_peg_558.nextPosition();
                    } else {
                        goto out_peg_560;
                    }
                }
                result_peg_558.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_558.reset();
                    do{
                        Result result_peg_590(result_peg_558.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_590.getPosition()))){
                                    result_peg_590.nextPosition();
                                } else {
                                    goto out_peg_593;
                                }
                            }
                            result_peg_590.setValue((void*) " ");
                                
                        }
                        goto success_peg_591;
                        out_peg_593:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_590.getPosition()))){
                                    result_peg_590.nextPosition();
                                } else {
                                    goto out_peg_595;
                                }
                            }
                            result_peg_590.setValue((void*) "\t");
                                
                        }
                        goto success_peg_591;
                        out_peg_595:
                        goto loop_peg_589;
                        success_peg_591:
                        ;
                        result_peg_558.addResult(result_peg_590);
                    } while (true);
                    loop_peg_589:
                    ;
                            
                }
                goto success_peg_580;
                goto out_peg_560;
                success_peg_580:
                ;
            
            
            
            result_peg_558 = rule_expr(stream, result_peg_558.getPosition());
                if (result_peg_558.error()){
                    goto out_peg_560;
                }
                exp = result_peg_558.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name, negateExpression(exp));
                    result_peg_558.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_558;
        stream.update(result_peg_558.getPosition());
        
        
        return result_peg_558;
        out_peg_560:
        Result result_peg_597(myposition);
        
        {
        
            result_peg_597 = rule_identifier(stream, result_peg_597.getPosition());
                if (result_peg_597.error()){
                    goto out_peg_599;
                }
                name = result_peg_597.getValues();
            
            
            
            {
                    
                    result_peg_597.reset();
                    do{
                        Result result_peg_611(result_peg_597.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_611.getPosition()))){
                                    result_peg_611.nextPosition();
                                } else {
                                    goto out_peg_614;
                                }
                            }
                            result_peg_611.setValue((void*) " ");
                                
                        }
                        goto success_peg_612;
                        out_peg_614:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_611.getPosition()))){
                                    result_peg_611.nextPosition();
                                } else {
                                    goto out_peg_616;
                                }
                            }
                            result_peg_611.setValue((void*) "\t");
                                
                        }
                        goto success_peg_612;
                        out_peg_616:
                        goto loop_peg_610;
                        success_peg_612:
                        ;
                        result_peg_597.addResult(result_peg_611);
                    } while (true);
                    loop_peg_610:
                    ;
                            
                }
                goto success_peg_601;
                goto out_peg_599;
                success_peg_601:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_597.getPosition()))){
                        result_peg_597.nextPosition();
                    } else {
                        goto out_peg_599;
                    }
                }
                result_peg_597.setValue((void*) "=");
            
            
            
            result_peg_597 = rule_whitespace(stream, result_peg_597.getPosition());
                if (result_peg_597.error()){
                    goto out_peg_599;
                }
            
            
            
            Result result_peg_620(result_peg_597.getPosition());
                result_peg_620 = rule_line_end(stream, result_peg_620.getPosition());
                if (result_peg_620.error()){
                    goto out_peg_599;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name);
                    result_peg_597.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_597;
        stream.update(result_peg_597.getPosition());
        
        
        return result_peg_597;
        out_peg_599:
        Result result_peg_621(myposition);
        
        {
        
            result_peg_621 = rule_identifier(stream, result_peg_621.getPosition());
                if (result_peg_621.error()){
                    goto out_peg_623;
                }
                name = result_peg_621.getValues();
            
            
            
            {
                    
                    result_peg_621.reset();
                    do{
                        Result result_peg_635(result_peg_621.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_635.getPosition()))){
                                    result_peg_635.nextPosition();
                                } else {
                                    goto out_peg_638;
                                }
                            }
                            result_peg_635.setValue((void*) " ");
                                
                        }
                        goto success_peg_636;
                        out_peg_638:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_635.getPosition()))){
                                    result_peg_635.nextPosition();
                                } else {
                                    goto out_peg_640;
                                }
                            }
                            result_peg_635.setValue((void*) "\t");
                                
                        }
                        goto success_peg_636;
                        out_peg_640:
                        goto loop_peg_634;
                        success_peg_636:
                        ;
                        result_peg_621.addResult(result_peg_635);
                    } while (true);
                    loop_peg_634:
                    ;
                            
                }
                goto success_peg_625;
                goto out_peg_623;
                success_peg_625:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_621.getPosition()))){
                        result_peg_621.nextPosition();
                    } else {
                        goto out_peg_623;
                    }
                }
                result_peg_621.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_621.reset();
                    do{
                        Result result_peg_653(result_peg_621.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_653.getPosition()))){
                                    result_peg_653.nextPosition();
                                } else {
                                    goto out_peg_656;
                                }
                            }
                            result_peg_653.setValue((void*) " ");
                                
                        }
                        goto success_peg_654;
                        out_peg_656:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_653.getPosition()))){
                                    result_peg_653.nextPosition();
                                } else {
                                    goto out_peg_658;
                                }
                            }
                            result_peg_653.setValue((void*) "\t");
                                
                        }
                        goto success_peg_654;
                        out_peg_658:
                        goto loop_peg_652;
                        success_peg_654:
                        ;
                        result_peg_621.addResult(result_peg_653);
                    } while (true);
                    loop_peg_652:
                    ;
                            
                }
                goto success_peg_643;
                goto out_peg_623;
                success_peg_643:
                ;
            
            
            
            result_peg_621 = rule_integer(stream, result_peg_621.getPosition());
                if (result_peg_621.error()){
                    goto out_peg_623;
                }
                index = result_peg_621.getValues();
            
            
            
            {
                    
                    result_peg_621.reset();
                    do{
                        Result result_peg_671(result_peg_621.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_671.getPosition()))){
                                    result_peg_671.nextPosition();
                                } else {
                                    goto out_peg_674;
                                }
                            }
                            result_peg_671.setValue((void*) " ");
                                
                        }
                        goto success_peg_672;
                        out_peg_674:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_671.getPosition()))){
                                    result_peg_671.nextPosition();
                                } else {
                                    goto out_peg_676;
                                }
                            }
                            result_peg_671.setValue((void*) "\t");
                                
                        }
                        goto success_peg_672;
                        out_peg_676:
                        goto loop_peg_670;
                        success_peg_672:
                        ;
                        result_peg_621.addResult(result_peg_671);
                    } while (true);
                    loop_peg_670:
                    ;
                            
                }
                goto success_peg_661;
                goto out_peg_623;
                success_peg_661:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_621.getPosition()))){
                        result_peg_621.nextPosition();
                    } else {
                        goto out_peg_623;
                    }
                }
                result_peg_621.setValue((void*) ")");
            
            
            
            {
                    
                    result_peg_621.reset();
                    do{
                        Result result_peg_689(result_peg_621.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_689.getPosition()))){
                                    result_peg_689.nextPosition();
                                } else {
                                    goto out_peg_692;
                                }
                            }
                            result_peg_689.setValue((void*) " ");
                                
                        }
                        goto success_peg_690;
                        out_peg_692:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_689.getPosition()))){
                                    result_peg_689.nextPosition();
                                } else {
                                    goto out_peg_694;
                                }
                            }
                            result_peg_689.setValue((void*) "\t");
                                
                        }
                        goto success_peg_690;
                        out_peg_694:
                        goto loop_peg_688;
                        success_peg_690:
                        ;
                        result_peg_621.addResult(result_peg_689);
                    } while (true);
                    loop_peg_688:
                    ;
                            
                }
                goto success_peg_679;
                goto out_peg_623;
                success_peg_679:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_621.getPosition()))){
                        result_peg_621.nextPosition();
                    } else {
                        goto out_peg_623;
                    }
                }
                result_peg_621.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_621.reset();
                    do{
                        Result result_peg_707(result_peg_621.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_707.getPosition()))){
                                    result_peg_707.nextPosition();
                                } else {
                                    goto out_peg_710;
                                }
                            }
                            result_peg_707.setValue((void*) " ");
                                
                        }
                        goto success_peg_708;
                        out_peg_710:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_707.getPosition()))){
                                    result_peg_707.nextPosition();
                                } else {
                                    goto out_peg_712;
                                }
                            }
                            result_peg_707.setValue((void*) "\t");
                                
                        }
                        goto success_peg_708;
                        out_peg_712:
                        goto loop_peg_706;
                        success_peg_708:
                        ;
                        result_peg_621.addResult(result_peg_707);
                    } while (true);
                    loop_peg_706:
                    ;
                            
                }
                goto success_peg_697;
                goto out_peg_623;
                success_peg_697:
                ;
            
            
            
            result_peg_621 = rule_expr(stream, result_peg_621.getPosition());
                if (result_peg_621.error()){
                    goto out_peg_623;
                }
                exp = result_peg_621.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(name, index, exp);
                    result_peg_621.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_621;
        stream.update(result_peg_621.getPosition());
        
        
        return result_peg_621;
        out_peg_623:
        Result result_peg_714(myposition);
        
        {
        
            for (int i = 0; i < 4; i++){
                    if (compareCharCase("ctrl"[i], stream.get(result_peg_714.getPosition()))){
                        result_peg_714.nextPosition();
                    } else {
                        goto out_peg_716;
                    }
                }
                result_peg_714.setValue((void*) "ctrl");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(makeSimpleIdentifier("ctrl"), makeNumber(1));
                    result_peg_714.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_714;
        stream.update(result_peg_714.getPosition());
        
        
        return result_peg_714;
        out_peg_716:
    
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_718 = stream.getColumn(position);
    if (column_peg_718.chunk2 != 0 && column_peg_718.chunk2->chunk_identifier.calculated()){
        return column_peg_718.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_728(stream, "identifier");
    int myposition = position;
    
    
    Value line;
        Value first;
        Value rest;
    Result result_peg_719(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_721 = stream.getLineInfo(result_peg_719.getPosition());
                line = &line_info_peg_721;
            
            
            
            result_peg_719 = rule_name(stream, result_peg_719.getPosition());
                if (result_peg_719.error()){
                    goto out_peg_723;
                }
                first = result_peg_719.getValues();
            
            
            
            result_peg_719.reset();
                do{
                    Result result_peg_726(result_peg_719.getPosition());
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_726.getPosition()))){
                                    result_peg_726.nextPosition();
                                } else {
                                    goto loop_peg_725;
                                }
                            }
                            result_peg_726.setValue((void*) ".");
                        
                        
                        
                        result_peg_726 = rule_name(stream, result_peg_726.getPosition());
                            if (result_peg_726.error()){
                                goto loop_peg_725;
                            }
                        
                        
                    }
                    result_peg_719.addResult(result_peg_726);
                } while (true);
                loop_peg_725:
                ;
                rest = result_peg_719.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                    result_peg_719.setValue(value);
                }
            
            
        }
        
        if (column_peg_718.chunk2 == 0){
            column_peg_718.chunk2 = new Chunk2();
        }
        column_peg_718.chunk2->chunk_identifier = result_peg_719;
        stream.update(result_peg_719.getPosition());
        
        
        return result_peg_719;
        out_peg_723:
    
        if (column_peg_718.chunk2 == 0){
            column_peg_718.chunk2 = new Chunk2();
        }
        column_peg_718.chunk2->chunk_identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_729 = stream.getColumn(position);
    if (column_peg_729.chunk2 != 0 && column_peg_729.chunk2->chunk_integer.calculated()){
        return column_peg_729.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_747(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_730(myposition);
        
        {
        
            int save_peg_732 = result_peg_730.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_730.getPosition()))){
                            result_peg_730.nextPosition();
                        } else {
                            goto out_peg_735;
                        }
                    }
                    result_peg_730.setValue((void*) "-");
                        
                }
                goto success_peg_733;
                out_peg_735:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_730.getPosition()))){
                            result_peg_730.nextPosition();
                        } else {
                            goto out_peg_737;
                        }
                    }
                    result_peg_730.setValue((void*) "+");
                        
                }
                goto success_peg_733;
                out_peg_737:
                
                result_peg_730 = Result(save_peg_732);
                result_peg_730.setValue((void*) 0);
                
                success_peg_733:
                ;
            
            Result result_peg_731 = result_peg_730;
            
            result_peg_730.reset();
                do{
                    Result result_peg_740(result_peg_730.getPosition());
                    {
                        
                        char letter_peg_745 = stream.get(result_peg_740.getPosition());
                        if (letter_peg_745 != '\0' && strchr("0123456789", letter_peg_745) != NULL){
                            result_peg_740.nextPosition();
                            result_peg_740.setValue((void*) (long) letter_peg_745);
                        } else {
                            goto out_peg_744;
                        }
                        
                    }
                    goto success_peg_741;
                    out_peg_744:
                    goto loop_peg_739;
                    success_peg_741:
                    ;
                    result_peg_730.addResult(result_peg_740);
                } while (true);
                loop_peg_739:
                if (result_peg_730.matches() == 0){
                    goto out_peg_746;
                }
            
            Result result_peg_738 = result_peg_730;
            
            {
                    Value value((void*) 0);
                    value = makeInteger(result_peg_731.getValues(), result_peg_738.getValues());
                    result_peg_730.setValue(value);
                }
            
            
        }
        
        if (column_peg_729.chunk2 == 0){
            column_peg_729.chunk2 = new Chunk2();
        }
        column_peg_729.chunk2->chunk_integer = result_peg_730;
        stream.update(result_peg_730.getPosition());
        
        
        return result_peg_730;
        out_peg_746:
    
        if (column_peg_729.chunk2 == 0){
            column_peg_729.chunk2 = new Chunk2();
        }
        column_peg_729.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_748 = stream.getColumn(position);
    if (column_peg_748.chunk2 != 0 && column_peg_748.chunk2->chunk_float.calculated()){
        return column_peg_748.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_793(stream, "float");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_749(myposition);
        
        {
        
            int save_peg_751 = result_peg_749.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_749.getPosition()))){
                            result_peg_749.nextPosition();
                        } else {
                            goto out_peg_754;
                        }
                    }
                    result_peg_749.setValue((void*) "-");
                        
                }
                goto success_peg_752;
                out_peg_754:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_749.getPosition()))){
                            result_peg_749.nextPosition();
                        } else {
                            goto out_peg_756;
                        }
                    }
                    result_peg_749.setValue((void*) "+");
                        
                }
                goto success_peg_752;
                out_peg_756:
                
                result_peg_749 = Result(save_peg_751);
                result_peg_749.setValue((void*) 0);
                
                success_peg_752:
                ;
            
            Result result_peg_750 = result_peg_749;
            
            result_peg_749.reset();
                do{
                    Result result_peg_759(result_peg_749.getPosition());
                    {
                        
                        char letter_peg_764 = stream.get(result_peg_759.getPosition());
                        if (letter_peg_764 != '\0' && strchr("0123456789", letter_peg_764) != NULL){
                            result_peg_759.nextPosition();
                            result_peg_759.setValue((void*) (long) letter_peg_764);
                        } else {
                            goto out_peg_763;
                        }
                        
                    }
                    goto success_peg_760;
                    out_peg_763:
                    goto loop_peg_758;
                    success_peg_760:
                    ;
                    result_peg_749.addResult(result_peg_759);
                } while (true);
                loop_peg_758:
                ;
                left = result_peg_749.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_749.getPosition()))){
                        result_peg_749.nextPosition();
                    } else {
                        goto out_peg_766;
                    }
                }
                result_peg_749.setValue((void*) ".");
            
            
            
            result_peg_749.reset();
                do{
                    Result result_peg_769(result_peg_749.getPosition());
                    {
                        
                        char letter_peg_774 = stream.get(result_peg_769.getPosition());
                        if (letter_peg_774 != '\0' && strchr("0123456789", letter_peg_774) != NULL){
                            result_peg_769.nextPosition();
                            result_peg_769.setValue((void*) (long) letter_peg_774);
                        } else {
                            goto out_peg_773;
                        }
                        
                    }
                    goto success_peg_770;
                    out_peg_773:
                    goto loop_peg_768;
                    success_peg_770:
                    ;
                    result_peg_749.addResult(result_peg_769);
                } while (true);
                loop_peg_768:
                if (result_peg_749.matches() == 0){
                    goto out_peg_766;
                }
                right = result_peg_749.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_750.getValues(), parseDouble(left,right));
                    result_peg_749.setValue(value);
                }
            
            
        }
        
        if (column_peg_748.chunk2 == 0){
            column_peg_748.chunk2 = new Chunk2();
        }
        column_peg_748.chunk2->chunk_float = result_peg_749;
        stream.update(result_peg_749.getPosition());
        
        
        return result_peg_749;
        out_peg_766:
        Result result_peg_775(myposition);
        
        {
        
            int save_peg_777 = result_peg_775.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_775.getPosition()))){
                            result_peg_775.nextPosition();
                        } else {
                            goto out_peg_780;
                        }
                    }
                    result_peg_775.setValue((void*) "-");
                        
                }
                goto success_peg_778;
                out_peg_780:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_775.getPosition()))){
                            result_peg_775.nextPosition();
                        } else {
                            goto out_peg_782;
                        }
                    }
                    result_peg_775.setValue((void*) "+");
                        
                }
                goto success_peg_778;
                out_peg_782:
                
                result_peg_775 = Result(save_peg_777);
                result_peg_775.setValue((void*) 0);
                
                success_peg_778:
                ;
            
            Result result_peg_776 = result_peg_775;
            
            result_peg_775.reset();
                do{
                    Result result_peg_785(result_peg_775.getPosition());
                    {
                        
                        char letter_peg_790 = stream.get(result_peg_785.getPosition());
                        if (letter_peg_790 != '\0' && strchr("0123456789", letter_peg_790) != NULL){
                            result_peg_785.nextPosition();
                            result_peg_785.setValue((void*) (long) letter_peg_790);
                        } else {
                            goto out_peg_789;
                        }
                        
                    }
                    goto success_peg_786;
                    out_peg_789:
                    goto loop_peg_784;
                    success_peg_786:
                    ;
                    result_peg_775.addResult(result_peg_785);
                } while (true);
                loop_peg_784:
                if (result_peg_775.matches() == 0){
                    goto out_peg_791;
                }
                left = result_peg_775.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_775.getPosition()))){
                        result_peg_775.nextPosition();
                    } else {
                        goto out_peg_791;
                    }
                }
                result_peg_775.setValue((void*) ".");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_776.getValues(), parseDouble(left));
                    result_peg_775.setValue(value);
                }
            
            
        }
        
        if (column_peg_748.chunk2 == 0){
            column_peg_748.chunk2 = new Chunk2();
        }
        column_peg_748.chunk2->chunk_float = result_peg_775;
        stream.update(result_peg_775.getPosition());
        
        
        return result_peg_775;
        out_peg_791:
    
        if (column_peg_748.chunk2 == 0){
            column_peg_748.chunk2 = new Chunk2();
        }
        column_peg_748.chunk2->chunk_float = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_794 = stream.getColumn(position);
    if (column_peg_794.chunk2 != 0 && column_peg_794.chunk2->chunk_string.calculated()){
        return column_peg_794.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_809(stream, "string");
    int myposition = position;
    
    
    Value contents;
    Result result_peg_795(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_795.getPosition()))){
                        result_peg_795.nextPosition();
                    } else {
                        goto out_peg_797;
                    }
                }
                result_peg_795.setValue((void*) "\"");
            
            
            
            result_peg_795.reset();
                do{
                    Result result_peg_800(result_peg_795.getPosition());
                    {
                    
                        Result result_peg_803(result_peg_800);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\""[i], stream.get(result_peg_803.getPosition()))){
                                    result_peg_803.nextPosition();
                                } else {
                                    goto not_peg_802;
                                }
                            }
                            result_peg_803.setValue((void*) "\"");
                            goto loop_peg_799;
                            not_peg_802:
                            result_peg_800.setValue((void*)0);
                        
                        
                        
                        Result result_peg_806(result_peg_800);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_806.getPosition()))){
                                    result_peg_806.nextPosition();
                                } else {
                                    goto not_peg_805;
                                }
                            }
                            result_peg_806.setValue((void*) "\n");
                            goto loop_peg_799;
                            not_peg_805:
                            result_peg_800.setValue((void*)0);
                        
                        
                        
                        char temp_peg_807 = stream.get(result_peg_800.getPosition());
                            if (temp_peg_807 != '\0'){
                                result_peg_800.setValue((void*) (long) temp_peg_807);
                                result_peg_800.nextPosition();
                            } else {
                                goto loop_peg_799;
                            }
                        
                        
                    }
                    result_peg_795.addResult(result_peg_800);
                } while (true);
                loop_peg_799:
                ;
                contents = result_peg_795.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_795.getPosition()))){
                        result_peg_795.nextPosition();
                    } else {
                        goto out_peg_797;
                    }
                }
                result_peg_795.setValue((void*) "\"");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeString(contents);
                    result_peg_795.setValue(value);
                }
            
            
        }
        
        if (column_peg_794.chunk2 == 0){
            column_peg_794.chunk2 = new Chunk2();
        }
        column_peg_794.chunk2->chunk_string = result_peg_795;
        stream.update(result_peg_795.getPosition());
        
        
        return result_peg_795;
        out_peg_797:
    
        if (column_peg_794.chunk2 == 0){
            column_peg_794.chunk2 = new Chunk2();
        }
        column_peg_794.chunk2->chunk_string = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_810 = stream.getColumn(position);
    if (column_peg_810.chunk2 != 0 && column_peg_810.chunk2->chunk_range.calculated()){
        return column_peg_810.chunk2->chunk_range;
    }
    
    RuleTrace trace_peg_1111(stream, "range");
    int myposition = position;
    
    
    Value low;
        Value high;
    Result result_peg_811(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_811.getPosition()))){
                        result_peg_811.nextPosition();
                    } else {
                        goto out_peg_813;
                    }
                }
                result_peg_811.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_811.reset();
                    do{
                        Result result_peg_825(result_peg_811.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_825.getPosition()))){
                                    result_peg_825.nextPosition();
                                } else {
                                    goto out_peg_828;
                                }
                            }
                            result_peg_825.setValue((void*) " ");
                                
                        }
                        goto success_peg_826;
                        out_peg_828:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_825.getPosition()))){
                                    result_peg_825.nextPosition();
                                } else {
                                    goto out_peg_830;
                                }
                            }
                            result_peg_825.setValue((void*) "\t");
                                
                        }
                        goto success_peg_826;
                        out_peg_830:
                        goto loop_peg_824;
                        success_peg_826:
                        ;
                        result_peg_811.addResult(result_peg_825);
                    } while (true);
                    loop_peg_824:
                    ;
                            
                }
                goto success_peg_815;
                goto out_peg_813;
                success_peg_815:
                ;
            
            
            
            result_peg_811 = rule_expr_c(stream, result_peg_811.getPosition());
                if (result_peg_811.error()){
                    goto out_peg_813;
                }
                low = result_peg_811.getValues();
            
            
            
            {
                    
                    result_peg_811.reset();
                    do{
                        Result result_peg_843(result_peg_811.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_843.getPosition()))){
                                    result_peg_843.nextPosition();
                                } else {
                                    goto out_peg_846;
                                }
                            }
                            result_peg_843.setValue((void*) " ");
                                
                        }
                        goto success_peg_844;
                        out_peg_846:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_843.getPosition()))){
                                    result_peg_843.nextPosition();
                                } else {
                                    goto out_peg_848;
                                }
                            }
                            result_peg_843.setValue((void*) "\t");
                                
                        }
                        goto success_peg_844;
                        out_peg_848:
                        goto loop_peg_842;
                        success_peg_844:
                        ;
                        result_peg_811.addResult(result_peg_843);
                    } while (true);
                    loop_peg_842:
                    ;
                            
                }
                goto success_peg_833;
                goto out_peg_813;
                success_peg_833:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_811.getPosition()))){
                        result_peg_811.nextPosition();
                    } else {
                        goto out_peg_813;
                    }
                }
                result_peg_811.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_811.reset();
                    do{
                        Result result_peg_861(result_peg_811.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_861.getPosition()))){
                                    result_peg_861.nextPosition();
                                } else {
                                    goto out_peg_864;
                                }
                            }
                            result_peg_861.setValue((void*) " ");
                                
                        }
                        goto success_peg_862;
                        out_peg_864:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_861.getPosition()))){
                                    result_peg_861.nextPosition();
                                } else {
                                    goto out_peg_866;
                                }
                            }
                            result_peg_861.setValue((void*) "\t");
                                
                        }
                        goto success_peg_862;
                        out_peg_866:
                        goto loop_peg_860;
                        success_peg_862:
                        ;
                        result_peg_811.addResult(result_peg_861);
                    } while (true);
                    loop_peg_860:
                    ;
                            
                }
                goto success_peg_851;
                goto out_peg_813;
                success_peg_851:
                ;
            
            
            
            result_peg_811 = rule_expr_c(stream, result_peg_811.getPosition());
                if (result_peg_811.error()){
                    goto out_peg_813;
                }
                high = result_peg_811.getValues();
            
            
            
            {
                    
                    result_peg_811.reset();
                    do{
                        Result result_peg_879(result_peg_811.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_879.getPosition()))){
                                    result_peg_879.nextPosition();
                                } else {
                                    goto out_peg_882;
                                }
                            }
                            result_peg_879.setValue((void*) " ");
                                
                        }
                        goto success_peg_880;
                        out_peg_882:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_879.getPosition()))){
                                    result_peg_879.nextPosition();
                                } else {
                                    goto out_peg_884;
                                }
                            }
                            result_peg_879.setValue((void*) "\t");
                                
                        }
                        goto success_peg_880;
                        out_peg_884:
                        goto loop_peg_878;
                        success_peg_880:
                        ;
                        result_peg_811.addResult(result_peg_879);
                    } while (true);
                    loop_peg_878:
                    ;
                            
                }
                goto success_peg_869;
                goto out_peg_813;
                success_peg_869:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_811.getPosition()))){
                        result_peg_811.nextPosition();
                    } else {
                        goto out_peg_813;
                    }
                }
                result_peg_811.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllInclusive, low, high);
                    result_peg_811.setValue(value);
                }
            
            
        }
        
        if (column_peg_810.chunk2 == 0){
            column_peg_810.chunk2 = new Chunk2();
        }
        column_peg_810.chunk2->chunk_range = result_peg_811;
        stream.update(result_peg_811.getPosition());
        
        
        return result_peg_811;
        out_peg_813:
        Result result_peg_886(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_886.getPosition()))){
                        result_peg_886.nextPosition();
                    } else {
                        goto out_peg_888;
                    }
                }
                result_peg_886.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_886.reset();
                    do{
                        Result result_peg_900(result_peg_886.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_900.getPosition()))){
                                    result_peg_900.nextPosition();
                                } else {
                                    goto out_peg_903;
                                }
                            }
                            result_peg_900.setValue((void*) " ");
                                
                        }
                        goto success_peg_901;
                        out_peg_903:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_900.getPosition()))){
                                    result_peg_900.nextPosition();
                                } else {
                                    goto out_peg_905;
                                }
                            }
                            result_peg_900.setValue((void*) "\t");
                                
                        }
                        goto success_peg_901;
                        out_peg_905:
                        goto loop_peg_899;
                        success_peg_901:
                        ;
                        result_peg_886.addResult(result_peg_900);
                    } while (true);
                    loop_peg_899:
                    ;
                            
                }
                goto success_peg_890;
                goto out_peg_888;
                success_peg_890:
                ;
            
            
            
            result_peg_886 = rule_expr_c(stream, result_peg_886.getPosition());
                if (result_peg_886.error()){
                    goto out_peg_888;
                }
                low = result_peg_886.getValues();
            
            
            
            {
                    
                    result_peg_886.reset();
                    do{
                        Result result_peg_918(result_peg_886.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_918.getPosition()))){
                                    result_peg_918.nextPosition();
                                } else {
                                    goto out_peg_921;
                                }
                            }
                            result_peg_918.setValue((void*) " ");
                                
                        }
                        goto success_peg_919;
                        out_peg_921:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_918.getPosition()))){
                                    result_peg_918.nextPosition();
                                } else {
                                    goto out_peg_923;
                                }
                            }
                            result_peg_918.setValue((void*) "\t");
                                
                        }
                        goto success_peg_919;
                        out_peg_923:
                        goto loop_peg_917;
                        success_peg_919:
                        ;
                        result_peg_886.addResult(result_peg_918);
                    } while (true);
                    loop_peg_917:
                    ;
                            
                }
                goto success_peg_908;
                goto out_peg_888;
                success_peg_908:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_886.getPosition()))){
                        result_peg_886.nextPosition();
                    } else {
                        goto out_peg_888;
                    }
                }
                result_peg_886.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_886.reset();
                    do{
                        Result result_peg_936(result_peg_886.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_936.getPosition()))){
                                    result_peg_936.nextPosition();
                                } else {
                                    goto out_peg_939;
                                }
                            }
                            result_peg_936.setValue((void*) " ");
                                
                        }
                        goto success_peg_937;
                        out_peg_939:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_936.getPosition()))){
                                    result_peg_936.nextPosition();
                                } else {
                                    goto out_peg_941;
                                }
                            }
                            result_peg_936.setValue((void*) "\t");
                                
                        }
                        goto success_peg_937;
                        out_peg_941:
                        goto loop_peg_935;
                        success_peg_937:
                        ;
                        result_peg_886.addResult(result_peg_936);
                    } while (true);
                    loop_peg_935:
                    ;
                            
                }
                goto success_peg_926;
                goto out_peg_888;
                success_peg_926:
                ;
            
            
            
            result_peg_886 = rule_expr_c(stream, result_peg_886.getPosition());
                if (result_peg_886.error()){
                    goto out_peg_888;
                }
                high = result_peg_886.getValues();
            
            
            
            {
                    
                    result_peg_886.reset();
                    do{
                        Result result_peg_954(result_peg_886.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_954.getPosition()))){
                                    result_peg_954.nextPosition();
                                } else {
                                    goto out_peg_957;
                                }
                            }
                            result_peg_954.setValue((void*) " ");
                                
                        }
                        goto success_peg_955;
                        out_peg_957:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_954.getPosition()))){
                                    result_peg_954.nextPosition();
                                } else {
                                    goto out_peg_959;
                                }
                            }
                            result_peg_954.setValue((void*) "\t");
                                
                        }
                        goto success_peg_955;
                        out_peg_959:
                        goto loop_peg_953;
                        success_peg_955:
                        ;
                        result_peg_886.addResult(result_peg_954);
                    } while (true);
                    loop_peg_953:
                    ;
                            
                }
                goto success_peg_944;
                goto out_peg_888;
                success_peg_944:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_886.getPosition()))){
                        result_peg_886.nextPosition();
                    } else {
                        goto out_peg_888;
                    }
                }
                result_peg_886.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllExclusive, low, high);
                    result_peg_886.setValue(value);
                }
            
            
        }
        
        if (column_peg_810.chunk2 == 0){
            column_peg_810.chunk2 = new Chunk2();
        }
        column_peg_810.chunk2->chunk_range = result_peg_886;
        stream.update(result_peg_886.getPosition());
        
        
        return result_peg_886;
        out_peg_888:
        Result result_peg_961(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_961.getPosition()))){
                        result_peg_961.nextPosition();
                    } else {
                        goto out_peg_963;
                    }
                }
                result_peg_961.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_961.reset();
                    do{
                        Result result_peg_975(result_peg_961.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_975.getPosition()))){
                                    result_peg_975.nextPosition();
                                } else {
                                    goto out_peg_978;
                                }
                            }
                            result_peg_975.setValue((void*) " ");
                                
                        }
                        goto success_peg_976;
                        out_peg_978:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_975.getPosition()))){
                                    result_peg_975.nextPosition();
                                } else {
                                    goto out_peg_980;
                                }
                            }
                            result_peg_975.setValue((void*) "\t");
                                
                        }
                        goto success_peg_976;
                        out_peg_980:
                        goto loop_peg_974;
                        success_peg_976:
                        ;
                        result_peg_961.addResult(result_peg_975);
                    } while (true);
                    loop_peg_974:
                    ;
                            
                }
                goto success_peg_965;
                goto out_peg_963;
                success_peg_965:
                ;
            
            
            
            result_peg_961 = rule_expr_c(stream, result_peg_961.getPosition());
                if (result_peg_961.error()){
                    goto out_peg_963;
                }
                low = result_peg_961.getValues();
            
            
            
            {
                    
                    result_peg_961.reset();
                    do{
                        Result result_peg_993(result_peg_961.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_993.getPosition()))){
                                    result_peg_993.nextPosition();
                                } else {
                                    goto out_peg_996;
                                }
                            }
                            result_peg_993.setValue((void*) " ");
                                
                        }
                        goto success_peg_994;
                        out_peg_996:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_993.getPosition()))){
                                    result_peg_993.nextPosition();
                                } else {
                                    goto out_peg_998;
                                }
                            }
                            result_peg_993.setValue((void*) "\t");
                                
                        }
                        goto success_peg_994;
                        out_peg_998:
                        goto loop_peg_992;
                        success_peg_994:
                        ;
                        result_peg_961.addResult(result_peg_993);
                    } while (true);
                    loop_peg_992:
                    ;
                            
                }
                goto success_peg_983;
                goto out_peg_963;
                success_peg_983:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_961.getPosition()))){
                        result_peg_961.nextPosition();
                    } else {
                        goto out_peg_963;
                    }
                }
                result_peg_961.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_961.reset();
                    do{
                        Result result_peg_1011(result_peg_961.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1011.getPosition()))){
                                    result_peg_1011.nextPosition();
                                } else {
                                    goto out_peg_1014;
                                }
                            }
                            result_peg_1011.setValue((void*) " ");
                                
                        }
                        goto success_peg_1012;
                        out_peg_1014:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1011.getPosition()))){
                                    result_peg_1011.nextPosition();
                                } else {
                                    goto out_peg_1016;
                                }
                            }
                            result_peg_1011.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1012;
                        out_peg_1016:
                        goto loop_peg_1010;
                        success_peg_1012:
                        ;
                        result_peg_961.addResult(result_peg_1011);
                    } while (true);
                    loop_peg_1010:
                    ;
                            
                }
                goto success_peg_1001;
                goto out_peg_963;
                success_peg_1001:
                ;
            
            
            
            result_peg_961 = rule_expr_c(stream, result_peg_961.getPosition());
                if (result_peg_961.error()){
                    goto out_peg_963;
                }
                high = result_peg_961.getValues();
            
            
            
            {
                    
                    result_peg_961.reset();
                    do{
                        Result result_peg_1029(result_peg_961.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1029.getPosition()))){
                                    result_peg_1029.nextPosition();
                                } else {
                                    goto out_peg_1032;
                                }
                            }
                            result_peg_1029.setValue((void*) " ");
                                
                        }
                        goto success_peg_1030;
                        out_peg_1032:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1029.getPosition()))){
                                    result_peg_1029.nextPosition();
                                } else {
                                    goto out_peg_1034;
                                }
                            }
                            result_peg_1029.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1030;
                        out_peg_1034:
                        goto loop_peg_1028;
                        success_peg_1030:
                        ;
                        result_peg_961.addResult(result_peg_1029);
                    } while (true);
                    loop_peg_1028:
                    ;
                            
                }
                goto success_peg_1019;
                goto out_peg_963;
                success_peg_1019:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_961.getPosition()))){
                        result_peg_961.nextPosition();
                    } else {
                        goto out_peg_963;
                    }
                }
                result_peg_961.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                    result_peg_961.setValue(value);
                }
            
            
        }
        
        if (column_peg_810.chunk2 == 0){
            column_peg_810.chunk2 = new Chunk2();
        }
        column_peg_810.chunk2->chunk_range = result_peg_961;
        stream.update(result_peg_961.getPosition());
        
        
        return result_peg_961;
        out_peg_963:
        Result result_peg_1036(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_1036.getPosition()))){
                        result_peg_1036.nextPosition();
                    } else {
                        goto out_peg_1038;
                    }
                }
                result_peg_1036.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_1036.reset();
                    do{
                        Result result_peg_1050(result_peg_1036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1050.getPosition()))){
                                    result_peg_1050.nextPosition();
                                } else {
                                    goto out_peg_1053;
                                }
                            }
                            result_peg_1050.setValue((void*) " ");
                                
                        }
                        goto success_peg_1051;
                        out_peg_1053:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1050.getPosition()))){
                                    result_peg_1050.nextPosition();
                                } else {
                                    goto out_peg_1055;
                                }
                            }
                            result_peg_1050.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1051;
                        out_peg_1055:
                        goto loop_peg_1049;
                        success_peg_1051:
                        ;
                        result_peg_1036.addResult(result_peg_1050);
                    } while (true);
                    loop_peg_1049:
                    ;
                            
                }
                goto success_peg_1040;
                goto out_peg_1038;
                success_peg_1040:
                ;
            
            
            
            result_peg_1036 = rule_expr_c(stream, result_peg_1036.getPosition());
                if (result_peg_1036.error()){
                    goto out_peg_1038;
                }
                low = result_peg_1036.getValues();
            
            
            
            {
                    
                    result_peg_1036.reset();
                    do{
                        Result result_peg_1068(result_peg_1036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1068.getPosition()))){
                                    result_peg_1068.nextPosition();
                                } else {
                                    goto out_peg_1071;
                                }
                            }
                            result_peg_1068.setValue((void*) " ");
                                
                        }
                        goto success_peg_1069;
                        out_peg_1071:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1068.getPosition()))){
                                    result_peg_1068.nextPosition();
                                } else {
                                    goto out_peg_1073;
                                }
                            }
                            result_peg_1068.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1069;
                        out_peg_1073:
                        goto loop_peg_1067;
                        success_peg_1069:
                        ;
                        result_peg_1036.addResult(result_peg_1068);
                    } while (true);
                    loop_peg_1067:
                    ;
                            
                }
                goto success_peg_1058;
                goto out_peg_1038;
                success_peg_1058:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1036.getPosition()))){
                        result_peg_1036.nextPosition();
                    } else {
                        goto out_peg_1038;
                    }
                }
                result_peg_1036.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1036.reset();
                    do{
                        Result result_peg_1086(result_peg_1036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1086.getPosition()))){
                                    result_peg_1086.nextPosition();
                                } else {
                                    goto out_peg_1089;
                                }
                            }
                            result_peg_1086.setValue((void*) " ");
                                
                        }
                        goto success_peg_1087;
                        out_peg_1089:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1086.getPosition()))){
                                    result_peg_1086.nextPosition();
                                } else {
                                    goto out_peg_1091;
                                }
                            }
                            result_peg_1086.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1087;
                        out_peg_1091:
                        goto loop_peg_1085;
                        success_peg_1087:
                        ;
                        result_peg_1036.addResult(result_peg_1086);
                    } while (true);
                    loop_peg_1085:
                    ;
                            
                }
                goto success_peg_1076;
                goto out_peg_1038;
                success_peg_1076:
                ;
            
            
            
            result_peg_1036 = rule_expr_c(stream, result_peg_1036.getPosition());
                if (result_peg_1036.error()){
                    goto out_peg_1038;
                }
                high = result_peg_1036.getValues();
            
            
            
            {
                    
                    result_peg_1036.reset();
                    do{
                        Result result_peg_1104(result_peg_1036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1104.getPosition()))){
                                    result_peg_1104.nextPosition();
                                } else {
                                    goto out_peg_1107;
                                }
                            }
                            result_peg_1104.setValue((void*) " ");
                                
                        }
                        goto success_peg_1105;
                        out_peg_1107:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1104.getPosition()))){
                                    result_peg_1104.nextPosition();
                                } else {
                                    goto out_peg_1109;
                                }
                            }
                            result_peg_1104.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1105;
                        out_peg_1109:
                        goto loop_peg_1103;
                        success_peg_1105:
                        ;
                        result_peg_1036.addResult(result_peg_1104);
                    } while (true);
                    loop_peg_1103:
                    ;
                            
                }
                goto success_peg_1094;
                goto out_peg_1038;
                success_peg_1094:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_1036.getPosition()))){
                        result_peg_1036.nextPosition();
                    } else {
                        goto out_peg_1038;
                    }
                }
                result_peg_1036.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                    result_peg_1036.setValue(value);
                }
            
            
        }
        
        if (column_peg_810.chunk2 == 0){
            column_peg_810.chunk2 = new Chunk2();
        }
        column_peg_810.chunk2->chunk_range = result_peg_1036;
        stream.update(result_peg_1036.getPosition());
        
        
        return result_peg_1036;
        out_peg_1038:
    
        if (column_peg_810.chunk2 == 0){
            column_peg_810.chunk2 = new Chunk2();
        }
        column_peg_810.chunk2->chunk_range = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_1112 = stream.getColumn(position);
    if (column_peg_1112.chunk3 != 0 && column_peg_1112.chunk3->chunk_name.calculated()){
        return column_peg_1112.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_1124(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_1113(myposition);
        
        {
        
            {
                    
                    char letter_peg_1119 = stream.get(result_peg_1113.getPosition());
                    if (letter_peg_1119 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1119) != NULL){
                        result_peg_1113.nextPosition();
                        result_peg_1113.setValue((void*) (long) letter_peg_1119);
                    } else {
                        goto out_peg_1118;
                    }
                    
                }
                goto success_peg_1115;
                out_peg_1118:
                goto out_peg_1120;
                success_peg_1115:
                ;
            
            Result result_peg_1114 = result_peg_1113;
            
            result_peg_1113.reset();
                do{
                    Result result_peg_1123(result_peg_1113.getPosition());
                    result_peg_1123 = rule_alpha_digit(stream, result_peg_1123.getPosition());
                    if (result_peg_1123.error()){
                        goto loop_peg_1122;
                    }
                    result_peg_1113.addResult(result_peg_1123);
                } while (true);
                loop_peg_1122:
                ;
            
            Result result_peg_1121 = result_peg_1113;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_1114.getValues().getValue(),result_peg_1121.getValues());
                    result_peg_1113.setValue(value);
                }
            
            
        }
        
        if (column_peg_1112.chunk3 == 0){
            column_peg_1112.chunk3 = new Chunk3();
        }
        column_peg_1112.chunk3->chunk_name = result_peg_1113;
        stream.update(result_peg_1113.getPosition());
        
        
        return result_peg_1113;
        out_peg_1120:
    
        if (column_peg_1112.chunk3 == 0){
            column_peg_1112.chunk3 = new Chunk3();
        }
        column_peg_1112.chunk3->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_1125 = stream.getColumn(position);
    if (column_peg_1125.chunk3 != 0 && column_peg_1125.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_1125.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_1140(stream, "alpha_digit");
    int myposition = position;
    
    
    
    Result result_peg_1126(myposition);
        
        {
            
            char letter_peg_1131 = stream.get(result_peg_1126.getPosition());
            if (letter_peg_1131 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1131) != NULL){
                result_peg_1126.nextPosition();
                result_peg_1126.setValue((void*) (long) letter_peg_1131);
            } else {
                goto out_peg_1130;
            }
            
        }
        goto success_peg_1127;
        out_peg_1130:
        goto out_peg_1132;
        success_peg_1127:
        ;
        
        if (column_peg_1125.chunk3 == 0){
            column_peg_1125.chunk3 = new Chunk3();
        }
        column_peg_1125.chunk3->chunk_alpha_digit = result_peg_1126;
        stream.update(result_peg_1126.getPosition());
        
        
        return result_peg_1126;
        out_peg_1132:
        Result result_peg_1133(myposition);
        
        {
            
            char letter_peg_1138 = stream.get(result_peg_1133.getPosition());
            if (letter_peg_1138 != '\0' && strchr("0123456789", letter_peg_1138) != NULL){
                result_peg_1133.nextPosition();
                result_peg_1133.setValue((void*) (long) letter_peg_1138);
            } else {
                goto out_peg_1137;
            }
            
        }
        goto success_peg_1134;
        out_peg_1137:
        goto out_peg_1139;
        success_peg_1134:
        ;
        
        if (column_peg_1125.chunk3 == 0){
            column_peg_1125.chunk3 = new Chunk3();
        }
        column_peg_1125.chunk3->chunk_alpha_digit = result_peg_1133;
        stream.update(result_peg_1133.getPosition());
        
        
        return result_peg_1133;
        out_peg_1139:
    
        if (column_peg_1125.chunk3 == 0){
            column_peg_1125.chunk3 = new Chunk3();
        }
        column_peg_1125.chunk3->chunk_alpha_digit = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_1141 = stream.getColumn(position);
    if (column_peg_1141.chunk3 != 0 && column_peg_1141.chunk3->chunk_valuelist.calculated()){
        return column_peg_1141.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1259(stream, "valuelist");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_1142(myposition);
        
        {
        
            result_peg_1142 = rule_expr_c(stream, result_peg_1142.getPosition());
                if (result_peg_1142.error()){
                    goto out_peg_1144;
                }
                first = result_peg_1142.getValues();
            
            
            
            result_peg_1142.reset();
                do{
                    Result result_peg_1147(result_peg_1142.getPosition());
                    {
                    
                        {
                                
                                result_peg_1147.reset();
                                do{
                                    Result result_peg_1159(result_peg_1147.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1159.getPosition()))){
                                                result_peg_1159.nextPosition();
                                            } else {
                                                goto out_peg_1162;
                                            }
                                        }
                                        result_peg_1159.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1160;
                                    out_peg_1162:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1159.getPosition()))){
                                                result_peg_1159.nextPosition();
                                            } else {
                                                goto out_peg_1164;
                                            }
                                        }
                                        result_peg_1159.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1160;
                                    out_peg_1164:
                                    goto loop_peg_1158;
                                    success_peg_1160:
                                    ;
                                    result_peg_1147.addResult(result_peg_1159);
                                } while (true);
                                loop_peg_1158:
                                ;
                                        
                            }
                            goto success_peg_1149;
                            goto loop_peg_1146;
                            success_peg_1149:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1147.getPosition()))){
                                    result_peg_1147.nextPosition();
                                } else {
                                    goto loop_peg_1146;
                                }
                            }
                            result_peg_1147.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1147.reset();
                                do{
                                    Result result_peg_1177(result_peg_1147.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1177.getPosition()))){
                                                result_peg_1177.nextPosition();
                                            } else {
                                                goto out_peg_1180;
                                            }
                                        }
                                        result_peg_1177.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1178;
                                    out_peg_1180:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1177.getPosition()))){
                                                result_peg_1177.nextPosition();
                                            } else {
                                                goto out_peg_1182;
                                            }
                                        }
                                        result_peg_1177.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1178;
                                    out_peg_1182:
                                    goto loop_peg_1176;
                                    success_peg_1178:
                                    ;
                                    result_peg_1147.addResult(result_peg_1177);
                                } while (true);
                                loop_peg_1176:
                                ;
                                        
                            }
                            goto success_peg_1167;
                            goto loop_peg_1146;
                            success_peg_1167:
                            ;
                        
                        
                        
                        result_peg_1147 = rule_expr_c(stream, result_peg_1147.getPosition());
                            if (result_peg_1147.error()){
                                goto loop_peg_1146;
                            }
                        
                        
                    }
                    result_peg_1142.addResult(result_peg_1147);
                } while (true);
                loop_peg_1146:
                if (result_peg_1142.matches() == 0){
                    goto out_peg_1144;
                }
                rest = result_peg_1142.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_1142.setValue(value);
                }
            
            
        }
        
        if (column_peg_1141.chunk3 == 0){
            column_peg_1141.chunk3 = new Chunk3();
        }
        column_peg_1141.chunk3->chunk_valuelist = result_peg_1142;
        stream.update(result_peg_1142.getPosition());
        
        
        return result_peg_1142;
        out_peg_1144:
        Result result_peg_1183(myposition);
        
        {
        
            {
                    
                    result_peg_1183.reset();
                    do{
                        Result result_peg_1195(result_peg_1183.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1195.getPosition()))){
                                    result_peg_1195.nextPosition();
                                } else {
                                    goto out_peg_1198;
                                }
                            }
                            result_peg_1195.setValue((void*) " ");
                                
                        }
                        goto success_peg_1196;
                        out_peg_1198:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1195.getPosition()))){
                                    result_peg_1195.nextPosition();
                                } else {
                                    goto out_peg_1200;
                                }
                            }
                            result_peg_1195.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1196;
                        out_peg_1200:
                        goto loop_peg_1194;
                        success_peg_1196:
                        ;
                        result_peg_1183.addResult(result_peg_1195);
                    } while (true);
                    loop_peg_1194:
                    ;
                            
                }
                goto success_peg_1185;
                goto out_peg_1201;
                success_peg_1185:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1183.getPosition()))){
                        result_peg_1183.nextPosition();
                    } else {
                        goto out_peg_1201;
                    }
                }
                result_peg_1183.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1183.reset();
                    do{
                        Result result_peg_1214(result_peg_1183.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1214.getPosition()))){
                                    result_peg_1214.nextPosition();
                                } else {
                                    goto out_peg_1217;
                                }
                            }
                            result_peg_1214.setValue((void*) " ");
                                
                        }
                        goto success_peg_1215;
                        out_peg_1217:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1214.getPosition()))){
                                    result_peg_1214.nextPosition();
                                } else {
                                    goto out_peg_1219;
                                }
                            }
                            result_peg_1214.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1215;
                        out_peg_1219:
                        goto loop_peg_1213;
                        success_peg_1215:
                        ;
                        result_peg_1183.addResult(result_peg_1214);
                    } while (true);
                    loop_peg_1213:
                    ;
                            
                }
                goto success_peg_1204;
                goto out_peg_1201;
                success_peg_1204:
                ;
            
            
            
            result_peg_1183 = rule_expr_c(stream, result_peg_1183.getPosition());
                if (result_peg_1183.error()){
                    goto out_peg_1201;
                }
                first = result_peg_1183.getValues();
            
            
            
            result_peg_1183.reset();
                do{
                    Result result_peg_1223(result_peg_1183.getPosition());
                    {
                    
                        {
                                
                                result_peg_1223.reset();
                                do{
                                    Result result_peg_1235(result_peg_1223.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1235.getPosition()))){
                                                result_peg_1235.nextPosition();
                                            } else {
                                                goto out_peg_1238;
                                            }
                                        }
                                        result_peg_1235.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1236;
                                    out_peg_1238:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1235.getPosition()))){
                                                result_peg_1235.nextPosition();
                                            } else {
                                                goto out_peg_1240;
                                            }
                                        }
                                        result_peg_1235.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1236;
                                    out_peg_1240:
                                    goto loop_peg_1234;
                                    success_peg_1236:
                                    ;
                                    result_peg_1223.addResult(result_peg_1235);
                                } while (true);
                                loop_peg_1234:
                                ;
                                        
                            }
                            goto success_peg_1225;
                            goto loop_peg_1222;
                            success_peg_1225:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1223.getPosition()))){
                                    result_peg_1223.nextPosition();
                                } else {
                                    goto loop_peg_1222;
                                }
                            }
                            result_peg_1223.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1223.reset();
                                do{
                                    Result result_peg_1253(result_peg_1223.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1253.getPosition()))){
                                                result_peg_1253.nextPosition();
                                            } else {
                                                goto out_peg_1256;
                                            }
                                        }
                                        result_peg_1253.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1254;
                                    out_peg_1256:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1253.getPosition()))){
                                                result_peg_1253.nextPosition();
                                            } else {
                                                goto out_peg_1258;
                                            }
                                        }
                                        result_peg_1253.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1254;
                                    out_peg_1258:
                                    goto loop_peg_1252;
                                    success_peg_1254:
                                    ;
                                    result_peg_1223.addResult(result_peg_1253);
                                } while (true);
                                loop_peg_1252:
                                ;
                                        
                            }
                            goto success_peg_1243;
                            goto loop_peg_1222;
                            success_peg_1243:
                            ;
                        
                        
                        
                        result_peg_1223 = rule_expr_c(stream, result_peg_1223.getPosition());
                            if (result_peg_1223.error()){
                                goto loop_peg_1222;
                            }
                        
                        
                    }
                    result_peg_1183.addResult(result_peg_1223);
                } while (true);
                loop_peg_1222:
                ;
                rest = result_peg_1183.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_1183.setValue(value);
                }
            
            
        }
        
        if (column_peg_1141.chunk3 == 0){
            column_peg_1141.chunk3 = new Chunk3();
        }
        column_peg_1141.chunk3->chunk_valuelist = result_peg_1183;
        stream.update(result_peg_1183.getPosition());
        
        
        return result_peg_1183;
        out_peg_1201:
    
        if (column_peg_1141.chunk3 == 0){
            column_peg_1141.chunk3 = new Chunk3();
        }
        column_peg_1141.chunk3->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_1260 = stream.getColumn(position);
    if (column_peg_1260.chunk3 != 0 && column_peg_1260.chunk3->chunk_expr.calculated()){
        return column_peg_1260.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_1287(stream, "expr");
    int myposition = position;
    
    
    
    Result result_peg_1261(myposition);
        
        {
        
            result_peg_1261 = rule_expr_c(stream, result_peg_1261.getPosition());
                if (result_peg_1261.error()){
                    goto out_peg_1263;
                }
            
            Result result_peg_1262 = result_peg_1261;
            
            Result result_peg_1266(result_peg_1261);
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
                        goto not_peg_1265;
                        success_peg_1268:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1266.getPosition()))){
                                result_peg_1266.nextPosition();
                            } else {
                                goto not_peg_1265;
                            }
                        }
                        result_peg_1266.setValue((void*) ",");
                    
                    
                }
                goto out_peg_1263;
                not_peg_1265:
                result_peg_1261.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_1262.getValues();
                    result_peg_1261.setValue(value);
                }
            
            
        }
        
        if (column_peg_1260.chunk3 == 0){
            column_peg_1260.chunk3 = new Chunk3();
        }
        column_peg_1260.chunk3->chunk_expr = result_peg_1261;
        stream.update(result_peg_1261.getPosition());
        
        
        return result_peg_1261;
        out_peg_1263:
        Result result_peg_1284(myposition);
        
        {
        
            result_peg_1284 = rule_valuelist(stream, result_peg_1284.getPosition());
                if (result_peg_1284.error()){
                    goto out_peg_1286;
                }
            
            Result result_peg_1285 = result_peg_1284;
            
            result_peg_1284 = rule_expr2_rest(stream, result_peg_1284.getPosition(), result_peg_1285.getValues());
                if (result_peg_1284.error()){
                    goto out_peg_1286;
                }
            
            
        }
        
        if (column_peg_1260.chunk3 == 0){
            column_peg_1260.chunk3 = new Chunk3();
        }
        column_peg_1260.chunk3->chunk_expr = result_peg_1284;
        stream.update(result_peg_1284.getPosition());
        
        
        return result_peg_1284;
        out_peg_1286:
    
        if (column_peg_1260.chunk3 == 0){
            column_peg_1260.chunk3 = new Chunk3();
        }
        column_peg_1260.chunk3->chunk_expr = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_1288 = stream.getColumn(position);
    if (column_peg_1288.chunk3 != 0 && column_peg_1288.chunk3->chunk_expr_c.calculated()){
        return column_peg_1288.chunk3->chunk_expr_c;
    }
    
    RuleTrace trace_peg_1292(stream, "expr_c");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1289(myposition);
        
        {
        
            result_peg_1289 = rule_expr2(stream, result_peg_1289.getPosition());
                if (result_peg_1289.error()){
                    goto out_peg_1291;
                }
                left = result_peg_1289.getValues();
            
            
            
            result_peg_1289 = rule_expr_rest(stream, result_peg_1289.getPosition(), left);
                if (result_peg_1289.error()){
                    goto out_peg_1291;
                }
            
            
        }
        
        if (column_peg_1288.chunk3 == 0){
            column_peg_1288.chunk3 = new Chunk3();
        }
        column_peg_1288.chunk3->chunk_expr_c = result_peg_1289;
        stream.update(result_peg_1289.getPosition());
        
        
        return result_peg_1289;
        out_peg_1291:
    
        if (column_peg_1288.chunk3 == 0){
            column_peg_1288.chunk3 = new Chunk3();
        }
        column_peg_1288.chunk3->chunk_expr_c = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_1293 = stream.getColumn(position);
    if (column_peg_1293.chunk4 != 0 && column_peg_1293.chunk4->chunk_expr2.calculated()){
        return column_peg_1293.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_1297(stream, "expr2");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1294(myposition);
        
        {
        
            result_peg_1294 = rule_expr3(stream, result_peg_1294.getPosition());
                if (result_peg_1294.error()){
                    goto out_peg_1296;
                }
                left = result_peg_1294.getValues();
            
            
            
            result_peg_1294 = rule_expr2_rest(stream, result_peg_1294.getPosition(), left);
                if (result_peg_1294.error()){
                    goto out_peg_1296;
                }
            
            
        }
        
        if (column_peg_1293.chunk4 == 0){
            column_peg_1293.chunk4 = new Chunk4();
        }
        column_peg_1293.chunk4->chunk_expr2 = result_peg_1294;
        stream.update(result_peg_1294.getPosition());
        
        
        return result_peg_1294;
        out_peg_1296:
    
        if (column_peg_1293.chunk4 == 0){
            column_peg_1293.chunk4 = new Chunk4();
        }
        column_peg_1293.chunk4->chunk_expr2 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_1298 = stream.getColumn(position);
    if (column_peg_1298.chunk4 != 0 && column_peg_1298.chunk4->chunk_expr3.calculated()){
        return column_peg_1298.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_1302(stream, "expr3");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1299(myposition);
        
        {
        
            result_peg_1299 = rule_expr4(stream, result_peg_1299.getPosition());
                if (result_peg_1299.error()){
                    goto out_peg_1301;
                }
                left = result_peg_1299.getValues();
            
            
            
            result_peg_1299 = rule_expr3_rest(stream, result_peg_1299.getPosition(), left);
                if (result_peg_1299.error()){
                    goto out_peg_1301;
                }
            
            
        }
        
        if (column_peg_1298.chunk4 == 0){
            column_peg_1298.chunk4 = new Chunk4();
        }
        column_peg_1298.chunk4->chunk_expr3 = result_peg_1299;
        stream.update(result_peg_1299.getPosition());
        
        
        return result_peg_1299;
        out_peg_1301:
    
        if (column_peg_1298.chunk4 == 0){
            column_peg_1298.chunk4 = new Chunk4();
        }
        column_peg_1298.chunk4->chunk_expr3 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_1303 = stream.getColumn(position);
    if (column_peg_1303.chunk4 != 0 && column_peg_1303.chunk4->chunk_expr4.calculated()){
        return column_peg_1303.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_1307(stream, "expr4");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1304(myposition);
        
        {
        
            result_peg_1304 = rule_expr5(stream, result_peg_1304.getPosition());
                if (result_peg_1304.error()){
                    goto out_peg_1306;
                }
                left = result_peg_1304.getValues();
            
            
            
            result_peg_1304 = rule_expr4_rest(stream, result_peg_1304.getPosition(), left);
                if (result_peg_1304.error()){
                    goto out_peg_1306;
                }
            
            
        }
        
        if (column_peg_1303.chunk4 == 0){
            column_peg_1303.chunk4 = new Chunk4();
        }
        column_peg_1303.chunk4->chunk_expr4 = result_peg_1304;
        stream.update(result_peg_1304.getPosition());
        
        
        return result_peg_1304;
        out_peg_1306:
    
        if (column_peg_1303.chunk4 == 0){
            column_peg_1303.chunk4 = new Chunk4();
        }
        column_peg_1303.chunk4->chunk_expr4 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_1308 = stream.getColumn(position);
    if (column_peg_1308.chunk4 != 0 && column_peg_1308.chunk4->chunk_expr5.calculated()){
        return column_peg_1308.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_1312(stream, "expr5");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1309(myposition);
        
        {
        
            result_peg_1309 = rule_expr6(stream, result_peg_1309.getPosition());
                if (result_peg_1309.error()){
                    goto out_peg_1311;
                }
                left = result_peg_1309.getValues();
            
            
            
            result_peg_1309 = rule_expr5_rest(stream, result_peg_1309.getPosition(), left);
                if (result_peg_1309.error()){
                    goto out_peg_1311;
                }
            
            
        }
        
        if (column_peg_1308.chunk4 == 0){
            column_peg_1308.chunk4 = new Chunk4();
        }
        column_peg_1308.chunk4->chunk_expr5 = result_peg_1309;
        stream.update(result_peg_1309.getPosition());
        
        
        return result_peg_1309;
        out_peg_1311:
    
        if (column_peg_1308.chunk4 == 0){
            column_peg_1308.chunk4 = new Chunk4();
        }
        column_peg_1308.chunk4->chunk_expr5 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_1313 = stream.getColumn(position);
    if (column_peg_1313.chunk4 != 0 && column_peg_1313.chunk4->chunk_expr6.calculated()){
        return column_peg_1313.chunk4->chunk_expr6;
    }
    
    RuleTrace trace_peg_1317(stream, "expr6");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1314(myposition);
        
        {
        
            result_peg_1314 = rule_expr7(stream, result_peg_1314.getPosition());
                if (result_peg_1314.error()){
                    goto out_peg_1316;
                }
                left = result_peg_1314.getValues();
            
            
            
            result_peg_1314 = rule_expr6_rest(stream, result_peg_1314.getPosition(), left);
                if (result_peg_1314.error()){
                    goto out_peg_1316;
                }
            
            
        }
        
        if (column_peg_1313.chunk4 == 0){
            column_peg_1313.chunk4 = new Chunk4();
        }
        column_peg_1313.chunk4->chunk_expr6 = result_peg_1314;
        stream.update(result_peg_1314.getPosition());
        
        
        return result_peg_1314;
        out_peg_1316:
    
        if (column_peg_1313.chunk4 == 0){
            column_peg_1313.chunk4 = new Chunk4();
        }
        column_peg_1313.chunk4->chunk_expr6 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_1318 = stream.getColumn(position);
    if (column_peg_1318.chunk5 != 0 && column_peg_1318.chunk5->chunk_expr7.calculated()){
        return column_peg_1318.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_1322(stream, "expr7");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1319(myposition);
        
        {
        
            result_peg_1319 = rule_expr8(stream, result_peg_1319.getPosition());
                if (result_peg_1319.error()){
                    goto out_peg_1321;
                }
                left = result_peg_1319.getValues();
            
            
            
            result_peg_1319 = rule_expr7_rest(stream, result_peg_1319.getPosition(), left);
                if (result_peg_1319.error()){
                    goto out_peg_1321;
                }
            
            
        }
        
        if (column_peg_1318.chunk5 == 0){
            column_peg_1318.chunk5 = new Chunk5();
        }
        column_peg_1318.chunk5->chunk_expr7 = result_peg_1319;
        stream.update(result_peg_1319.getPosition());
        
        
        return result_peg_1319;
        out_peg_1321:
    
        if (column_peg_1318.chunk5 == 0){
            column_peg_1318.chunk5 = new Chunk5();
        }
        column_peg_1318.chunk5->chunk_expr7 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_1323 = stream.getColumn(position);
    if (column_peg_1323.chunk5 != 0 && column_peg_1323.chunk5->chunk_expr8.calculated()){
        return column_peg_1323.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_1327(stream, "expr8");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1324(myposition);
        
        {
        
            result_peg_1324 = rule_expr9(stream, result_peg_1324.getPosition());
                if (result_peg_1324.error()){
                    goto out_peg_1326;
                }
                left = result_peg_1324.getValues();
            
            
            
            result_peg_1324 = rule_expr8_rest(stream, result_peg_1324.getPosition(), left);
                if (result_peg_1324.error()){
                    goto out_peg_1326;
                }
            
            
        }
        
        if (column_peg_1323.chunk5 == 0){
            column_peg_1323.chunk5 = new Chunk5();
        }
        column_peg_1323.chunk5->chunk_expr8 = result_peg_1324;
        stream.update(result_peg_1324.getPosition());
        
        
        return result_peg_1324;
        out_peg_1326:
    
        if (column_peg_1323.chunk5 == 0){
            column_peg_1323.chunk5 = new Chunk5();
        }
        column_peg_1323.chunk5->chunk_expr8 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_1328 = stream.getColumn(position);
    if (column_peg_1328.chunk5 != 0 && column_peg_1328.chunk5->chunk_expr9.calculated()){
        return column_peg_1328.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_1332(stream, "expr9");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1329(myposition);
        
        {
        
            result_peg_1329 = rule_expr10(stream, result_peg_1329.getPosition());
                if (result_peg_1329.error()){
                    goto out_peg_1331;
                }
                left = result_peg_1329.getValues();
            
            
            
            result_peg_1329 = rule_expr9_rest(stream, result_peg_1329.getPosition(), left);
                if (result_peg_1329.error()){
                    goto out_peg_1331;
                }
            
            
        }
        
        if (column_peg_1328.chunk5 == 0){
            column_peg_1328.chunk5 = new Chunk5();
        }
        column_peg_1328.chunk5->chunk_expr9 = result_peg_1329;
        stream.update(result_peg_1329.getPosition());
        
        
        return result_peg_1329;
        out_peg_1331:
    
        if (column_peg_1328.chunk5 == 0){
            column_peg_1328.chunk5 = new Chunk5();
        }
        column_peg_1328.chunk5->chunk_expr9 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_1333 = stream.getColumn(position);
    if (column_peg_1333.chunk5 != 0 && column_peg_1333.chunk5->chunk_expr10.calculated()){
        return column_peg_1333.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_1337(stream, "expr10");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1334(myposition);
        
        {
        
            result_peg_1334 = rule_expr11(stream, result_peg_1334.getPosition());
                if (result_peg_1334.error()){
                    goto out_peg_1336;
                }
                left = result_peg_1334.getValues();
            
            
            
            result_peg_1334 = rule_expr10_rest(stream, result_peg_1334.getPosition(), left);
                if (result_peg_1334.error()){
                    goto out_peg_1336;
                }
            
            
        }
        
        if (column_peg_1333.chunk5 == 0){
            column_peg_1333.chunk5 = new Chunk5();
        }
        column_peg_1333.chunk5->chunk_expr10 = result_peg_1334;
        stream.update(result_peg_1334.getPosition());
        
        
        return result_peg_1334;
        out_peg_1336:
    
        if (column_peg_1333.chunk5 == 0){
            column_peg_1333.chunk5 = new Chunk5();
        }
        column_peg_1333.chunk5->chunk_expr10 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_1338 = stream.getColumn(position);
    if (column_peg_1338.chunk5 != 0 && column_peg_1338.chunk5->chunk_expr11.calculated()){
        return column_peg_1338.chunk5->chunk_expr11;
    }
    
    RuleTrace trace_peg_1342(stream, "expr11");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1339(myposition);
        
        {
        
            result_peg_1339 = rule_expr12(stream, result_peg_1339.getPosition());
                if (result_peg_1339.error()){
                    goto out_peg_1341;
                }
                left = result_peg_1339.getValues();
            
            
            
            result_peg_1339 = rule_expr11_rest(stream, result_peg_1339.getPosition(), left);
                if (result_peg_1339.error()){
                    goto out_peg_1341;
                }
            
            
        }
        
        if (column_peg_1338.chunk5 == 0){
            column_peg_1338.chunk5 = new Chunk5();
        }
        column_peg_1338.chunk5->chunk_expr11 = result_peg_1339;
        stream.update(result_peg_1339.getPosition());
        
        
        return result_peg_1339;
        out_peg_1341:
    
        if (column_peg_1338.chunk5 == 0){
            column_peg_1338.chunk5 = new Chunk5();
        }
        column_peg_1338.chunk5->chunk_expr11 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_1343 = stream.getColumn(position);
    if (column_peg_1343.chunk6 != 0 && column_peg_1343.chunk6->chunk_expr12.calculated()){
        return column_peg_1343.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_1347(stream, "expr12");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1344(myposition);
        
        {
        
            result_peg_1344 = rule_expr13(stream, result_peg_1344.getPosition());
                if (result_peg_1344.error()){
                    goto out_peg_1346;
                }
                left = result_peg_1344.getValues();
            
            
            
            result_peg_1344 = rule_expr12_rest(stream, result_peg_1344.getPosition(), left);
                if (result_peg_1344.error()){
                    goto out_peg_1346;
                }
            
            
        }
        
        if (column_peg_1343.chunk6 == 0){
            column_peg_1343.chunk6 = new Chunk6();
        }
        column_peg_1343.chunk6->chunk_expr12 = result_peg_1344;
        stream.update(result_peg_1344.getPosition());
        
        
        return result_peg_1344;
        out_peg_1346:
    
        if (column_peg_1343.chunk6 == 0){
            column_peg_1343.chunk6 = new Chunk6();
        }
        column_peg_1343.chunk6->chunk_expr12 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_1348 = stream.getColumn(position);
    if (column_peg_1348.chunk6 != 0 && column_peg_1348.chunk6->chunk_expr13.calculated()){
        return column_peg_1348.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_1372(stream, "expr13");
    int myposition = position;
    
    
    Value exp;
    Result result_peg_1349(myposition);
        
        {
        
            result_peg_1349.reset();
                do{
                    Result result_peg_1352(result_peg_1349.getPosition());
                    result_peg_1352 = rule_unary(stream, result_peg_1352.getPosition());
                    if (result_peg_1352.error()){
                        goto loop_peg_1351;
                    }
                    result_peg_1349.addResult(result_peg_1352);
                } while (true);
                loop_peg_1351:
                ;
            
            Result result_peg_1350 = result_peg_1349;
            
            {
                    
                    result_peg_1349.reset();
                    do{
                        Result result_peg_1364(result_peg_1349.getPosition());
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
                        result_peg_1349.addResult(result_peg_1364);
                    } while (true);
                    loop_peg_1363:
                    ;
                            
                }
                goto success_peg_1354;
                goto out_peg_1370;
                success_peg_1354:
                ;
            
            
            
            result_peg_1349 = rule_expr13_real(stream, result_peg_1349.getPosition());
                if (result_peg_1349.error()){
                    goto out_peg_1370;
                }
                exp = result_peg_1349.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeUnaryExpression(result_peg_1350.getValues(), exp);
                    result_peg_1349.setValue(value);
                }
            
            
        }
        
        if (column_peg_1348.chunk6 == 0){
            column_peg_1348.chunk6 = new Chunk6();
        }
        column_peg_1348.chunk6->chunk_expr13 = result_peg_1349;
        stream.update(result_peg_1349.getPosition());
        
        
        return result_peg_1349;
        out_peg_1370:
    
        if (column_peg_1348.chunk6 == 0){
            column_peg_1348.chunk6 = new Chunk6();
        }
        column_peg_1348.chunk6->chunk_expr13 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1416(stream, "expr_rest");
    int myposition = position;
    
    tail_peg_1375:
    Value right;
        Value new_left;
    Result result_peg_1374(myposition);
        {
        
            {
                    
                    result_peg_1374.reset();
                    do{
                        Result result_peg_1387(result_peg_1374.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1387.getPosition()))){
                                    result_peg_1387.nextPosition();
                                } else {
                                    goto out_peg_1390;
                                }
                            }
                            result_peg_1387.setValue((void*) " ");
                                
                        }
                        goto success_peg_1388;
                        out_peg_1390:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1387.getPosition()))){
                                    result_peg_1387.nextPosition();
                                } else {
                                    goto out_peg_1392;
                                }
                            }
                            result_peg_1387.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1388;
                        out_peg_1392:
                        goto loop_peg_1386;
                        success_peg_1388:
                        ;
                        result_peg_1374.addResult(result_peg_1387);
                    } while (true);
                    loop_peg_1386:
                    ;
                            
                }
                goto success_peg_1377;
                goto out_peg_1393;
                success_peg_1377:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("||"[i], stream.get(result_peg_1374.getPosition()))){
                        result_peg_1374.nextPosition();
                    } else {
                        goto out_peg_1393;
                    }
                }
                result_peg_1374.setValue((void*) "||");
            
            
            
            {
                    
                    result_peg_1374.reset();
                    do{
                        Result result_peg_1406(result_peg_1374.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1406.getPosition()))){
                                    result_peg_1406.nextPosition();
                                } else {
                                    goto out_peg_1409;
                                }
                            }
                            result_peg_1406.setValue((void*) " ");
                                
                        }
                        goto success_peg_1407;
                        out_peg_1409:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1406.getPosition()))){
                                    result_peg_1406.nextPosition();
                                } else {
                                    goto out_peg_1411;
                                }
                            }
                            result_peg_1406.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1407;
                        out_peg_1411:
                        goto loop_peg_1405;
                        success_peg_1407:
                        ;
                        result_peg_1374.addResult(result_peg_1406);
                    } while (true);
                    loop_peg_1405:
                    ;
                            
                }
                goto success_peg_1396;
                goto out_peg_1393;
                success_peg_1396:
                ;
            
            
            
            result_peg_1374 = rule_expr2(stream, result_peg_1374.getPosition());
                if (result_peg_1374.error()){
                    goto out_peg_1393;
                }
                right = result_peg_1374.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionOr(left, right);
                    result_peg_1374.setValue(value);
                }
                new_left = result_peg_1374.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1374.getPosition();
        goto tail_peg_1375;
        out_peg_1393:
        Result result_peg_1414(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1414.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1414.getPosition());
        
        return result_peg_1414;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1460(stream, "expr2_rest");
    int myposition = position;
    
    tail_peg_1419:
    Value right;
        Value new_left;
    Result result_peg_1418(myposition);
        {
        
            {
                    
                    result_peg_1418.reset();
                    do{
                        Result result_peg_1431(result_peg_1418.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1431.getPosition()))){
                                    result_peg_1431.nextPosition();
                                } else {
                                    goto out_peg_1434;
                                }
                            }
                            result_peg_1431.setValue((void*) " ");
                                
                        }
                        goto success_peg_1432;
                        out_peg_1434:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1431.getPosition()))){
                                    result_peg_1431.nextPosition();
                                } else {
                                    goto out_peg_1436;
                                }
                            }
                            result_peg_1431.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1432;
                        out_peg_1436:
                        goto loop_peg_1430;
                        success_peg_1432:
                        ;
                        result_peg_1418.addResult(result_peg_1431);
                    } while (true);
                    loop_peg_1430:
                    ;
                            
                }
                goto success_peg_1421;
                goto out_peg_1437;
                success_peg_1421:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("^^"[i], stream.get(result_peg_1418.getPosition()))){
                        result_peg_1418.nextPosition();
                    } else {
                        goto out_peg_1437;
                    }
                }
                result_peg_1418.setValue((void*) "^^");
            
            
            
            {
                    
                    result_peg_1418.reset();
                    do{
                        Result result_peg_1450(result_peg_1418.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1450.getPosition()))){
                                    result_peg_1450.nextPosition();
                                } else {
                                    goto out_peg_1453;
                                }
                            }
                            result_peg_1450.setValue((void*) " ");
                                
                        }
                        goto success_peg_1451;
                        out_peg_1453:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1450.getPosition()))){
                                    result_peg_1450.nextPosition();
                                } else {
                                    goto out_peg_1455;
                                }
                            }
                            result_peg_1450.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1451;
                        out_peg_1455:
                        goto loop_peg_1449;
                        success_peg_1451:
                        ;
                        result_peg_1418.addResult(result_peg_1450);
                    } while (true);
                    loop_peg_1449:
                    ;
                            
                }
                goto success_peg_1440;
                goto out_peg_1437;
                success_peg_1440:
                ;
            
            
            
            result_peg_1418 = rule_expr3(stream, result_peg_1418.getPosition());
                if (result_peg_1418.error()){
                    goto out_peg_1437;
                }
                right = result_peg_1418.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionXOr(left, right);
                    result_peg_1418.setValue(value);
                }
                new_left = result_peg_1418.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1418.getPosition();
        goto tail_peg_1419;
        out_peg_1437:
        Result result_peg_1458(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1458.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1458.getPosition());
        
        return result_peg_1458;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1504(stream, "expr3_rest");
    int myposition = position;
    
    tail_peg_1463:
    Value right;
        Value new_left;
    Result result_peg_1462(myposition);
        {
        
            {
                    
                    result_peg_1462.reset();
                    do{
                        Result result_peg_1475(result_peg_1462.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1475.getPosition()))){
                                    result_peg_1475.nextPosition();
                                } else {
                                    goto out_peg_1478;
                                }
                            }
                            result_peg_1475.setValue((void*) " ");
                                
                        }
                        goto success_peg_1476;
                        out_peg_1478:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1475.getPosition()))){
                                    result_peg_1475.nextPosition();
                                } else {
                                    goto out_peg_1480;
                                }
                            }
                            result_peg_1475.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1476;
                        out_peg_1480:
                        goto loop_peg_1474;
                        success_peg_1476:
                        ;
                        result_peg_1462.addResult(result_peg_1475);
                    } while (true);
                    loop_peg_1474:
                    ;
                            
                }
                goto success_peg_1465;
                goto out_peg_1481;
                success_peg_1465:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("&&"[i], stream.get(result_peg_1462.getPosition()))){
                        result_peg_1462.nextPosition();
                    } else {
                        goto out_peg_1481;
                    }
                }
                result_peg_1462.setValue((void*) "&&");
            
            
            
            {
                    
                    result_peg_1462.reset();
                    do{
                        Result result_peg_1494(result_peg_1462.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1494.getPosition()))){
                                    result_peg_1494.nextPosition();
                                } else {
                                    goto out_peg_1497;
                                }
                            }
                            result_peg_1494.setValue((void*) " ");
                                
                        }
                        goto success_peg_1495;
                        out_peg_1497:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1494.getPosition()))){
                                    result_peg_1494.nextPosition();
                                } else {
                                    goto out_peg_1499;
                                }
                            }
                            result_peg_1494.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1495;
                        out_peg_1499:
                        goto loop_peg_1493;
                        success_peg_1495:
                        ;
                        result_peg_1462.addResult(result_peg_1494);
                    } while (true);
                    loop_peg_1493:
                    ;
                            
                }
                goto success_peg_1484;
                goto out_peg_1481;
                success_peg_1484:
                ;
            
            
            
            result_peg_1462 = rule_expr4(stream, result_peg_1462.getPosition());
                if (result_peg_1462.error()){
                    goto out_peg_1481;
                }
                right = result_peg_1462.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAnd(left, right);
                    result_peg_1462.setValue(value);
                }
                new_left = result_peg_1462.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1462.getPosition();
        goto tail_peg_1463;
        out_peg_1481:
        Result result_peg_1502(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1502.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1502.getPosition());
        
        return result_peg_1502;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1548(stream, "expr4_rest");
    int myposition = position;
    
    tail_peg_1507:
    Value right;
        Value new_left;
    Result result_peg_1506(myposition);
        {
        
            {
                    
                    result_peg_1506.reset();
                    do{
                        Result result_peg_1519(result_peg_1506.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1519.getPosition()))){
                                    result_peg_1519.nextPosition();
                                } else {
                                    goto out_peg_1522;
                                }
                            }
                            result_peg_1519.setValue((void*) " ");
                                
                        }
                        goto success_peg_1520;
                        out_peg_1522:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1519.getPosition()))){
                                    result_peg_1519.nextPosition();
                                } else {
                                    goto out_peg_1524;
                                }
                            }
                            result_peg_1519.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1520;
                        out_peg_1524:
                        goto loop_peg_1518;
                        success_peg_1520:
                        ;
                        result_peg_1506.addResult(result_peg_1519);
                    } while (true);
                    loop_peg_1518:
                    ;
                            
                }
                goto success_peg_1509;
                goto out_peg_1525;
                success_peg_1509:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("|"[i], stream.get(result_peg_1506.getPosition()))){
                        result_peg_1506.nextPosition();
                    } else {
                        goto out_peg_1525;
                    }
                }
                result_peg_1506.setValue((void*) "|");
            
            
            
            {
                    
                    result_peg_1506.reset();
                    do{
                        Result result_peg_1538(result_peg_1506.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1538.getPosition()))){
                                    result_peg_1538.nextPosition();
                                } else {
                                    goto out_peg_1541;
                                }
                            }
                            result_peg_1538.setValue((void*) " ");
                                
                        }
                        goto success_peg_1539;
                        out_peg_1541:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1538.getPosition()))){
                                    result_peg_1538.nextPosition();
                                } else {
                                    goto out_peg_1543;
                                }
                            }
                            result_peg_1538.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1539;
                        out_peg_1543:
                        goto loop_peg_1537;
                        success_peg_1539:
                        ;
                        result_peg_1506.addResult(result_peg_1538);
                    } while (true);
                    loop_peg_1537:
                    ;
                            
                }
                goto success_peg_1528;
                goto out_peg_1525;
                success_peg_1528:
                ;
            
            
            
            result_peg_1506 = rule_expr5(stream, result_peg_1506.getPosition());
                if (result_peg_1506.error()){
                    goto out_peg_1525;
                }
                right = result_peg_1506.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseOr(left, right);
                    result_peg_1506.setValue(value);
                }
                new_left = result_peg_1506.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1506.getPosition();
        goto tail_peg_1507;
        out_peg_1525:
        Result result_peg_1546(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1546.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1546.getPosition());
        
        return result_peg_1546;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1592(stream, "expr5_rest");
    int myposition = position;
    
    tail_peg_1551:
    Value right;
        Value new_left;
    Result result_peg_1550(myposition);
        {
        
            {
                    
                    result_peg_1550.reset();
                    do{
                        Result result_peg_1563(result_peg_1550.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1563.getPosition()))){
                                    result_peg_1563.nextPosition();
                                } else {
                                    goto out_peg_1566;
                                }
                            }
                            result_peg_1563.setValue((void*) " ");
                                
                        }
                        goto success_peg_1564;
                        out_peg_1566:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1563.getPosition()))){
                                    result_peg_1563.nextPosition();
                                } else {
                                    goto out_peg_1568;
                                }
                            }
                            result_peg_1563.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1564;
                        out_peg_1568:
                        goto loop_peg_1562;
                        success_peg_1564:
                        ;
                        result_peg_1550.addResult(result_peg_1563);
                    } while (true);
                    loop_peg_1562:
                    ;
                            
                }
                goto success_peg_1553;
                goto out_peg_1569;
                success_peg_1553:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("^"[i], stream.get(result_peg_1550.getPosition()))){
                        result_peg_1550.nextPosition();
                    } else {
                        goto out_peg_1569;
                    }
                }
                result_peg_1550.setValue((void*) "^");
            
            
            
            {
                    
                    result_peg_1550.reset();
                    do{
                        Result result_peg_1582(result_peg_1550.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1582.getPosition()))){
                                    result_peg_1582.nextPosition();
                                } else {
                                    goto out_peg_1585;
                                }
                            }
                            result_peg_1582.setValue((void*) " ");
                                
                        }
                        goto success_peg_1583;
                        out_peg_1585:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1582.getPosition()))){
                                    result_peg_1582.nextPosition();
                                } else {
                                    goto out_peg_1587;
                                }
                            }
                            result_peg_1582.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1583;
                        out_peg_1587:
                        goto loop_peg_1581;
                        success_peg_1583:
                        ;
                        result_peg_1550.addResult(result_peg_1582);
                    } while (true);
                    loop_peg_1581:
                    ;
                            
                }
                goto success_peg_1572;
                goto out_peg_1569;
                success_peg_1572:
                ;
            
            
            
            result_peg_1550 = rule_expr6(stream, result_peg_1550.getPosition());
                if (result_peg_1550.error()){
                    goto out_peg_1569;
                }
                right = result_peg_1550.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseXOr(left, right);
                    result_peg_1550.setValue(value);
                }
                new_left = result_peg_1550.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1550.getPosition();
        goto tail_peg_1551;
        out_peg_1569:
        Result result_peg_1590(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1590.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1590.getPosition());
        
        return result_peg_1590;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1636(stream, "expr6_rest");
    int myposition = position;
    
    tail_peg_1595:
    Value right;
        Value new_left;
    Result result_peg_1594(myposition);
        {
        
            {
                    
                    result_peg_1594.reset();
                    do{
                        Result result_peg_1607(result_peg_1594.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1607.getPosition()))){
                                    result_peg_1607.nextPosition();
                                } else {
                                    goto out_peg_1610;
                                }
                            }
                            result_peg_1607.setValue((void*) " ");
                                
                        }
                        goto success_peg_1608;
                        out_peg_1610:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1607.getPosition()))){
                                    result_peg_1607.nextPosition();
                                } else {
                                    goto out_peg_1612;
                                }
                            }
                            result_peg_1607.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1608;
                        out_peg_1612:
                        goto loop_peg_1606;
                        success_peg_1608:
                        ;
                        result_peg_1594.addResult(result_peg_1607);
                    } while (true);
                    loop_peg_1606:
                    ;
                            
                }
                goto success_peg_1597;
                goto out_peg_1613;
                success_peg_1597:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("&"[i], stream.get(result_peg_1594.getPosition()))){
                        result_peg_1594.nextPosition();
                    } else {
                        goto out_peg_1613;
                    }
                }
                result_peg_1594.setValue((void*) "&");
            
            
            
            {
                    
                    result_peg_1594.reset();
                    do{
                        Result result_peg_1626(result_peg_1594.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1626.getPosition()))){
                                    result_peg_1626.nextPosition();
                                } else {
                                    goto out_peg_1629;
                                }
                            }
                            result_peg_1626.setValue((void*) " ");
                                
                        }
                        goto success_peg_1627;
                        out_peg_1629:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1626.getPosition()))){
                                    result_peg_1626.nextPosition();
                                } else {
                                    goto out_peg_1631;
                                }
                            }
                            result_peg_1626.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1627;
                        out_peg_1631:
                        goto loop_peg_1625;
                        success_peg_1627:
                        ;
                        result_peg_1594.addResult(result_peg_1626);
                    } while (true);
                    loop_peg_1625:
                    ;
                            
                }
                goto success_peg_1616;
                goto out_peg_1613;
                success_peg_1616:
                ;
            
            
            
            result_peg_1594 = rule_expr7(stream, result_peg_1594.getPosition());
                if (result_peg_1594.error()){
                    goto out_peg_1613;
                }
                right = result_peg_1594.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseAnd(left, right);
                    result_peg_1594.setValue(value);
                }
                new_left = result_peg_1594.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1594.getPosition();
        goto tail_peg_1595;
        out_peg_1613:
        Result result_peg_1634(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1634.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1634.getPosition());
        
        return result_peg_1634;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1680(stream, "expr7_rest");
    int myposition = position;
    
    tail_peg_1639:
    Value right;
        Value new_left;
    Result result_peg_1638(myposition);
        {
        
            {
                    
                    result_peg_1638.reset();
                    do{
                        Result result_peg_1651(result_peg_1638.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1651.getPosition()))){
                                    result_peg_1651.nextPosition();
                                } else {
                                    goto out_peg_1654;
                                }
                            }
                            result_peg_1651.setValue((void*) " ");
                                
                        }
                        goto success_peg_1652;
                        out_peg_1654:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1651.getPosition()))){
                                    result_peg_1651.nextPosition();
                                } else {
                                    goto out_peg_1656;
                                }
                            }
                            result_peg_1651.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1652;
                        out_peg_1656:
                        goto loop_peg_1650;
                        success_peg_1652:
                        ;
                        result_peg_1638.addResult(result_peg_1651);
                    } while (true);
                    loop_peg_1650:
                    ;
                            
                }
                goto success_peg_1641;
                goto out_peg_1657;
                success_peg_1641:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar(":="[i], stream.get(result_peg_1638.getPosition()))){
                        result_peg_1638.nextPosition();
                    } else {
                        goto out_peg_1657;
                    }
                }
                result_peg_1638.setValue((void*) ":=");
            
            
            
            {
                    
                    result_peg_1638.reset();
                    do{
                        Result result_peg_1670(result_peg_1638.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1670.getPosition()))){
                                    result_peg_1670.nextPosition();
                                } else {
                                    goto out_peg_1673;
                                }
                            }
                            result_peg_1670.setValue((void*) " ");
                                
                        }
                        goto success_peg_1671;
                        out_peg_1673:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1670.getPosition()))){
                                    result_peg_1670.nextPosition();
                                } else {
                                    goto out_peg_1675;
                                }
                            }
                            result_peg_1670.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1671;
                        out_peg_1675:
                        goto loop_peg_1669;
                        success_peg_1671:
                        ;
                        result_peg_1638.addResult(result_peg_1670);
                    } while (true);
                    loop_peg_1669:
                    ;
                            
                }
                goto success_peg_1660;
                goto out_peg_1657;
                success_peg_1660:
                ;
            
            
            
            result_peg_1638 = rule_expr8(stream, result_peg_1638.getPosition());
                if (result_peg_1638.error()){
                    goto out_peg_1657;
                }
                right = result_peg_1638.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAssignment(left, right);
                    result_peg_1638.setValue(value);
                }
                new_left = result_peg_1638.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1638.getPosition();
        goto tail_peg_1639;
        out_peg_1657:
        Result result_peg_1678(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1678.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1678.getPosition());
        
        return result_peg_1678;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1763(stream, "expr8_rest");
    int myposition = position;
    
    tail_peg_1683:
    Value right;
        Value new_left;
    Result result_peg_1682(myposition);
        {
        
            {
                    
                    result_peg_1682.reset();
                    do{
                        Result result_peg_1695(result_peg_1682.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1695.getPosition()))){
                                    result_peg_1695.nextPosition();
                                } else {
                                    goto out_peg_1698;
                                }
                            }
                            result_peg_1695.setValue((void*) " ");
                                
                        }
                        goto success_peg_1696;
                        out_peg_1698:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1695.getPosition()))){
                                    result_peg_1695.nextPosition();
                                } else {
                                    goto out_peg_1700;
                                }
                            }
                            result_peg_1695.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1696;
                        out_peg_1700:
                        goto loop_peg_1694;
                        success_peg_1696:
                        ;
                        result_peg_1682.addResult(result_peg_1695);
                    } while (true);
                    loop_peg_1694:
                    ;
                            
                }
                goto success_peg_1685;
                goto out_peg_1701;
                success_peg_1685:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1682.getPosition()))){
                        result_peg_1682.nextPosition();
                    } else {
                        goto out_peg_1701;
                    }
                }
                result_peg_1682.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1682.reset();
                    do{
                        Result result_peg_1714(result_peg_1682.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1714.getPosition()))){
                                    result_peg_1714.nextPosition();
                                } else {
                                    goto out_peg_1717;
                                }
                            }
                            result_peg_1714.setValue((void*) " ");
                                
                        }
                        goto success_peg_1715;
                        out_peg_1717:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1714.getPosition()))){
                                    result_peg_1714.nextPosition();
                                } else {
                                    goto out_peg_1719;
                                }
                            }
                            result_peg_1714.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1715;
                        out_peg_1719:
                        goto loop_peg_1713;
                        success_peg_1715:
                        ;
                        result_peg_1682.addResult(result_peg_1714);
                    } while (true);
                    loop_peg_1713:
                    ;
                            
                }
                goto success_peg_1704;
                goto out_peg_1701;
                success_peg_1704:
                ;
            
            
            
            result_peg_1682 = rule_expr9(stream, result_peg_1682.getPosition());
                if (result_peg_1682.error()){
                    goto out_peg_1701;
                }
                right = result_peg_1682.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionEquals(left, right);
                    result_peg_1682.setValue(value);
                }
                new_left = result_peg_1682.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1682.getPosition();
        goto tail_peg_1683;
        out_peg_1701:
        Result result_peg_1722(myposition);
        {
        
            {
                    
                    result_peg_1722.reset();
                    do{
                        Result result_peg_1734(result_peg_1722.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1734.getPosition()))){
                                    result_peg_1734.nextPosition();
                                } else {
                                    goto out_peg_1737;
                                }
                            }
                            result_peg_1734.setValue((void*) " ");
                                
                        }
                        goto success_peg_1735;
                        out_peg_1737:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1734.getPosition()))){
                                    result_peg_1734.nextPosition();
                                } else {
                                    goto out_peg_1739;
                                }
                            }
                            result_peg_1734.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1735;
                        out_peg_1739:
                        goto loop_peg_1733;
                        success_peg_1735:
                        ;
                        result_peg_1722.addResult(result_peg_1734);
                    } while (true);
                    loop_peg_1733:
                    ;
                            
                }
                goto success_peg_1724;
                goto out_peg_1740;
                success_peg_1724:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_1722.getPosition()))){
                        result_peg_1722.nextPosition();
                    } else {
                        goto out_peg_1740;
                    }
                }
                result_peg_1722.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_1722.reset();
                    do{
                        Result result_peg_1753(result_peg_1722.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1753.getPosition()))){
                                    result_peg_1753.nextPosition();
                                } else {
                                    goto out_peg_1756;
                                }
                            }
                            result_peg_1753.setValue((void*) " ");
                                
                        }
                        goto success_peg_1754;
                        out_peg_1756:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1753.getPosition()))){
                                    result_peg_1753.nextPosition();
                                } else {
                                    goto out_peg_1758;
                                }
                            }
                            result_peg_1753.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1754;
                        out_peg_1758:
                        goto loop_peg_1752;
                        success_peg_1754:
                        ;
                        result_peg_1722.addResult(result_peg_1753);
                    } while (true);
                    loop_peg_1752:
                    ;
                            
                }
                goto success_peg_1743;
                goto out_peg_1740;
                success_peg_1743:
                ;
            
            
            
            result_peg_1722 = rule_expr9(stream, result_peg_1722.getPosition());
                if (result_peg_1722.error()){
                    goto out_peg_1740;
                }
                right = result_peg_1722.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionUnequals(left, right);
                    result_peg_1722.setValue(value);
                }
                new_left = result_peg_1722.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1722.getPosition();
        goto tail_peg_1683;
        out_peg_1740:
        Result result_peg_1761(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1761.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1761.getPosition());
        
        return result_peg_1761;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1807(stream, "expr9_rest");
    int myposition = position;
    
    tail_peg_1766:
    Value maker;
        Value right;
        Value new_left;
    Result result_peg_1765(myposition);
        {
        
            {
                    
                    result_peg_1765.reset();
                    do{
                        Result result_peg_1778(result_peg_1765.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1778.getPosition()))){
                                    result_peg_1778.nextPosition();
                                } else {
                                    goto out_peg_1781;
                                }
                            }
                            result_peg_1778.setValue((void*) " ");
                                
                        }
                        goto success_peg_1779;
                        out_peg_1781:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1778.getPosition()))){
                                    result_peg_1778.nextPosition();
                                } else {
                                    goto out_peg_1783;
                                }
                            }
                            result_peg_1778.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1779;
                        out_peg_1783:
                        goto loop_peg_1777;
                        success_peg_1779:
                        ;
                        result_peg_1765.addResult(result_peg_1778);
                    } while (true);
                    loop_peg_1777:
                    ;
                            
                }
                goto success_peg_1768;
                goto out_peg_1784;
                success_peg_1768:
                ;
            
            
            
            result_peg_1765 = rule_compare(stream, result_peg_1765.getPosition());
                if (result_peg_1765.error()){
                    goto out_peg_1784;
                }
                maker = result_peg_1765.getValues();
            
            
            
            {
                    
                    result_peg_1765.reset();
                    do{
                        Result result_peg_1797(result_peg_1765.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1797.getPosition()))){
                                    result_peg_1797.nextPosition();
                                } else {
                                    goto out_peg_1800;
                                }
                            }
                            result_peg_1797.setValue((void*) " ");
                                
                        }
                        goto success_peg_1798;
                        out_peg_1800:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1797.getPosition()))){
                                    result_peg_1797.nextPosition();
                                } else {
                                    goto out_peg_1802;
                                }
                            }
                            result_peg_1797.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1798;
                        out_peg_1802:
                        goto loop_peg_1796;
                        success_peg_1798:
                        ;
                        result_peg_1765.addResult(result_peg_1797);
                    } while (true);
                    loop_peg_1796:
                    ;
                            
                }
                goto success_peg_1787;
                goto out_peg_1784;
                success_peg_1787:
                ;
            
            
            
            result_peg_1765 = rule_expr10(stream, result_peg_1765.getPosition());
                if (result_peg_1765.error()){
                    goto out_peg_1784;
                }
                right = result_peg_1765.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    {
                            typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                            value = as<compare_func>(maker)(left, right);
                            }
                    result_peg_1765.setValue(value);
                }
                new_left = result_peg_1765.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1765.getPosition();
        goto tail_peg_1766;
        out_peg_1784:
        Result result_peg_1805(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1805.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1805.getPosition());
        
        return result_peg_1805;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1808 = stream.getColumn(position);
    if (column_peg_1808.chunk8 != 0 && column_peg_1808.chunk8->chunk_compare.calculated()){
        return column_peg_1808.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_1821(stream, "compare");
    int myposition = position;
    
    
    
    Result result_peg_1809(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar("<="[i], stream.get(result_peg_1809.getPosition()))){
                        result_peg_1809.nextPosition();
                    } else {
                        goto out_peg_1811;
                    }
                }
                result_peg_1809.setValue((void*) "<=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThanEquals;
                    result_peg_1809.setValue(value);
                }
            
            
        }
        
        if (column_peg_1808.chunk8 == 0){
            column_peg_1808.chunk8 = new Chunk8();
        }
        column_peg_1808.chunk8->chunk_compare = result_peg_1809;
        stream.update(result_peg_1809.getPosition());
        
        
        return result_peg_1809;
        out_peg_1811:
        Result result_peg_1812(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar(">="[i], stream.get(result_peg_1812.getPosition()))){
                        result_peg_1812.nextPosition();
                    } else {
                        goto out_peg_1814;
                    }
                }
                result_peg_1812.setValue((void*) ">=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThanEquals;
                    result_peg_1812.setValue(value);
                }
            
            
        }
        
        if (column_peg_1808.chunk8 == 0){
            column_peg_1808.chunk8 = new Chunk8();
        }
        column_peg_1808.chunk8->chunk_compare = result_peg_1812;
        stream.update(result_peg_1812.getPosition());
        
        
        return result_peg_1812;
        out_peg_1814:
        Result result_peg_1815(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("<"[i], stream.get(result_peg_1815.getPosition()))){
                        result_peg_1815.nextPosition();
                    } else {
                        goto out_peg_1817;
                    }
                }
                result_peg_1815.setValue((void*) "<");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThan;
                    result_peg_1815.setValue(value);
                }
            
            
        }
        
        if (column_peg_1808.chunk8 == 0){
            column_peg_1808.chunk8 = new Chunk8();
        }
        column_peg_1808.chunk8->chunk_compare = result_peg_1815;
        stream.update(result_peg_1815.getPosition());
        
        
        return result_peg_1815;
        out_peg_1817:
        Result result_peg_1818(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_1818.getPosition()))){
                        result_peg_1818.nextPosition();
                    } else {
                        goto out_peg_1820;
                    }
                }
                result_peg_1818.setValue((void*) ">");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThan;
                    result_peg_1818.setValue(value);
                }
            
            
        }
        
        if (column_peg_1808.chunk8 == 0){
            column_peg_1808.chunk8 = new Chunk8();
        }
        column_peg_1808.chunk8->chunk_compare = result_peg_1818;
        stream.update(result_peg_1818.getPosition());
        
        
        return result_peg_1818;
        out_peg_1820:
    
        if (column_peg_1808.chunk8 == 0){
            column_peg_1808.chunk8 = new Chunk8();
        }
        column_peg_1808.chunk8->chunk_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1822 = stream.getColumn(position);
    if (column_peg_1822.chunk8 != 0 && column_peg_1822.chunk8->chunk_all_compare.calculated()){
        return column_peg_1822.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_1829(stream, "all_compare");
    int myposition = position;
    
    
    
    Result result_peg_1823(myposition);
        
        result_peg_1823 = rule_compare(stream, result_peg_1823.getPosition());
        if (result_peg_1823.error()){
            goto out_peg_1824;
        }
        
        if (column_peg_1822.chunk8 == 0){
            column_peg_1822.chunk8 = new Chunk8();
        }
        column_peg_1822.chunk8->chunk_all_compare = result_peg_1823;
        stream.update(result_peg_1823.getPosition());
        
        
        return result_peg_1823;
        out_peg_1824:
        Result result_peg_1825(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("="[i], stream.get(result_peg_1825.getPosition()))){
                result_peg_1825.nextPosition();
            } else {
                goto out_peg_1826;
            }
        }
        result_peg_1825.setValue((void*) "=");
        
        if (column_peg_1822.chunk8 == 0){
            column_peg_1822.chunk8 = new Chunk8();
        }
        column_peg_1822.chunk8->chunk_all_compare = result_peg_1825;
        stream.update(result_peg_1825.getPosition());
        
        
        return result_peg_1825;
        out_peg_1826:
        Result result_peg_1827(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("!="[i], stream.get(result_peg_1827.getPosition()))){
                result_peg_1827.nextPosition();
            } else {
                goto out_peg_1828;
            }
        }
        result_peg_1827.setValue((void*) "!=");
        
        if (column_peg_1822.chunk8 == 0){
            column_peg_1822.chunk8 = new Chunk8();
        }
        column_peg_1822.chunk8->chunk_all_compare = result_peg_1827;
        stream.update(result_peg_1827.getPosition());
        
        
        return result_peg_1827;
        out_peg_1828:
    
        if (column_peg_1822.chunk8 == 0){
            column_peg_1822.chunk8 = new Chunk8();
        }
        column_peg_1822.chunk8->chunk_all_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1912(stream, "expr10_rest");
    int myposition = position;
    
    tail_peg_1832:
    Value right;
        Value new_left;
    Result result_peg_1831(myposition);
        {
        
            {
                    
                    result_peg_1831.reset();
                    do{
                        Result result_peg_1844(result_peg_1831.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1844.getPosition()))){
                                    result_peg_1844.nextPosition();
                                } else {
                                    goto out_peg_1847;
                                }
                            }
                            result_peg_1844.setValue((void*) " ");
                                
                        }
                        goto success_peg_1845;
                        out_peg_1847:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1844.getPosition()))){
                                    result_peg_1844.nextPosition();
                                } else {
                                    goto out_peg_1849;
                                }
                            }
                            result_peg_1844.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1845;
                        out_peg_1849:
                        goto loop_peg_1843;
                        success_peg_1845:
                        ;
                        result_peg_1831.addResult(result_peg_1844);
                    } while (true);
                    loop_peg_1843:
                    ;
                            
                }
                goto success_peg_1834;
                goto out_peg_1850;
                success_peg_1834:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1831.getPosition()))){
                        result_peg_1831.nextPosition();
                    } else {
                        goto out_peg_1850;
                    }
                }
                result_peg_1831.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_1831.reset();
                    do{
                        Result result_peg_1863(result_peg_1831.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1863.getPosition()))){
                                    result_peg_1863.nextPosition();
                                } else {
                                    goto out_peg_1866;
                                }
                            }
                            result_peg_1863.setValue((void*) " ");
                                
                        }
                        goto success_peg_1864;
                        out_peg_1866:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1863.getPosition()))){
                                    result_peg_1863.nextPosition();
                                } else {
                                    goto out_peg_1868;
                                }
                            }
                            result_peg_1863.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1864;
                        out_peg_1868:
                        goto loop_peg_1862;
                        success_peg_1864:
                        ;
                        result_peg_1831.addResult(result_peg_1863);
                    } while (true);
                    loop_peg_1862:
                    ;
                            
                }
                goto success_peg_1853;
                goto out_peg_1850;
                success_peg_1853:
                ;
            
            
            
            result_peg_1831 = rule_expr11(stream, result_peg_1831.getPosition());
                if (result_peg_1831.error()){
                    goto out_peg_1850;
                }
                right = result_peg_1831.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAdd(left, right);
                    result_peg_1831.setValue(value);
                }
                new_left = result_peg_1831.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1831.getPosition();
        goto tail_peg_1832;
        out_peg_1850:
        Result result_peg_1871(myposition);
        {
        
            {
                    
                    result_peg_1871.reset();
                    do{
                        Result result_peg_1883(result_peg_1871.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1883.getPosition()))){
                                    result_peg_1883.nextPosition();
                                } else {
                                    goto out_peg_1886;
                                }
                            }
                            result_peg_1883.setValue((void*) " ");
                                
                        }
                        goto success_peg_1884;
                        out_peg_1886:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1883.getPosition()))){
                                    result_peg_1883.nextPosition();
                                } else {
                                    goto out_peg_1888;
                                }
                            }
                            result_peg_1883.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1884;
                        out_peg_1888:
                        goto loop_peg_1882;
                        success_peg_1884:
                        ;
                        result_peg_1871.addResult(result_peg_1883);
                    } while (true);
                    loop_peg_1882:
                    ;
                            
                }
                goto success_peg_1873;
                goto out_peg_1889;
                success_peg_1873:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1871.getPosition()))){
                        result_peg_1871.nextPosition();
                    } else {
                        goto out_peg_1889;
                    }
                }
                result_peg_1871.setValue((void*) "-");
            
            
            
            {
                    
                    result_peg_1871.reset();
                    do{
                        Result result_peg_1902(result_peg_1871.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1902.getPosition()))){
                                    result_peg_1902.nextPosition();
                                } else {
                                    goto out_peg_1905;
                                }
                            }
                            result_peg_1902.setValue((void*) " ");
                                
                        }
                        goto success_peg_1903;
                        out_peg_1905:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1902.getPosition()))){
                                    result_peg_1902.nextPosition();
                                } else {
                                    goto out_peg_1907;
                                }
                            }
                            result_peg_1902.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1903;
                        out_peg_1907:
                        goto loop_peg_1901;
                        success_peg_1903:
                        ;
                        result_peg_1871.addResult(result_peg_1902);
                    } while (true);
                    loop_peg_1901:
                    ;
                            
                }
                goto success_peg_1892;
                goto out_peg_1889;
                success_peg_1892:
                ;
            
            
            
            result_peg_1871 = rule_expr11(stream, result_peg_1871.getPosition());
                if (result_peg_1871.error()){
                    goto out_peg_1889;
                }
                right = result_peg_1871.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionSubtract(left, right);
                    result_peg_1871.setValue(value);
                }
                new_left = result_peg_1871.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1871.getPosition();
        goto tail_peg_1832;
        out_peg_1889:
        Result result_peg_1910(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1910.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1910.getPosition());
        
        return result_peg_1910;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2034(stream, "expr11_rest");
    int myposition = position;
    
    tail_peg_1915:
    Value right;
        Value new_left;
    Result result_peg_1914(myposition);
        {
        
            {
                    
                    result_peg_1914.reset();
                    do{
                        Result result_peg_1927(result_peg_1914.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1927.getPosition()))){
                                    result_peg_1927.nextPosition();
                                } else {
                                    goto out_peg_1930;
                                }
                            }
                            result_peg_1927.setValue((void*) " ");
                                
                        }
                        goto success_peg_1928;
                        out_peg_1930:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1927.getPosition()))){
                                    result_peg_1927.nextPosition();
                                } else {
                                    goto out_peg_1932;
                                }
                            }
                            result_peg_1927.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1928;
                        out_peg_1932:
                        goto loop_peg_1926;
                        success_peg_1928:
                        ;
                        result_peg_1914.addResult(result_peg_1927);
                    } while (true);
                    loop_peg_1926:
                    ;
                            
                }
                goto success_peg_1917;
                goto out_peg_1933;
                success_peg_1917:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("*"[i], stream.get(result_peg_1914.getPosition()))){
                        result_peg_1914.nextPosition();
                    } else {
                        goto out_peg_1933;
                    }
                }
                result_peg_1914.setValue((void*) "*");
            
            
            
            {
                    
                    result_peg_1914.reset();
                    do{
                        Result result_peg_1946(result_peg_1914.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1946.getPosition()))){
                                    result_peg_1946.nextPosition();
                                } else {
                                    goto out_peg_1949;
                                }
                            }
                            result_peg_1946.setValue((void*) " ");
                                
                        }
                        goto success_peg_1947;
                        out_peg_1949:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1946.getPosition()))){
                                    result_peg_1946.nextPosition();
                                } else {
                                    goto out_peg_1951;
                                }
                            }
                            result_peg_1946.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1947;
                        out_peg_1951:
                        goto loop_peg_1945;
                        success_peg_1947:
                        ;
                        result_peg_1914.addResult(result_peg_1946);
                    } while (true);
                    loop_peg_1945:
                    ;
                            
                }
                goto success_peg_1936;
                goto out_peg_1933;
                success_peg_1936:
                ;
            
            
            
            result_peg_1914 = rule_expr12(stream, result_peg_1914.getPosition());
                if (result_peg_1914.error()){
                    goto out_peg_1933;
                }
                right = result_peg_1914.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionMultiply(left, right);
                    result_peg_1914.setValue(value);
                }
                new_left = result_peg_1914.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1914.getPosition();
        goto tail_peg_1915;
        out_peg_1933:
        Result result_peg_1954(myposition);
        {
        
            {
                    
                    result_peg_1954.reset();
                    do{
                        Result result_peg_1966(result_peg_1954.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1966.getPosition()))){
                                    result_peg_1966.nextPosition();
                                } else {
                                    goto out_peg_1969;
                                }
                            }
                            result_peg_1966.setValue((void*) " ");
                                
                        }
                        goto success_peg_1967;
                        out_peg_1969:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1966.getPosition()))){
                                    result_peg_1966.nextPosition();
                                } else {
                                    goto out_peg_1971;
                                }
                            }
                            result_peg_1966.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1967;
                        out_peg_1971:
                        goto loop_peg_1965;
                        success_peg_1967:
                        ;
                        result_peg_1954.addResult(result_peg_1966);
                    } while (true);
                    loop_peg_1965:
                    ;
                            
                }
                goto success_peg_1956;
                goto out_peg_1972;
                success_peg_1956:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_1954.getPosition()))){
                        result_peg_1954.nextPosition();
                    } else {
                        goto out_peg_1972;
                    }
                }
                result_peg_1954.setValue((void*) "/");
            
            
            
            {
                    
                    result_peg_1954.reset();
                    do{
                        Result result_peg_1985(result_peg_1954.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1985.getPosition()))){
                                    result_peg_1985.nextPosition();
                                } else {
                                    goto out_peg_1988;
                                }
                            }
                            result_peg_1985.setValue((void*) " ");
                                
                        }
                        goto success_peg_1986;
                        out_peg_1988:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1985.getPosition()))){
                                    result_peg_1985.nextPosition();
                                } else {
                                    goto out_peg_1990;
                                }
                            }
                            result_peg_1985.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1986;
                        out_peg_1990:
                        goto loop_peg_1984;
                        success_peg_1986:
                        ;
                        result_peg_1954.addResult(result_peg_1985);
                    } while (true);
                    loop_peg_1984:
                    ;
                            
                }
                goto success_peg_1975;
                goto out_peg_1972;
                success_peg_1975:
                ;
            
            
            
            result_peg_1954 = rule_expr12(stream, result_peg_1954.getPosition());
                if (result_peg_1954.error()){
                    goto out_peg_1972;
                }
                right = result_peg_1954.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionDivide(left, right);
                    result_peg_1954.setValue(value);
                }
                new_left = result_peg_1954.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1954.getPosition();
        goto tail_peg_1915;
        out_peg_1972:
        Result result_peg_1993(myposition);
        {
        
            {
                    
                    result_peg_1993.reset();
                    do{
                        Result result_peg_2005(result_peg_1993.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2005.getPosition()))){
                                    result_peg_2005.nextPosition();
                                } else {
                                    goto out_peg_2008;
                                }
                            }
                            result_peg_2005.setValue((void*) " ");
                                
                        }
                        goto success_peg_2006;
                        out_peg_2008:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2005.getPosition()))){
                                    result_peg_2005.nextPosition();
                                } else {
                                    goto out_peg_2010;
                                }
                            }
                            result_peg_2005.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2006;
                        out_peg_2010:
                        goto loop_peg_2004;
                        success_peg_2006:
                        ;
                        result_peg_1993.addResult(result_peg_2005);
                    } while (true);
                    loop_peg_2004:
                    ;
                            
                }
                goto success_peg_1995;
                goto out_peg_2011;
                success_peg_1995:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("%"[i], stream.get(result_peg_1993.getPosition()))){
                        result_peg_1993.nextPosition();
                    } else {
                        goto out_peg_2011;
                    }
                }
                result_peg_1993.setValue((void*) "%");
            
            
            
            {
                    
                    result_peg_1993.reset();
                    do{
                        Result result_peg_2024(result_peg_1993.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2024.getPosition()))){
                                    result_peg_2024.nextPosition();
                                } else {
                                    goto out_peg_2027;
                                }
                            }
                            result_peg_2024.setValue((void*) " ");
                                
                        }
                        goto success_peg_2025;
                        out_peg_2027:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2024.getPosition()))){
                                    result_peg_2024.nextPosition();
                                } else {
                                    goto out_peg_2029;
                                }
                            }
                            result_peg_2024.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2025;
                        out_peg_2029:
                        goto loop_peg_2023;
                        success_peg_2025:
                        ;
                        result_peg_1993.addResult(result_peg_2024);
                    } while (true);
                    loop_peg_2023:
                    ;
                            
                }
                goto success_peg_2014;
                goto out_peg_2011;
                success_peg_2014:
                ;
            
            
            
            result_peg_1993 = rule_expr12(stream, result_peg_1993.getPosition());
                if (result_peg_1993.error()){
                    goto out_peg_2011;
                }
                right = result_peg_1993.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionModulo(left, right);
                    result_peg_1993.setValue(value);
                }
                new_left = result_peg_1993.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1993.getPosition();
        goto tail_peg_1915;
        out_peg_2011:
        Result result_peg_2032(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2032.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2032.getPosition());
        
        return result_peg_2032;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2078(stream, "expr12_rest");
    int myposition = position;
    
    tail_peg_2037:
    Value right;
        Value new_left;
    Result result_peg_2036(myposition);
        {
        
            {
                    
                    result_peg_2036.reset();
                    do{
                        Result result_peg_2049(result_peg_2036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2049.getPosition()))){
                                    result_peg_2049.nextPosition();
                                } else {
                                    goto out_peg_2052;
                                }
                            }
                            result_peg_2049.setValue((void*) " ");
                                
                        }
                        goto success_peg_2050;
                        out_peg_2052:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2049.getPosition()))){
                                    result_peg_2049.nextPosition();
                                } else {
                                    goto out_peg_2054;
                                }
                            }
                            result_peg_2049.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2050;
                        out_peg_2054:
                        goto loop_peg_2048;
                        success_peg_2050:
                        ;
                        result_peg_2036.addResult(result_peg_2049);
                    } while (true);
                    loop_peg_2048:
                    ;
                            
                }
                goto success_peg_2039;
                goto out_peg_2055;
                success_peg_2039:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("**"[i], stream.get(result_peg_2036.getPosition()))){
                        result_peg_2036.nextPosition();
                    } else {
                        goto out_peg_2055;
                    }
                }
                result_peg_2036.setValue((void*) "**");
            
            
            
            {
                    
                    result_peg_2036.reset();
                    do{
                        Result result_peg_2068(result_peg_2036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2068.getPosition()))){
                                    result_peg_2068.nextPosition();
                                } else {
                                    goto out_peg_2071;
                                }
                            }
                            result_peg_2068.setValue((void*) " ");
                                
                        }
                        goto success_peg_2069;
                        out_peg_2071:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2068.getPosition()))){
                                    result_peg_2068.nextPosition();
                                } else {
                                    goto out_peg_2073;
                                }
                            }
                            result_peg_2068.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2069;
                        out_peg_2073:
                        goto loop_peg_2067;
                        success_peg_2069:
                        ;
                        result_peg_2036.addResult(result_peg_2068);
                    } while (true);
                    loop_peg_2067:
                    ;
                            
                }
                goto success_peg_2058;
                goto out_peg_2055;
                success_peg_2058:
                ;
            
            
            
            result_peg_2036 = rule_expr13(stream, result_peg_2036.getPosition());
                if (result_peg_2036.error()){
                    goto out_peg_2055;
                }
                right = result_peg_2036.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionPower(left, right);
                    result_peg_2036.setValue(value);
                }
                new_left = result_peg_2036.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2036.getPosition();
        goto tail_peg_2037;
        out_peg_2055:
        Result result_peg_2076(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2076.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2076.getPosition());
        
        return result_peg_2076;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_2079 = stream.getColumn(position);
    if (column_peg_2079.chunk9 != 0 && column_peg_2079.chunk9->chunk_unary.calculated()){
        return column_peg_2079.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_2097(stream, "unary");
    int myposition = position;
    
    
    
    Result result_peg_2080(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("!"[i], stream.get(result_peg_2080.getPosition()))){
                        result_peg_2080.nextPosition();
                    } else {
                        goto out_peg_2082;
                    }
                }
                result_peg_2080.setValue((void*) "!");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Not;
                    result_peg_2080.setValue(value);
                }
            
            
        }
        
        if (column_peg_2079.chunk9 == 0){
            column_peg_2079.chunk9 = new Chunk9();
        }
        column_peg_2079.chunk9->chunk_unary = result_peg_2080;
        stream.update(result_peg_2080.getPosition());
        
        
        return result_peg_2080;
        out_peg_2082:
        Result result_peg_2083(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2083.getPosition()))){
                        result_peg_2083.nextPosition();
                    } else {
                        goto out_peg_2085;
                    }
                }
                result_peg_2083.setValue((void*) "-");
            
            
            
            Result result_peg_2088(result_peg_2083);
                {
                    
                    char letter_peg_2093 = stream.get(result_peg_2088.getPosition());
                    if (letter_peg_2093 != '\0' && strchr("0123456789", letter_peg_2093) != NULL){
                        result_peg_2088.nextPosition();
                        result_peg_2088.setValue((void*) (long) letter_peg_2093);
                    } else {
                        goto out_peg_2092;
                    }
                    
                }
                goto success_peg_2089;
                out_peg_2092:
                goto not_peg_2087;
                success_peg_2089:
                ;
                goto out_peg_2085;
                not_peg_2087:
                result_peg_2083.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Minus;
                    result_peg_2083.setValue(value);
                }
            
            
        }
        
        if (column_peg_2079.chunk9 == 0){
            column_peg_2079.chunk9 = new Chunk9();
        }
        column_peg_2079.chunk9->chunk_unary = result_peg_2083;
        stream.update(result_peg_2083.getPosition());
        
        
        return result_peg_2083;
        out_peg_2085:
        Result result_peg_2094(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2094.getPosition()))){
                        result_peg_2094.nextPosition();
                    } else {
                        goto out_peg_2096;
                    }
                }
                result_peg_2094.setValue((void*) "~");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Negation;
                    result_peg_2094.setValue(value);
                }
            
            
        }
        
        if (column_peg_2079.chunk9 == 0){
            column_peg_2079.chunk9 = new Chunk9();
        }
        column_peg_2079.chunk9->chunk_unary = result_peg_2094;
        stream.update(result_peg_2094.getPosition());
        
        
        return result_peg_2094;
        out_peg_2096:
    
        if (column_peg_2079.chunk9 == 0){
            column_peg_2079.chunk9 = new Chunk9();
        }
        column_peg_2079.chunk9->chunk_unary = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_2098 = stream.getColumn(position);
    if (column_peg_2098.chunk9 != 0 && column_peg_2098.chunk9->chunk_expr13_real.calculated()){
        return column_peg_2098.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_2144(stream, "expr13_real");
    int myposition = position;
    
    
    Value e;
    Result result_peg_2099(myposition);
        
        result_peg_2099 = rule_helper(stream, result_peg_2099.getPosition());
        if (result_peg_2099.error()){
            goto out_peg_2100;
        }
        
        if (column_peg_2098.chunk9 == 0){
            column_peg_2098.chunk9 = new Chunk9();
        }
        column_peg_2098.chunk9->chunk_expr13_real = result_peg_2099;
        stream.update(result_peg_2099.getPosition());
        
        
        return result_peg_2099;
        out_peg_2100:
        Result result_peg_2101(myposition);
        
        result_peg_2101 = rule_function(stream, result_peg_2101.getPosition());
        if (result_peg_2101.error()){
            goto out_peg_2102;
        }
        
        if (column_peg_2098.chunk9 == 0){
            column_peg_2098.chunk9 = new Chunk9();
        }
        column_peg_2098.chunk9->chunk_expr13_real = result_peg_2101;
        stream.update(result_peg_2101.getPosition());
        
        
        return result_peg_2101;
        out_peg_2102:
        Result result_peg_2103(myposition);
        
        result_peg_2103 = rule_value(stream, result_peg_2103.getPosition());
        if (result_peg_2103.error()){
            goto out_peg_2104;
        }
        
        if (column_peg_2098.chunk9 == 0){
            column_peg_2098.chunk9 = new Chunk9();
        }
        column_peg_2098.chunk9->chunk_expr13_real = result_peg_2103;
        stream.update(result_peg_2103.getPosition());
        
        
        return result_peg_2103;
        out_peg_2104:
        Result result_peg_2105(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2105.getPosition()))){
                        result_peg_2105.nextPosition();
                    } else {
                        goto out_peg_2107;
                    }
                }
                result_peg_2105.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2105.reset();
                    do{
                        Result result_peg_2119(result_peg_2105.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2119.getPosition()))){
                                    result_peg_2119.nextPosition();
                                } else {
                                    goto out_peg_2122;
                                }
                            }
                            result_peg_2119.setValue((void*) " ");
                                
                        }
                        goto success_peg_2120;
                        out_peg_2122:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2119.getPosition()))){
                                    result_peg_2119.nextPosition();
                                } else {
                                    goto out_peg_2124;
                                }
                            }
                            result_peg_2119.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2120;
                        out_peg_2124:
                        goto loop_peg_2118;
                        success_peg_2120:
                        ;
                        result_peg_2105.addResult(result_peg_2119);
                    } while (true);
                    loop_peg_2118:
                    ;
                            
                }
                goto success_peg_2109;
                goto out_peg_2107;
                success_peg_2109:
                ;
            
            
            
            result_peg_2105 = rule_expr(stream, result_peg_2105.getPosition());
                if (result_peg_2105.error()){
                    goto out_peg_2107;
                }
                e = result_peg_2105.getValues();
            
            
            
            {
                    
                    result_peg_2105.reset();
                    do{
                        Result result_peg_2137(result_peg_2105.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2137.getPosition()))){
                                    result_peg_2137.nextPosition();
                                } else {
                                    goto out_peg_2140;
                                }
                            }
                            result_peg_2137.setValue((void*) " ");
                                
                        }
                        goto success_peg_2138;
                        out_peg_2140:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2137.getPosition()))){
                                    result_peg_2137.nextPosition();
                                } else {
                                    goto out_peg_2142;
                                }
                            }
                            result_peg_2137.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2138;
                        out_peg_2142:
                        goto loop_peg_2136;
                        success_peg_2138:
                        ;
                        result_peg_2105.addResult(result_peg_2137);
                    } while (true);
                    loop_peg_2136:
                    ;
                            
                }
                goto success_peg_2127;
                goto out_peg_2107;
                success_peg_2127:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2105.getPosition()))){
                        result_peg_2105.nextPosition();
                    } else {
                        goto out_peg_2107;
                    }
                }
                result_peg_2105.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = e;
                    result_peg_2105.setValue(value);
                }
            
            
        }
        
        if (column_peg_2098.chunk9 == 0){
            column_peg_2098.chunk9 = new Chunk9();
        }
        column_peg_2098.chunk9->chunk_expr13_real = result_peg_2105;
        stream.update(result_peg_2105.getPosition());
        
        
        return result_peg_2105;
        out_peg_2107:
    
        if (column_peg_2098.chunk9 == 0){
            column_peg_2098.chunk9 = new Chunk9();
        }
        column_peg_2098.chunk9->chunk_expr13_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_2145 = stream.getColumn(position);
    if (column_peg_2145.chunk9 != 0 && column_peg_2145.chunk9->chunk_function.calculated()){
        return column_peg_2145.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2361(stream, "function");
    int myposition = position;
    
    
    Value name;
        Value args;
        Value arg1;
    Result result_peg_2146(myposition);
        
        {
        
            result_peg_2146 = rule_function_name(stream, result_peg_2146.getPosition());
                if (result_peg_2146.error()){
                    goto out_peg_2148;
                }
                name = result_peg_2146.getValues();
            
            
            
            {
                    
                    result_peg_2146.reset();
                    do{
                        Result result_peg_2160(result_peg_2146.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2160.getPosition()))){
                                    result_peg_2160.nextPosition();
                                } else {
                                    goto out_peg_2163;
                                }
                            }
                            result_peg_2160.setValue((void*) " ");
                                
                        }
                        goto success_peg_2161;
                        out_peg_2163:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2160.getPosition()))){
                                    result_peg_2160.nextPosition();
                                } else {
                                    goto out_peg_2165;
                                }
                            }
                            result_peg_2160.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2161;
                        out_peg_2165:
                        goto loop_peg_2159;
                        success_peg_2161:
                        ;
                        result_peg_2146.addResult(result_peg_2160);
                    } while (true);
                    loop_peg_2159:
                    ;
                            
                }
                goto success_peg_2150;
                goto out_peg_2148;
                success_peg_2150:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2146.getPosition()))){
                        result_peg_2146.nextPosition();
                    } else {
                        goto out_peg_2148;
                    }
                }
                result_peg_2146.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2146.reset();
                    do{
                        Result result_peg_2178(result_peg_2146.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2178.getPosition()))){
                                    result_peg_2178.nextPosition();
                                } else {
                                    goto out_peg_2181;
                                }
                            }
                            result_peg_2178.setValue((void*) " ");
                                
                        }
                        goto success_peg_2179;
                        out_peg_2181:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2178.getPosition()))){
                                    result_peg_2178.nextPosition();
                                } else {
                                    goto out_peg_2183;
                                }
                            }
                            result_peg_2178.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2179;
                        out_peg_2183:
                        goto loop_peg_2177;
                        success_peg_2179:
                        ;
                        result_peg_2146.addResult(result_peg_2178);
                    } while (true);
                    loop_peg_2177:
                    ;
                            
                }
                goto success_peg_2168;
                goto out_peg_2148;
                success_peg_2168:
                ;
            
            
            
            result_peg_2146 = rule_args(stream, result_peg_2146.getPosition());
                if (result_peg_2146.error()){
                    goto out_peg_2148;
                }
                args = result_peg_2146.getValues();
            
            
            
            {
                    
                    result_peg_2146.reset();
                    do{
                        Result result_peg_2196(result_peg_2146.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2196.getPosition()))){
                                    result_peg_2196.nextPosition();
                                } else {
                                    goto out_peg_2199;
                                }
                            }
                            result_peg_2196.setValue((void*) " ");
                                
                        }
                        goto success_peg_2197;
                        out_peg_2199:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2196.getPosition()))){
                                    result_peg_2196.nextPosition();
                                } else {
                                    goto out_peg_2201;
                                }
                            }
                            result_peg_2196.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2197;
                        out_peg_2201:
                        goto loop_peg_2195;
                        success_peg_2197:
                        ;
                        result_peg_2146.addResult(result_peg_2196);
                    } while (true);
                    loop_peg_2195:
                    ;
                            
                }
                goto success_peg_2186;
                goto out_peg_2148;
                success_peg_2186:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2146.getPosition()))){
                        result_peg_2146.nextPosition();
                    } else {
                        goto out_peg_2148;
                    }
                }
                result_peg_2146.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction(name, args);
                    result_peg_2146.setValue(value);
                }
            
            
        }
        
        if (column_peg_2145.chunk9 == 0){
            column_peg_2145.chunk9 = new Chunk9();
        }
        column_peg_2145.chunk9->chunk_function = result_peg_2146;
        stream.update(result_peg_2146.getPosition());
        
        
        return result_peg_2146;
        out_peg_2148:
        Result result_peg_2203(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2203.getPosition()))){
                        result_peg_2203.nextPosition();
                    } else {
                        goto out_peg_2205;
                    }
                }
                result_peg_2203.setValue((void*) "teammode");
                name = result_peg_2203.getValues();
            
            
            
            {
                    
                    result_peg_2203.reset();
                    do{
                        Result result_peg_2217(result_peg_2203.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2217.getPosition()))){
                                    result_peg_2217.nextPosition();
                                } else {
                                    goto out_peg_2220;
                                }
                            }
                            result_peg_2217.setValue((void*) " ");
                                
                        }
                        goto success_peg_2218;
                        out_peg_2220:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2217.getPosition()))){
                                    result_peg_2217.nextPosition();
                                } else {
                                    goto out_peg_2222;
                                }
                            }
                            result_peg_2217.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2218;
                        out_peg_2222:
                        goto loop_peg_2216;
                        success_peg_2218:
                        ;
                        result_peg_2203.addResult(result_peg_2217);
                    } while (true);
                    loop_peg_2216:
                    ;
                            
                }
                goto success_peg_2207;
                goto out_peg_2205;
                success_peg_2207:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2203.getPosition()))){
                        result_peg_2203.nextPosition();
                    } else {
                        goto out_peg_2205;
                    }
                }
                result_peg_2203.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2203.reset();
                    do{
                        Result result_peg_2235(result_peg_2203.getPosition());
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
                        result_peg_2203.addResult(result_peg_2235);
                    } while (true);
                    loop_peg_2234:
                    ;
                            
                }
                goto success_peg_2225;
                goto out_peg_2205;
                success_peg_2225:
                ;
            
            
            
            result_peg_2203 = rule_identifier(stream, result_peg_2203.getPosition());
                if (result_peg_2203.error()){
                    goto out_peg_2205;
                }
                arg1 = result_peg_2203.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(name, arg1);
                    result_peg_2203.setValue(value);
                }
            
            
        }
        
        if (column_peg_2145.chunk9 == 0){
            column_peg_2145.chunk9 = new Chunk9();
        }
        column_peg_2145.chunk9->chunk_function = result_peg_2203;
        stream.update(result_peg_2203.getPosition());
        
        
        return result_peg_2203;
        out_peg_2205:
        Result result_peg_2242(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2242.getPosition()))){
                        result_peg_2242.nextPosition();
                    } else {
                        goto out_peg_2244;
                    }
                }
                result_peg_2242.setValue((void*) "teammode");
                name = result_peg_2242.getValues();
            
            
            
            {
                    
                    result_peg_2242.reset();
                    do{
                        Result result_peg_2256(result_peg_2242.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2256.getPosition()))){
                                    result_peg_2256.nextPosition();
                                } else {
                                    goto out_peg_2259;
                                }
                            }
                            result_peg_2256.setValue((void*) " ");
                                
                        }
                        goto success_peg_2257;
                        out_peg_2259:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2256.getPosition()))){
                                    result_peg_2256.nextPosition();
                                } else {
                                    goto out_peg_2261;
                                }
                            }
                            result_peg_2256.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2257;
                        out_peg_2261:
                        goto loop_peg_2255;
                        success_peg_2257:
                        ;
                        result_peg_2242.addResult(result_peg_2256);
                    } while (true);
                    loop_peg_2255:
                    ;
                            
                }
                goto success_peg_2246;
                goto out_peg_2244;
                success_peg_2246:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_2242.getPosition()))){
                        result_peg_2242.nextPosition();
                    } else {
                        goto out_peg_2244;
                    }
                }
                result_peg_2242.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_2242.reset();
                    do{
                        Result result_peg_2274(result_peg_2242.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2274.getPosition()))){
                                    result_peg_2274.nextPosition();
                                } else {
                                    goto out_peg_2277;
                                }
                            }
                            result_peg_2274.setValue((void*) " ");
                                
                        }
                        goto success_peg_2275;
                        out_peg_2277:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2274.getPosition()))){
                                    result_peg_2274.nextPosition();
                                } else {
                                    goto out_peg_2279;
                                }
                            }
                            result_peg_2274.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2275;
                        out_peg_2279:
                        goto loop_peg_2273;
                        success_peg_2275:
                        ;
                        result_peg_2242.addResult(result_peg_2274);
                    } while (true);
                    loop_peg_2273:
                    ;
                            
                }
                goto success_peg_2264;
                goto out_peg_2244;
                success_peg_2264:
                ;
            
            
            
            result_peg_2242 = rule_identifier(stream, result_peg_2242.getPosition());
                if (result_peg_2242.error()){
                    goto out_peg_2244;
                }
                arg1 = result_peg_2242.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1("teammode!=", arg1);
                    result_peg_2242.setValue(value);
                }
            
            
        }
        
        if (column_peg_2145.chunk9 == 0){
            column_peg_2145.chunk9 = new Chunk9();
        }
        column_peg_2145.chunk9->chunk_function = result_peg_2242;
        stream.update(result_peg_2242.getPosition());
        
        
        return result_peg_2242;
        out_peg_2244:
        Result result_peg_2281(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2281.getPosition()))){
                        result_peg_2281.nextPosition();
                    } else {
                        goto out_peg_2283;
                    }
                }
                result_peg_2281.setValue((void*) "animelem");
                name = result_peg_2281.getValues();
            
            
            
            {
                    
                    result_peg_2281.reset();
                    do{
                        Result result_peg_2295(result_peg_2281.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2295.getPosition()))){
                                    result_peg_2295.nextPosition();
                                } else {
                                    goto out_peg_2298;
                                }
                            }
                            result_peg_2295.setValue((void*) " ");
                                
                        }
                        goto success_peg_2296;
                        out_peg_2298:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2295.getPosition()))){
                                    result_peg_2295.nextPosition();
                                } else {
                                    goto out_peg_2300;
                                }
                            }
                            result_peg_2295.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2296;
                        out_peg_2300:
                        goto loop_peg_2294;
                        success_peg_2296:
                        ;
                        result_peg_2281.addResult(result_peg_2295);
                    } while (true);
                    loop_peg_2294:
                    ;
                            
                }
                goto success_peg_2285;
                goto out_peg_2283;
                success_peg_2285:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2281.getPosition()))){
                        result_peg_2281.nextPosition();
                    } else {
                        goto out_peg_2283;
                    }
                }
                result_peg_2281.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2281.reset();
                    do{
                        Result result_peg_2313(result_peg_2281.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2313.getPosition()))){
                                    result_peg_2313.nextPosition();
                                } else {
                                    goto out_peg_2316;
                                }
                            }
                            result_peg_2313.setValue((void*) " ");
                                
                        }
                        goto success_peg_2314;
                        out_peg_2316:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2313.getPosition()))){
                                    result_peg_2313.nextPosition();
                                } else {
                                    goto out_peg_2318;
                                }
                            }
                            result_peg_2313.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2314;
                        out_peg_2318:
                        goto loop_peg_2312;
                        success_peg_2314:
                        ;
                        result_peg_2281.addResult(result_peg_2313);
                    } while (true);
                    loop_peg_2312:
                    ;
                            
                }
                goto success_peg_2303;
                goto out_peg_2283;
                success_peg_2303:
                ;
            
            
            
            result_peg_2281 = rule_integer(stream, result_peg_2281.getPosition());
                if (result_peg_2281.error()){
                    goto out_peg_2283;
                }
                arg1 = result_peg_2281.getValues();
            
            
            
            result_peg_2281 = rule_function_rest(stream, result_peg_2281.getPosition());
                if (result_peg_2281.error()){
                    goto out_peg_2283;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    value = makeFunction1(name, arg1);
                    result_peg_2281.setValue(value);
                }
            
            
        }
        
        if (column_peg_2145.chunk9 == 0){
            column_peg_2145.chunk9 = new Chunk9();
        }
        column_peg_2145.chunk9->chunk_function = result_peg_2281;
        stream.update(result_peg_2281.getPosition());
        
        
        return result_peg_2281;
        out_peg_2283:
        Result result_peg_2321(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2321.getPosition()))){
                        result_peg_2321.nextPosition();
                    } else {
                        goto out_peg_2323;
                    }
                }
                result_peg_2321.setValue((void*) "animelem");
                name = result_peg_2321.getValues();
            
            
            
            {
                    
                    result_peg_2321.reset();
                    do{
                        Result result_peg_2335(result_peg_2321.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2335.getPosition()))){
                                    result_peg_2335.nextPosition();
                                } else {
                                    goto out_peg_2338;
                                }
                            }
                            result_peg_2335.setValue((void*) " ");
                                
                        }
                        goto success_peg_2336;
                        out_peg_2338:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2335.getPosition()))){
                                    result_peg_2335.nextPosition();
                                } else {
                                    goto out_peg_2340;
                                }
                            }
                            result_peg_2335.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2336;
                        out_peg_2340:
                        goto loop_peg_2334;
                        success_peg_2336:
                        ;
                        result_peg_2321.addResult(result_peg_2335);
                    } while (true);
                    loop_peg_2334:
                    ;
                            
                }
                goto success_peg_2325;
                goto out_peg_2323;
                success_peg_2325:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("<="[i], stream.get(result_peg_2321.getPosition()))){
                        result_peg_2321.nextPosition();
                    } else {
                        goto out_peg_2323;
                    }
                }
                result_peg_2321.setValue((void*) "<=");
            
            
            
            {
                    
                    result_peg_2321.reset();
                    do{
                        Result result_peg_2353(result_peg_2321.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2353.getPosition()))){
                                    result_peg_2353.nextPosition();
                                } else {
                                    goto out_peg_2356;
                                }
                            }
                            result_peg_2353.setValue((void*) " ");
                                
                        }
                        goto success_peg_2354;
                        out_peg_2356:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2353.getPosition()))){
                                    result_peg_2353.nextPosition();
                                } else {
                                    goto out_peg_2358;
                                }
                            }
                            result_peg_2353.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2354;
                        out_peg_2358:
                        goto loop_peg_2352;
                        success_peg_2354:
                        ;
                        result_peg_2321.addResult(result_peg_2353);
                    } while (true);
                    loop_peg_2352:
                    ;
                            
                }
                goto success_peg_2343;
                goto out_peg_2323;
                success_peg_2343:
                ;
            
            
            
            result_peg_2321 = rule_integer(stream, result_peg_2321.getPosition());
                if (result_peg_2321.error()){
                    goto out_peg_2323;
                }
                arg1 = result_peg_2321.getValues();
            
            
            
            result_peg_2321 = rule_function_rest(stream, result_peg_2321.getPosition());
                if (result_peg_2321.error()){
                    goto out_peg_2323;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    value = makeFunction1("animelem<=", arg1);
                    result_peg_2321.setValue(value);
                }
            
            
        }
        
        if (column_peg_2145.chunk9 == 0){
            column_peg_2145.chunk9 = new Chunk9();
        }
        column_peg_2145.chunk9->chunk_function = result_peg_2321;
        stream.update(result_peg_2321.getPosition());
        
        
        return result_peg_2321;
        out_peg_2323:
    
        if (column_peg_2145.chunk9 == 0){
            column_peg_2145.chunk9 = new Chunk9();
        }
        column_peg_2145.chunk9->chunk_function = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2362 = stream.getColumn(position);
    if (column_peg_2362.chunk9 != 0 && column_peg_2362.chunk9->chunk_args.calculated()){
        return column_peg_2362.chunk9->chunk_args;
    }
    
    RuleTrace trace_peg_2404(stream, "args");
    int myposition = position;
    
    
    Value expr1;
        Value expr_rest;
    Result result_peg_2363(myposition);
        
        {
        
            result_peg_2363 = rule_expr_c(stream, result_peg_2363.getPosition());
                if (result_peg_2363.error()){
                    goto out_peg_2365;
                }
                expr1 = result_peg_2363.getValues();
            
            
            
            result_peg_2363.reset();
                do{
                    Result result_peg_2368(result_peg_2363.getPosition());
                    {
                    
                        {
                                
                                result_peg_2368.reset();
                                do{
                                    Result result_peg_2380(result_peg_2368.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2380.getPosition()))){
                                                result_peg_2380.nextPosition();
                                            } else {
                                                goto out_peg_2383;
                                            }
                                        }
                                        result_peg_2380.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2381;
                                    out_peg_2383:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2380.getPosition()))){
                                                result_peg_2380.nextPosition();
                                            } else {
                                                goto out_peg_2385;
                                            }
                                        }
                                        result_peg_2380.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2381;
                                    out_peg_2385:
                                    goto loop_peg_2379;
                                    success_peg_2381:
                                    ;
                                    result_peg_2368.addResult(result_peg_2380);
                                } while (true);
                                loop_peg_2379:
                                ;
                                        
                            }
                            goto success_peg_2370;
                            goto loop_peg_2367;
                            success_peg_2370:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2368.getPosition()))){
                                    result_peg_2368.nextPosition();
                                } else {
                                    goto loop_peg_2367;
                                }
                            }
                            result_peg_2368.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2368.reset();
                                do{
                                    Result result_peg_2398(result_peg_2368.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2398.getPosition()))){
                                                result_peg_2398.nextPosition();
                                            } else {
                                                goto out_peg_2401;
                                            }
                                        }
                                        result_peg_2398.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2399;
                                    out_peg_2401:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2398.getPosition()))){
                                                result_peg_2398.nextPosition();
                                            } else {
                                                goto out_peg_2403;
                                            }
                                        }
                                        result_peg_2398.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2399;
                                    out_peg_2403:
                                    goto loop_peg_2397;
                                    success_peg_2399:
                                    ;
                                    result_peg_2368.addResult(result_peg_2398);
                                } while (true);
                                loop_peg_2397:
                                ;
                                        
                            }
                            goto success_peg_2388;
                            goto loop_peg_2367;
                            success_peg_2388:
                            ;
                        
                        
                        
                        result_peg_2368 = rule_expr_c(stream, result_peg_2368.getPosition());
                            if (result_peg_2368.error()){
                                goto loop_peg_2367;
                            }
                        
                        
                    }
                    result_peg_2363.addResult(result_peg_2368);
                } while (true);
                loop_peg_2367:
                ;
                expr_rest = result_peg_2363.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(expr1, expr_rest);
                    result_peg_2363.setValue(value);
                }
            
            
        }
        
        if (column_peg_2362.chunk9 == 0){
            column_peg_2362.chunk9 = new Chunk9();
        }
        column_peg_2362.chunk9->chunk_args = result_peg_2363;
        stream.update(result_peg_2363.getPosition());
        
        
        return result_peg_2363;
        out_peg_2365:
    
        if (column_peg_2362.chunk9 == 0){
            column_peg_2362.chunk9 = new Chunk9();
        }
        column_peg_2362.chunk9->chunk_args = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2405 = stream.getColumn(position);
    if (column_peg_2405.chunk10 != 0 && column_peg_2405.chunk10->chunk_function_name.calculated()){
        return column_peg_2405.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_2470(stream, "function_name");
    int myposition = position;
    
    
    
    Result result_peg_2406(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("abs"[i], stream.get(result_peg_2406.getPosition()))){
                result_peg_2406.nextPosition();
            } else {
                goto out_peg_2407;
            }
        }
        result_peg_2406.setValue((void*) "abs");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2406;
        stream.update(result_peg_2406.getPosition());
        
        
        return result_peg_2406;
        out_peg_2407:
        Result result_peg_2408(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("const"[i], stream.get(result_peg_2408.getPosition()))){
                result_peg_2408.nextPosition();
            } else {
                goto out_peg_2409;
            }
        }
        result_peg_2408.setValue((void*) "const");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2408;
        stream.update(result_peg_2408.getPosition());
        
        
        return result_peg_2408;
        out_peg_2409:
        Result result_peg_2410(myposition);
        
        for (int i = 0; i < 13; i++){
            if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2410.getPosition()))){
                result_peg_2410.nextPosition();
            } else {
                goto out_peg_2411;
            }
        }
        result_peg_2410.setValue((void*) "selfanimexist");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2410;
        stream.update(result_peg_2410.getPosition());
        
        
        return result_peg_2410;
        out_peg_2411:
        Result result_peg_2412(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("ifelse"[i], stream.get(result_peg_2412.getPosition()))){
                result_peg_2412.nextPosition();
            } else {
                goto out_peg_2413;
            }
        }
        result_peg_2412.setValue((void*) "ifelse");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2412;
        stream.update(result_peg_2412.getPosition());
        
        
        return result_peg_2412;
        out_peg_2413:
        Result result_peg_2414(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("gethitvar"[i], stream.get(result_peg_2414.getPosition()))){
                result_peg_2414.nextPosition();
            } else {
                goto out_peg_2415;
            }
        }
        result_peg_2414.setValue((void*) "gethitvar");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2414;
        stream.update(result_peg_2414.getPosition());
        
        
        return result_peg_2414;
        out_peg_2415:
        Result result_peg_2416(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("floor"[i], stream.get(result_peg_2416.getPosition()))){
                result_peg_2416.nextPosition();
            } else {
                goto out_peg_2417;
            }
        }
        result_peg_2416.setValue((void*) "floor");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2416;
        stream.update(result_peg_2416.getPosition());
        
        
        return result_peg_2416;
        out_peg_2417:
        Result result_peg_2418(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("ceil"[i], stream.get(result_peg_2418.getPosition()))){
                result_peg_2418.nextPosition();
            } else {
                goto out_peg_2419;
            }
        }
        result_peg_2418.setValue((void*) "ceil");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2418;
        stream.update(result_peg_2418.getPosition());
        
        
        return result_peg_2418;
        out_peg_2419:
        Result result_peg_2420(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("acos"[i], stream.get(result_peg_2420.getPosition()))){
                result_peg_2420.nextPosition();
            } else {
                goto out_peg_2421;
            }
        }
        result_peg_2420.setValue((void*) "acos");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2420;
        stream.update(result_peg_2420.getPosition());
        
        
        return result_peg_2420;
        out_peg_2421:
        Result result_peg_2422(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("asin"[i], stream.get(result_peg_2422.getPosition()))){
                result_peg_2422.nextPosition();
            } else {
                goto out_peg_2423;
            }
        }
        result_peg_2422.setValue((void*) "asin");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2422;
        stream.update(result_peg_2422.getPosition());
        
        
        return result_peg_2422;
        out_peg_2423:
        Result result_peg_2424(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("atan"[i], stream.get(result_peg_2424.getPosition()))){
                result_peg_2424.nextPosition();
            } else {
                goto out_peg_2425;
            }
        }
        result_peg_2424.setValue((void*) "atan");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2424;
        stream.update(result_peg_2424.getPosition());
        
        
        return result_peg_2424;
        out_peg_2425:
        Result result_peg_2426(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("tan"[i], stream.get(result_peg_2426.getPosition()))){
                result_peg_2426.nextPosition();
            } else {
                goto out_peg_2427;
            }
        }
        result_peg_2426.setValue((void*) "tan");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2426;
        stream.update(result_peg_2426.getPosition());
        
        
        return result_peg_2426;
        out_peg_2427:
        Result result_peg_2428(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("cos"[i], stream.get(result_peg_2428.getPosition()))){
                result_peg_2428.nextPosition();
            } else {
                goto out_peg_2429;
            }
        }
        result_peg_2428.setValue((void*) "cos");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2428;
        stream.update(result_peg_2428.getPosition());
        
        
        return result_peg_2428;
        out_peg_2429:
        Result result_peg_2430(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("sin"[i], stream.get(result_peg_2430.getPosition()))){
                result_peg_2430.nextPosition();
            } else {
                goto out_peg_2431;
            }
        }
        result_peg_2430.setValue((void*) "sin");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2430;
        stream.update(result_peg_2430.getPosition());
        
        
        return result_peg_2430;
        out_peg_2431:
        Result result_peg_2432(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("log"[i], stream.get(result_peg_2432.getPosition()))){
                result_peg_2432.nextPosition();
            } else {
                goto out_peg_2433;
            }
        }
        result_peg_2432.setValue((void*) "log");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2432;
        stream.update(result_peg_2432.getPosition());
        
        
        return result_peg_2432;
        out_peg_2433:
        Result result_peg_2434(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareCharCase("ln"[i], stream.get(result_peg_2434.getPosition()))){
                result_peg_2434.nextPosition();
            } else {
                goto out_peg_2435;
            }
        }
        result_peg_2434.setValue((void*) "ln");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2434;
        stream.update(result_peg_2434.getPosition());
        
        
        return result_peg_2434;
        out_peg_2435:
        Result result_peg_2436(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("sysfvar"[i], stream.get(result_peg_2436.getPosition()))){
                result_peg_2436.nextPosition();
            } else {
                goto out_peg_2437;
            }
        }
        result_peg_2436.setValue((void*) "sysfvar");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2436;
        stream.update(result_peg_2436.getPosition());
        
        
        return result_peg_2436;
        out_peg_2437:
        Result result_peg_2438(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("sysvar"[i], stream.get(result_peg_2438.getPosition()))){
                result_peg_2438.nextPosition();
            } else {
                goto out_peg_2439;
            }
        }
        result_peg_2438.setValue((void*) "sysvar");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2438;
        stream.update(result_peg_2438.getPosition());
        
        
        return result_peg_2438;
        out_peg_2439:
        Result result_peg_2440(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("var"[i], stream.get(result_peg_2440.getPosition()))){
                result_peg_2440.nextPosition();
            } else {
                goto out_peg_2441;
            }
        }
        result_peg_2440.setValue((void*) "var");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2440;
        stream.update(result_peg_2440.getPosition());
        
        
        return result_peg_2440;
        out_peg_2441:
        Result result_peg_2442(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numexplod"[i], stream.get(result_peg_2442.getPosition()))){
                result_peg_2442.nextPosition();
            } else {
                goto out_peg_2443;
            }
        }
        result_peg_2442.setValue((void*) "numexplod");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2442;
        stream.update(result_peg_2442.getPosition());
        
        
        return result_peg_2442;
        out_peg_2443:
        Result result_peg_2444(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2444.getPosition()))){
                result_peg_2444.nextPosition();
            } else {
                goto out_peg_2445;
            }
        }
        result_peg_2444.setValue((void*) "numhelper");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2444;
        stream.update(result_peg_2444.getPosition());
        
        
        return result_peg_2444;
        out_peg_2445:
        Result result_peg_2446(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numprojid"[i], stream.get(result_peg_2446.getPosition()))){
                result_peg_2446.nextPosition();
            } else {
                goto out_peg_2447;
            }
        }
        result_peg_2446.setValue((void*) "numprojid");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2446;
        stream.update(result_peg_2446.getPosition());
        
        
        return result_peg_2446;
        out_peg_2447:
        Result result_peg_2448(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("fvar"[i], stream.get(result_peg_2448.getPosition()))){
                result_peg_2448.nextPosition();
            } else {
                goto out_peg_2449;
            }
        }
        result_peg_2448.setValue((void*) "fvar");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2448;
        stream.update(result_peg_2448.getPosition());
        
        
        return result_peg_2448;
        out_peg_2449:
        Result result_peg_2450(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("ishelper"[i], stream.get(result_peg_2450.getPosition()))){
                result_peg_2450.nextPosition();
            } else {
                goto out_peg_2451;
            }
        }
        result_peg_2450.setValue((void*) "ishelper");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2450;
        stream.update(result_peg_2450.getPosition());
        
        
        return result_peg_2450;
        out_peg_2451:
        Result result_peg_2452(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numtarget"[i], stream.get(result_peg_2452.getPosition()))){
                result_peg_2452.nextPosition();
            } else {
                goto out_peg_2453;
            }
        }
        result_peg_2452.setValue((void*) "numtarget");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2452;
        stream.update(result_peg_2452.getPosition());
        
        
        return result_peg_2452;
        out_peg_2453:
        Result result_peg_2454(myposition);
        
        for (int i = 0; i < 12; i++){
            if (compareCharCase("animelemtime"[i], stream.get(result_peg_2454.getPosition()))){
                result_peg_2454.nextPosition();
            } else {
                goto out_peg_2455;
            }
        }
        result_peg_2454.setValue((void*) "animelemtime");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2454;
        stream.update(result_peg_2454.getPosition());
        
        
        return result_peg_2454;
        out_peg_2455:
        Result result_peg_2456(myposition);
        
        for (int i = 0; i < 10; i++){
            if (compareCharCase("animelemno"[i], stream.get(result_peg_2456.getPosition()))){
                result_peg_2456.nextPosition();
            } else {
                goto out_peg_2457;
            }
        }
        result_peg_2456.setValue((void*) "animelemno");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2456;
        stream.update(result_peg_2456.getPosition());
        
        
        return result_peg_2456;
        out_peg_2457:
        Result result_peg_2458(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("animexist"[i], stream.get(result_peg_2458.getPosition()))){
                result_peg_2458.nextPosition();
            } else {
                goto out_peg_2459;
            }
        }
        result_peg_2458.setValue((void*) "animexist");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2458;
        stream.update(result_peg_2458.getPosition());
        
        
        return result_peg_2458;
        out_peg_2459:
        Result result_peg_2460(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projguarded"[i], stream.get(result_peg_2460.getPosition()))){
                result_peg_2460.nextPosition();
            } else {
                goto out_peg_2461;
            }
        }
        result_peg_2460.setValue((void*) "projguarded");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2460;
        stream.update(result_peg_2460.getPosition());
        
        
        return result_peg_2460;
        out_peg_2461:
        Result result_peg_2462(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projhittime"[i], stream.get(result_peg_2462.getPosition()))){
                result_peg_2462.nextPosition();
            } else {
                goto out_peg_2463;
            }
        }
        result_peg_2462.setValue((void*) "projhittime");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2462;
        stream.update(result_peg_2462.getPosition());
        
        
        return result_peg_2462;
        out_peg_2463:
        Result result_peg_2464(myposition);
        
        for (int i = 0; i < 15; i++){
            if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2464.getPosition()))){
                result_peg_2464.nextPosition();
            } else {
                goto out_peg_2465;
            }
        }
        result_peg_2464.setValue((void*) "projcontacttime");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2464;
        stream.update(result_peg_2464.getPosition());
        
        
        return result_peg_2464;
        out_peg_2465:
        Result result_peg_2466(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projcontact"[i], stream.get(result_peg_2466.getPosition()))){
                result_peg_2466.nextPosition();
            } else {
                goto out_peg_2467;
            }
        }
        result_peg_2466.setValue((void*) "projcontact");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2466;
        stream.update(result_peg_2466.getPosition());
        
        
        return result_peg_2466;
        out_peg_2467:
        Result result_peg_2468(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2468.getPosition()))){
                result_peg_2468.nextPosition();
            } else {
                goto out_peg_2469;
            }
        }
        result_peg_2468.setValue((void*) "numhelper");
        
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = result_peg_2468;
        stream.update(result_peg_2468.getPosition());
        
        
        return result_peg_2468;
        out_peg_2469:
    
        if (column_peg_2405.chunk10 == 0){
            column_peg_2405.chunk10 = new Chunk10();
        }
        column_peg_2405.chunk10->chunk_function_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2471 = stream.getColumn(position);
    if (column_peg_2471.chunk10 != 0 && column_peg_2471.chunk10->chunk_function_rest.calculated()){
        return column_peg_2471.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2529(stream, "function_rest");
    int myposition = position;
    
    
    
    Result result_peg_2472(myposition);
        
        {
        
            {
                    
                    result_peg_2472.reset();
                    do{
                        Result result_peg_2484(result_peg_2472.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2484.getPosition()))){
                                    result_peg_2484.nextPosition();
                                } else {
                                    goto out_peg_2487;
                                }
                            }
                            result_peg_2484.setValue((void*) " ");
                                
                        }
                        goto success_peg_2485;
                        out_peg_2487:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2484.getPosition()))){
                                    result_peg_2484.nextPosition();
                                } else {
                                    goto out_peg_2489;
                                }
                            }
                            result_peg_2484.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2485;
                        out_peg_2489:
                        goto loop_peg_2483;
                        success_peg_2485:
                        ;
                        result_peg_2472.addResult(result_peg_2484);
                    } while (true);
                    loop_peg_2483:
                    ;
                            
                }
                goto success_peg_2474;
                goto out_peg_2490;
                success_peg_2474:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2472.getPosition()))){
                        result_peg_2472.nextPosition();
                    } else {
                        goto out_peg_2490;
                    }
                }
                result_peg_2472.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2472.reset();
                    do{
                        Result result_peg_2503(result_peg_2472.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2503.getPosition()))){
                                    result_peg_2503.nextPosition();
                                } else {
                                    goto out_peg_2506;
                                }
                            }
                            result_peg_2503.setValue((void*) " ");
                                
                        }
                        goto success_peg_2504;
                        out_peg_2506:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2503.getPosition()))){
                                    result_peg_2503.nextPosition();
                                } else {
                                    goto out_peg_2508;
                                }
                            }
                            result_peg_2503.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2504;
                        out_peg_2508:
                        goto loop_peg_2502;
                        success_peg_2504:
                        ;
                        result_peg_2472.addResult(result_peg_2503);
                    } while (true);
                    loop_peg_2502:
                    ;
                            
                }
                goto success_peg_2493;
                goto out_peg_2490;
                success_peg_2493:
                ;
            
            
            
            int save_peg_2510 = result_peg_2472.getPosition();
                
                result_peg_2472 = rule_all_compare(stream, result_peg_2472.getPosition());
                if (result_peg_2472.error()){
                    
                    result_peg_2472 = Result(save_peg_2510);
                    result_peg_2472.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_2472.reset();
                    do{
                        Result result_peg_2522(result_peg_2472.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2522.getPosition()))){
                                    result_peg_2522.nextPosition();
                                } else {
                                    goto out_peg_2525;
                                }
                            }
                            result_peg_2522.setValue((void*) " ");
                                
                        }
                        goto success_peg_2523;
                        out_peg_2525:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2522.getPosition()))){
                                    result_peg_2522.nextPosition();
                                } else {
                                    goto out_peg_2527;
                                }
                            }
                            result_peg_2522.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2523;
                        out_peg_2527:
                        goto loop_peg_2521;
                        success_peg_2523:
                        ;
                        result_peg_2472.addResult(result_peg_2522);
                    } while (true);
                    loop_peg_2521:
                    ;
                            
                }
                goto success_peg_2512;
                goto out_peg_2490;
                success_peg_2512:
                ;
            
            
            
            result_peg_2472 = rule_value(stream, result_peg_2472.getPosition());
                if (result_peg_2472.error()){
                    goto out_peg_2490;
                }
            
            
        }
        
        if (column_peg_2471.chunk10 == 0){
            column_peg_2471.chunk10 = new Chunk10();
        }
        column_peg_2471.chunk10->chunk_function_rest = result_peg_2472;
        stream.update(result_peg_2472.getPosition());
        
        
        return result_peg_2472;
        out_peg_2490:
        Result result_peg_2528(myposition);
        
        
        
        if (column_peg_2471.chunk10 == 0){
            column_peg_2471.chunk10 = new Chunk10();
        }
        column_peg_2471.chunk10->chunk_function_rest = result_peg_2528;
        stream.update(result_peg_2528.getPosition());
        
        
        return result_peg_2528;
    
        if (column_peg_2471.chunk10 == 0){
            column_peg_2471.chunk10 = new Chunk10();
        }
        column_peg_2471.chunk10->chunk_function_rest = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2530 = stream.getColumn(position);
    if (column_peg_2530.chunk10 != 0 && column_peg_2530.chunk10->chunk_keys.calculated()){
        return column_peg_2530.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_2533(stream, "keys");
    int myposition = position;
    
    
    
    Result result_peg_2531(myposition);
        
        result_peg_2531 = rule_key_value_list(stream, result_peg_2531.getPosition());
        if (result_peg_2531.error()){
            goto out_peg_2532;
        }
        
        if (column_peg_2530.chunk10 == 0){
            column_peg_2530.chunk10 = new Chunk10();
        }
        column_peg_2530.chunk10->chunk_keys = result_peg_2531;
        stream.update(result_peg_2531.getPosition());
        
        
        return result_peg_2531;
        out_peg_2532:
    
        if (column_peg_2530.chunk10 == 0){
            column_peg_2530.chunk10 = new Chunk10();
        }
        column_peg_2530.chunk10->chunk_keys = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2534 = stream.getColumn(position);
    if (column_peg_2534.chunk10 != 0 && column_peg_2534.chunk10->chunk_key_value_list.calculated()){
        return column_peg_2534.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2578(stream, "key_value_list");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_2535(myposition);
        
        {
        
            result_peg_2535 = rule_key(stream, result_peg_2535.getPosition());
                if (result_peg_2535.error()){
                    goto out_peg_2537;
                }
                first = result_peg_2535.getValues();
            
            
            
            result_peg_2535.reset();
                do{
                    Result result_peg_2540(result_peg_2535.getPosition());
                    {
                    
                        {
                                
                                result_peg_2540.reset();
                                do{
                                    Result result_peg_2552(result_peg_2540.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2552.getPosition()))){
                                                result_peg_2552.nextPosition();
                                            } else {
                                                goto out_peg_2555;
                                            }
                                        }
                                        result_peg_2552.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2553;
                                    out_peg_2555:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2552.getPosition()))){
                                                result_peg_2552.nextPosition();
                                            } else {
                                                goto out_peg_2557;
                                            }
                                        }
                                        result_peg_2552.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2553;
                                    out_peg_2557:
                                    goto loop_peg_2551;
                                    success_peg_2553:
                                    ;
                                    result_peg_2540.addResult(result_peg_2552);
                                } while (true);
                                loop_peg_2551:
                                ;
                                        
                            }
                            goto success_peg_2542;
                            goto loop_peg_2539;
                            success_peg_2542:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2540.getPosition()))){
                                    result_peg_2540.nextPosition();
                                } else {
                                    goto loop_peg_2539;
                                }
                            }
                            result_peg_2540.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2540.reset();
                                do{
                                    Result result_peg_2570(result_peg_2540.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2570.getPosition()))){
                                                result_peg_2570.nextPosition();
                                            } else {
                                                goto out_peg_2573;
                                            }
                                        }
                                        result_peg_2570.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2571;
                                    out_peg_2573:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2570.getPosition()))){
                                                result_peg_2570.nextPosition();
                                            } else {
                                                goto out_peg_2575;
                                            }
                                        }
                                        result_peg_2570.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2571;
                                    out_peg_2575:
                                    goto loop_peg_2569;
                                    success_peg_2571:
                                    ;
                                    result_peg_2540.addResult(result_peg_2570);
                                } while (true);
                                loop_peg_2569:
                                ;
                                        
                            }
                            goto success_peg_2560;
                            goto loop_peg_2539;
                            success_peg_2560:
                            ;
                        
                        
                        
                        result_peg_2540 = rule_key(stream, result_peg_2540.getPosition());
                            if (result_peg_2540.error()){
                                goto loop_peg_2539;
                            }
                        
                        
                    }
                    result_peg_2535.addResult(result_peg_2540);
                } while (true);
                loop_peg_2539:
                ;
                rest = result_peg_2535.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyList(first, rest);
                    result_peg_2535.setValue(value);
                }
            
            
        }
        
        if (column_peg_2534.chunk10 == 0){
            column_peg_2534.chunk10 = new Chunk10();
        }
        column_peg_2534.chunk10->chunk_key_value_list = result_peg_2535;
        stream.update(result_peg_2535.getPosition());
        
        
        return result_peg_2535;
        out_peg_2537:
        Result result_peg_2576(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = makeEmptyKeyList();
                    result_peg_2576.setValue(value);
                }
            
            
        }
        
        if (column_peg_2534.chunk10 == 0){
            column_peg_2534.chunk10 = new Chunk10();
        }
        column_peg_2534.chunk10->chunk_key_value_list = result_peg_2576;
        stream.update(result_peg_2576.getPosition());
        
        
        return result_peg_2576;
    
        if (column_peg_2534.chunk10 == 0){
            column_peg_2534.chunk10 = new Chunk10();
        }
        column_peg_2534.chunk10->chunk_key_value_list = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2579 = stream.getColumn(position);
    if (column_peg_2579.chunk10 != 0 && column_peg_2579.chunk10->chunk_key.calculated()){
        return column_peg_2579.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_2587(stream, "key");
    int myposition = position;
    
    
    Value ok;
    Result result_peg_2580(myposition);
        
        {
        
            result_peg_2580 = rule_key_real(stream, result_peg_2580.getPosition());
                if (result_peg_2580.error()){
                    goto out_peg_2582;
                }
            
            Result result_peg_2581 = result_peg_2580;
            
            result_peg_2580 = rule_key_rest(stream, result_peg_2580.getPosition(), result_peg_2581.getValues());
                if (result_peg_2580.error()){
                    goto out_peg_2582;
                }
                ok = result_peg_2580.getValues();
            
            
            
            Result result_peg_2586(result_peg_2580);
                result_peg_2586 = rule_identifier(stream, result_peg_2586.getPosition());
                if (result_peg_2586.error()){
                    goto not_peg_2585;
                }
                goto out_peg_2582;
                not_peg_2585:
                result_peg_2580.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = ok;
                    result_peg_2580.setValue(value);
                }
            
            
        }
        
        if (column_peg_2579.chunk10 == 0){
            column_peg_2579.chunk10 = new Chunk10();
        }
        column_peg_2579.chunk10->chunk_key = result_peg_2580;
        stream.update(result_peg_2580.getPosition());
        
        
        return result_peg_2580;
        out_peg_2582:
    
        if (column_peg_2579.chunk10 == 0){
            column_peg_2579.chunk10 = new Chunk10();
        }
        column_peg_2579.chunk10->chunk_key = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2631(stream, "key_rest");
    int myposition = position;
    
    tail_peg_2590:
    Value another;
        Value new_left;
    Result result_peg_2589(myposition);
        {
        
            {
                    
                    result_peg_2589.reset();
                    do{
                        Result result_peg_2602(result_peg_2589.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2602.getPosition()))){
                                    result_peg_2602.nextPosition();
                                } else {
                                    goto out_peg_2605;
                                }
                            }
                            result_peg_2602.setValue((void*) " ");
                                
                        }
                        goto success_peg_2603;
                        out_peg_2605:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2602.getPosition()))){
                                    result_peg_2602.nextPosition();
                                } else {
                                    goto out_peg_2607;
                                }
                            }
                            result_peg_2602.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2603;
                        out_peg_2607:
                        goto loop_peg_2601;
                        success_peg_2603:
                        ;
                        result_peg_2589.addResult(result_peg_2602);
                    } while (true);
                    loop_peg_2601:
                    ;
                            
                }
                goto success_peg_2592;
                goto out_peg_2608;
                success_peg_2592:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2589.getPosition()))){
                        result_peg_2589.nextPosition();
                    } else {
                        goto out_peg_2608;
                    }
                }
                result_peg_2589.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_2589.reset();
                    do{
                        Result result_peg_2621(result_peg_2589.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2621.getPosition()))){
                                    result_peg_2621.nextPosition();
                                } else {
                                    goto out_peg_2624;
                                }
                            }
                            result_peg_2621.setValue((void*) " ");
                                
                        }
                        goto success_peg_2622;
                        out_peg_2624:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2621.getPosition()))){
                                    result_peg_2621.nextPosition();
                                } else {
                                    goto out_peg_2626;
                                }
                            }
                            result_peg_2621.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2622;
                        out_peg_2626:
                        goto loop_peg_2620;
                        success_peg_2622:
                        ;
                        result_peg_2589.addResult(result_peg_2621);
                    } while (true);
                    loop_peg_2620:
                    ;
                            
                }
                goto success_peg_2611;
                goto out_peg_2608;
                success_peg_2611:
                ;
            
            
            
            result_peg_2589 = rule_key_real(stream, result_peg_2589.getPosition());
                if (result_peg_2589.error()){
                    goto out_peg_2608;
                }
                another = result_peg_2589.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyCombined(left, another);
                    result_peg_2589.setValue(value);
                }
                new_left = result_peg_2589.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2589.getPosition();
        goto tail_peg_2590;
        out_peg_2608:
        Result result_peg_2629(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2629.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2629.getPosition());
        
        return result_peg_2629;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2632 = stream.getColumn(position);
    if (column_peg_2632.chunk11 != 0 && column_peg_2632.chunk11->chunk_key_real.calculated()){
        return column_peg_2632.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_2639(stream, "key_real");
    int myposition = position;
    
    
    Value mods;
        Value name;
    Result result_peg_2633(myposition);
        
        {
        
            result_peg_2633.reset();
                do{
                    Result result_peg_2636(result_peg_2633.getPosition());
                    result_peg_2636 = rule_key_modifier(stream, result_peg_2636.getPosition());
                    if (result_peg_2636.error()){
                        goto loop_peg_2635;
                    }
                    result_peg_2633.addResult(result_peg_2636);
                } while (true);
                loop_peg_2635:
                ;
                mods = result_peg_2633.getValues();
            
            
            
            result_peg_2633 = rule_key_name(stream, result_peg_2633.getPosition());
                if (result_peg_2633.error()){
                    goto out_peg_2638;
                }
                name = result_peg_2633.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = applyKeyModifiers(mods, makeKey(name));
                    result_peg_2633.setValue(value);
                }
            
            
        }
        
        if (column_peg_2632.chunk11 == 0){
            column_peg_2632.chunk11 = new Chunk11();
        }
        column_peg_2632.chunk11->chunk_key_real = result_peg_2633;
        stream.update(result_peg_2633.getPosition());
        
        
        return result_peg_2633;
        out_peg_2638:
    
        if (column_peg_2632.chunk11 == 0){
            column_peg_2632.chunk11 = new Chunk11();
        }
        column_peg_2632.chunk11->chunk_key_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2640 = stream.getColumn(position);
    if (column_peg_2640.chunk11 != 0 && column_peg_2640.chunk11->chunk_key_modifier.calculated()){
        return column_peg_2640.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2661(stream, "key_modifier");
    int myposition = position;
    
    
    Value num;
    Result result_peg_2641(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2641.getPosition()))){
                        result_peg_2641.nextPosition();
                    } else {
                        goto out_peg_2643;
                    }
                }
                result_peg_2641.setValue((void*) "~");
            
            
            
            result_peg_2641.reset();
                do{
                    Result result_peg_2646(result_peg_2641.getPosition());
                    {
                        
                        char letter_peg_2651 = stream.get(result_peg_2646.getPosition());
                        if (letter_peg_2651 != '\0' && strchr("0123456789", letter_peg_2651) != NULL){
                            result_peg_2646.nextPosition();
                            result_peg_2646.setValue((void*) (long) letter_peg_2651);
                        } else {
                            goto out_peg_2650;
                        }
                        
                    }
                    goto success_peg_2647;
                    out_peg_2650:
                    goto loop_peg_2645;
                    success_peg_2647:
                    ;
                    result_peg_2641.addResult(result_peg_2646);
                } while (true);
                loop_peg_2645:
                ;
                num = result_peg_2641.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new ReleaseKeyModifier((int) *parseDouble(num));
                    result_peg_2641.setValue(value);
                }
            
            
        }
        
        if (column_peg_2640.chunk11 == 0){
            column_peg_2640.chunk11 = new Chunk11();
        }
        column_peg_2640.chunk11->chunk_key_modifier = result_peg_2641;
        stream.update(result_peg_2641.getPosition());
        
        
        return result_peg_2641;
        out_peg_2643:
        Result result_peg_2652(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("$"[i], stream.get(result_peg_2652.getPosition()))){
                        result_peg_2652.nextPosition();
                    } else {
                        goto out_peg_2654;
                    }
                }
                result_peg_2652.setValue((void*) "$");
            
            
            
            {
                    Value value((void*) 0);
                    value = new DirectionKeyModifier();
                    result_peg_2652.setValue(value);
                }
            
            
        }
        
        if (column_peg_2640.chunk11 == 0){
            column_peg_2640.chunk11 = new Chunk11();
        }
        column_peg_2640.chunk11->chunk_key_modifier = result_peg_2652;
        stream.update(result_peg_2652.getPosition());
        
        
        return result_peg_2652;
        out_peg_2654:
        Result result_peg_2655(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_2655.getPosition()))){
                        result_peg_2655.nextPosition();
                    } else {
                        goto out_peg_2657;
                    }
                }
                result_peg_2655.setValue((void*) "/");
            
            
            
            {
                    Value value((void*) 0);
                    value = new HeldDownKeyModifier();
                    result_peg_2655.setValue(value);
                }
            
            
        }
        
        if (column_peg_2640.chunk11 == 0){
            column_peg_2640.chunk11 = new Chunk11();
        }
        column_peg_2640.chunk11->chunk_key_modifier = result_peg_2655;
        stream.update(result_peg_2655.getPosition());
        
        
        return result_peg_2655;
        out_peg_2657:
        Result result_peg_2658(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_2658.getPosition()))){
                        result_peg_2658.nextPosition();
                    } else {
                        goto out_peg_2660;
                    }
                }
                result_peg_2658.setValue((void*) ">");
            
            
            
            {
                    Value value((void*) 0);
                    value = new OnlyKeyModifier();
                    result_peg_2658.setValue(value);
                }
            
            
        }
        
        if (column_peg_2640.chunk11 == 0){
            column_peg_2640.chunk11 = new Chunk11();
        }
        column_peg_2640.chunk11->chunk_key_modifier = result_peg_2658;
        stream.update(result_peg_2658.getPosition());
        
        
        return result_peg_2658;
        out_peg_2660:
    
        if (column_peg_2640.chunk11 == 0){
            column_peg_2640.chunk11 = new Chunk11();
        }
        column_peg_2640.chunk11->chunk_key_modifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2662 = stream.getColumn(position);
    if (column_peg_2662.chunk11 != 0 && column_peg_2662.chunk11->chunk_key_name.calculated()){
        return column_peg_2662.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_2693(stream, "key_name");
    int myposition = position;
    
    
    
    Result result_peg_2663(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DB"[i], stream.get(result_peg_2663.getPosition()))){
                result_peg_2663.nextPosition();
            } else {
                goto out_peg_2664;
            }
        }
        result_peg_2663.setValue((void*) "DB");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2663;
        stream.update(result_peg_2663.getPosition());
        
        
        return result_peg_2663;
        out_peg_2664:
        Result result_peg_2665(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("B"[i], stream.get(result_peg_2665.getPosition()))){
                result_peg_2665.nextPosition();
            } else {
                goto out_peg_2666;
            }
        }
        result_peg_2665.setValue((void*) "B");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2665;
        stream.update(result_peg_2665.getPosition());
        
        
        return result_peg_2665;
        out_peg_2666:
        Result result_peg_2667(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DF"[i], stream.get(result_peg_2667.getPosition()))){
                result_peg_2667.nextPosition();
            } else {
                goto out_peg_2668;
            }
        }
        result_peg_2667.setValue((void*) "DF");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2667;
        stream.update(result_peg_2667.getPosition());
        
        
        return result_peg_2667;
        out_peg_2668:
        Result result_peg_2669(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("D"[i], stream.get(result_peg_2669.getPosition()))){
                result_peg_2669.nextPosition();
            } else {
                goto out_peg_2670;
            }
        }
        result_peg_2669.setValue((void*) "D");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2669;
        stream.update(result_peg_2669.getPosition());
        
        
        return result_peg_2669;
        out_peg_2670:
        Result result_peg_2671(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("F"[i], stream.get(result_peg_2671.getPosition()))){
                result_peg_2671.nextPosition();
            } else {
                goto out_peg_2672;
            }
        }
        result_peg_2671.setValue((void*) "F");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2671;
        stream.update(result_peg_2671.getPosition());
        
        
        return result_peg_2671;
        out_peg_2672:
        Result result_peg_2673(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UF"[i], stream.get(result_peg_2673.getPosition()))){
                result_peg_2673.nextPosition();
            } else {
                goto out_peg_2674;
            }
        }
        result_peg_2673.setValue((void*) "UF");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2673;
        stream.update(result_peg_2673.getPosition());
        
        
        return result_peg_2673;
        out_peg_2674:
        Result result_peg_2675(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UB"[i], stream.get(result_peg_2675.getPosition()))){
                result_peg_2675.nextPosition();
            } else {
                goto out_peg_2676;
            }
        }
        result_peg_2675.setValue((void*) "UB");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2675;
        stream.update(result_peg_2675.getPosition());
        
        
        return result_peg_2675;
        out_peg_2676:
        Result result_peg_2677(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("U"[i], stream.get(result_peg_2677.getPosition()))){
                result_peg_2677.nextPosition();
            } else {
                goto out_peg_2678;
            }
        }
        result_peg_2677.setValue((void*) "U");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2677;
        stream.update(result_peg_2677.getPosition());
        
        
        return result_peg_2677;
        out_peg_2678:
        Result result_peg_2679(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("a"[i], stream.get(result_peg_2679.getPosition()))){
                result_peg_2679.nextPosition();
            } else {
                goto out_peg_2680;
            }
        }
        result_peg_2679.setValue((void*) "a");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2679;
        stream.update(result_peg_2679.getPosition());
        
        
        return result_peg_2679;
        out_peg_2680:
        Result result_peg_2681(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("b"[i], stream.get(result_peg_2681.getPosition()))){
                result_peg_2681.nextPosition();
            } else {
                goto out_peg_2682;
            }
        }
        result_peg_2681.setValue((void*) "b");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2681;
        stream.update(result_peg_2681.getPosition());
        
        
        return result_peg_2681;
        out_peg_2682:
        Result result_peg_2683(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("c"[i], stream.get(result_peg_2683.getPosition()))){
                result_peg_2683.nextPosition();
            } else {
                goto out_peg_2684;
            }
        }
        result_peg_2683.setValue((void*) "c");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2683;
        stream.update(result_peg_2683.getPosition());
        
        
        return result_peg_2683;
        out_peg_2684:
        Result result_peg_2685(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("x"[i], stream.get(result_peg_2685.getPosition()))){
                result_peg_2685.nextPosition();
            } else {
                goto out_peg_2686;
            }
        }
        result_peg_2685.setValue((void*) "x");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2685;
        stream.update(result_peg_2685.getPosition());
        
        
        return result_peg_2685;
        out_peg_2686:
        Result result_peg_2687(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("y"[i], stream.get(result_peg_2687.getPosition()))){
                result_peg_2687.nextPosition();
            } else {
                goto out_peg_2688;
            }
        }
        result_peg_2687.setValue((void*) "y");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2687;
        stream.update(result_peg_2687.getPosition());
        
        
        return result_peg_2687;
        out_peg_2688:
        Result result_peg_2689(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("z"[i], stream.get(result_peg_2689.getPosition()))){
                result_peg_2689.nextPosition();
            } else {
                goto out_peg_2690;
            }
        }
        result_peg_2689.setValue((void*) "z");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2689;
        stream.update(result_peg_2689.getPosition());
        
        
        return result_peg_2689;
        out_peg_2690:
        Result result_peg_2691(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("s"[i], stream.get(result_peg_2691.getPosition()))){
                result_peg_2691.nextPosition();
            } else {
                goto out_peg_2692;
            }
        }
        result_peg_2691.setValue((void*) "s");
        
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = result_peg_2691;
        stream.update(result_peg_2691.getPosition());
        
        
        return result_peg_2691;
        out_peg_2692:
    
        if (column_peg_2662.chunk11 == 0){
            column_peg_2662.chunk11 = new Chunk11();
        }
        column_peg_2662.chunk11->chunk_key_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2694 = stream.getColumn(position);
    if (column_peg_2694.chunk11 != 0 && column_peg_2694.chunk11->chunk_value.calculated()){
        return column_peg_2694.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_2712(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_2695(myposition);
        
        result_peg_2695 = rule_float(stream, result_peg_2695.getPosition());
        if (result_peg_2695.error()){
            goto out_peg_2696;
        }
        
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = result_peg_2695;
        stream.update(result_peg_2695.getPosition());
        
        
        return result_peg_2695;
        out_peg_2696:
        Result result_peg_2697(myposition);
        
        result_peg_2697 = rule_integer(stream, result_peg_2697.getPosition());
        if (result_peg_2697.error()){
            goto out_peg_2698;
        }
        
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = result_peg_2697;
        stream.update(result_peg_2697.getPosition());
        
        
        return result_peg_2697;
        out_peg_2698:
        Result result_peg_2699(myposition);
        
        result_peg_2699 = rule_keyword(stream, result_peg_2699.getPosition());
        if (result_peg_2699.error()){
            goto out_peg_2700;
        }
        
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = result_peg_2699;
        stream.update(result_peg_2699.getPosition());
        
        
        return result_peg_2699;
        out_peg_2700:
        Result result_peg_2701(myposition);
        
        {
        
            Result result_peg_2704(result_peg_2701);
                result_peg_2704 = rule_keyword(stream, result_peg_2704.getPosition());
                if (result_peg_2704.error()){
                    goto not_peg_2703;
                }
                goto out_peg_2705;
                not_peg_2703:
                result_peg_2701.setValue((void*)0);
            
            
            
            result_peg_2701 = rule_identifier(stream, result_peg_2701.getPosition());
                if (result_peg_2701.error()){
                    goto out_peg_2705;
                }
            
            
        }
        
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = result_peg_2701;
        stream.update(result_peg_2701.getPosition());
        
        
        return result_peg_2701;
        out_peg_2705:
        Result result_peg_2706(myposition);
        
        result_peg_2706 = rule_range(stream, result_peg_2706.getPosition());
        if (result_peg_2706.error()){
            goto out_peg_2707;
        }
        
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = result_peg_2706;
        stream.update(result_peg_2706.getPosition());
        
        
        return result_peg_2706;
        out_peg_2707:
        Result result_peg_2708(myposition);
        
        result_peg_2708 = rule_string(stream, result_peg_2708.getPosition());
        if (result_peg_2708.error()){
            goto out_peg_2709;
        }
        
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = result_peg_2708;
        stream.update(result_peg_2708.getPosition());
        
        
        return result_peg_2708;
        out_peg_2709:
        Result result_peg_2710(myposition);
        
        result_peg_2710 = rule_hitflag(stream, result_peg_2710.getPosition());
        if (result_peg_2710.error()){
            goto out_peg_2711;
        }
        
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = result_peg_2710;
        stream.update(result_peg_2710.getPosition());
        
        
        return result_peg_2710;
        out_peg_2711:
    
        if (column_peg_2694.chunk11 == 0){
            column_peg_2694.chunk11 = new Chunk11();
        }
        column_peg_2694.chunk11->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2713 = stream.getColumn(position);
    if (column_peg_2713.chunk12 != 0 && column_peg_2713.chunk12->chunk_helper.calculated()){
        return column_peg_2713.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_2772(stream, "helper");
    int myposition = position;
    
    
    Value name;
        Value id;
    Result result_peg_2714(myposition);
        
        {
        
            result_peg_2714 = rule_helper__name(stream, result_peg_2714.getPosition());
                if (result_peg_2714.error()){
                    goto out_peg_2716;
                }
                name = result_peg_2714.getValues();
            
            
            
            {
                    
                    result_peg_2714.reset();
                    do{
                        Result result_peg_2728(result_peg_2714.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2728.getPosition()))){
                                    result_peg_2728.nextPosition();
                                } else {
                                    goto out_peg_2731;
                                }
                            }
                            result_peg_2728.setValue((void*) " ");
                                
                        }
                        goto success_peg_2729;
                        out_peg_2731:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2728.getPosition()))){
                                    result_peg_2728.nextPosition();
                                } else {
                                    goto out_peg_2733;
                                }
                            }
                            result_peg_2728.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2729;
                        out_peg_2733:
                        goto loop_peg_2727;
                        success_peg_2729:
                        ;
                        result_peg_2714.addResult(result_peg_2728);
                    } while (true);
                    loop_peg_2727:
                    ;
                            
                }
                goto success_peg_2718;
                goto out_peg_2716;
                success_peg_2718:
                ;
            
            
            
            int save_peg_2735 = result_peg_2714.getPosition();
                
                result_peg_2714 = rule_helper__expression(stream, result_peg_2714.getPosition());
                if (result_peg_2714.error()){
                    
                    result_peg_2714 = Result(save_peg_2735);
                    result_peg_2714.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_2714.reset();
                    do{
                        Result result_peg_2747(result_peg_2714.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2747.getPosition()))){
                                    result_peg_2747.nextPosition();
                                } else {
                                    goto out_peg_2750;
                                }
                            }
                            result_peg_2747.setValue((void*) " ");
                                
                        }
                        goto success_peg_2748;
                        out_peg_2750:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2747.getPosition()))){
                                    result_peg_2747.nextPosition();
                                } else {
                                    goto out_peg_2752;
                                }
                            }
                            result_peg_2747.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2748;
                        out_peg_2752:
                        goto loop_peg_2746;
                        success_peg_2748:
                        ;
                        result_peg_2714.addResult(result_peg_2747);
                    } while (true);
                    loop_peg_2746:
                    ;
                            
                }
                goto success_peg_2737;
                goto out_peg_2716;
                success_peg_2737:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2714.getPosition()))){
                        result_peg_2714.nextPosition();
                    } else {
                        goto out_peg_2716;
                    }
                }
                result_peg_2714.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2714.reset();
                    do{
                        Result result_peg_2765(result_peg_2714.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2765.getPosition()))){
                                    result_peg_2765.nextPosition();
                                } else {
                                    goto out_peg_2768;
                                }
                            }
                            result_peg_2765.setValue((void*) " ");
                                
                        }
                        goto success_peg_2766;
                        out_peg_2768:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2765.getPosition()))){
                                    result_peg_2765.nextPosition();
                                } else {
                                    goto out_peg_2770;
                                }
                            }
                            result_peg_2765.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2766;
                        out_peg_2770:
                        goto loop_peg_2764;
                        success_peg_2766:
                        ;
                        result_peg_2714.addResult(result_peg_2765);
                    } while (true);
                    loop_peg_2764:
                    ;
                            
                }
                goto success_peg_2755;
                goto out_peg_2716;
                success_peg_2755:
                ;
            
            
            
            result_peg_2714 = rule_helper__identifier(stream, result_peg_2714.getPosition());
                if (result_peg_2714.error()){
                    goto out_peg_2716;
                }
                id = result_peg_2714.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHelper(name, id);
                    result_peg_2714.setValue(value);
                }
            
            
        }
        
        if (column_peg_2713.chunk12 == 0){
            column_peg_2713.chunk12 = new Chunk12();
        }
        column_peg_2713.chunk12->chunk_helper = result_peg_2714;
        stream.update(result_peg_2714.getPosition());
        
        
        return result_peg_2714;
        out_peg_2716:
    
        if (column_peg_2713.chunk12 == 0){
            column_peg_2713.chunk12 = new Chunk12();
        }
        column_peg_2713.chunk12->chunk_helper = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_2773 = stream.getColumn(position);
    if (column_peg_2773.chunk12 != 0 && column_peg_2773.chunk12->chunk_helper__expression.calculated()){
        return column_peg_2773.chunk12->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_2812(stream, "helper__expression");
    int myposition = position;
    
    
    
    Result result_peg_2774(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2774.getPosition()))){
                        result_peg_2774.nextPosition();
                    } else {
                        goto out_peg_2776;
                    }
                }
                result_peg_2774.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2774.reset();
                    do{
                        Result result_peg_2788(result_peg_2774.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2788.getPosition()))){
                                    result_peg_2788.nextPosition();
                                } else {
                                    goto out_peg_2791;
                                }
                            }
                            result_peg_2788.setValue((void*) " ");
                                
                        }
                        goto success_peg_2789;
                        out_peg_2791:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2788.getPosition()))){
                                    result_peg_2788.nextPosition();
                                } else {
                                    goto out_peg_2793;
                                }
                            }
                            result_peg_2788.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2789;
                        out_peg_2793:
                        goto loop_peg_2787;
                        success_peg_2789:
                        ;
                        result_peg_2774.addResult(result_peg_2788);
                    } while (true);
                    loop_peg_2787:
                    ;
                            
                }
                goto success_peg_2778;
                goto out_peg_2776;
                success_peg_2778:
                ;
            
            
            
            result_peg_2774 = rule_expr_c(stream, result_peg_2774.getPosition());
                if (result_peg_2774.error()){
                    goto out_peg_2776;
                }
            
            
            
            {
                    
                    result_peg_2774.reset();
                    do{
                        Result result_peg_2806(result_peg_2774.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2806.getPosition()))){
                                    result_peg_2806.nextPosition();
                                } else {
                                    goto out_peg_2809;
                                }
                            }
                            result_peg_2806.setValue((void*) " ");
                                
                        }
                        goto success_peg_2807;
                        out_peg_2809:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2806.getPosition()))){
                                    result_peg_2806.nextPosition();
                                } else {
                                    goto out_peg_2811;
                                }
                            }
                            result_peg_2806.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2807;
                        out_peg_2811:
                        goto loop_peg_2805;
                        success_peg_2807:
                        ;
                        result_peg_2774.addResult(result_peg_2806);
                    } while (true);
                    loop_peg_2805:
                    ;
                            
                }
                goto success_peg_2796;
                goto out_peg_2776;
                success_peg_2796:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2774.getPosition()))){
                        result_peg_2774.nextPosition();
                    } else {
                        goto out_peg_2776;
                    }
                }
                result_peg_2774.setValue((void*) ")");
            
            
        }
        
        if (column_peg_2773.chunk12 == 0){
            column_peg_2773.chunk12 = new Chunk12();
        }
        column_peg_2773.chunk12->chunk_helper__expression = result_peg_2774;
        stream.update(result_peg_2774.getPosition());
        
        
        return result_peg_2774;
        out_peg_2776:
    
        if (column_peg_2773.chunk12 == 0){
            column_peg_2773.chunk12 = new Chunk12();
        }
        column_peg_2773.chunk12->chunk_helper__expression = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_2813 = stream.getColumn(position);
    if (column_peg_2813.chunk12 != 0 && column_peg_2813.chunk12->chunk_helper__name.calculated()){
        return column_peg_2813.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_2830(stream, "helper__name");
    int myposition = position;
    
    
    
    Result result_peg_2814(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("parent"[i], stream.get(result_peg_2814.getPosition()))){
                result_peg_2814.nextPosition();
            } else {
                goto out_peg_2815;
            }
        }
        result_peg_2814.setValue((void*) "parent");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2814;
        stream.update(result_peg_2814.getPosition());
        
        
        return result_peg_2814;
        out_peg_2815:
        Result result_peg_2816(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("root"[i], stream.get(result_peg_2816.getPosition()))){
                result_peg_2816.nextPosition();
            } else {
                goto out_peg_2817;
            }
        }
        result_peg_2816.setValue((void*) "root");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2816;
        stream.update(result_peg_2816.getPosition());
        
        
        return result_peg_2816;
        out_peg_2817:
        Result result_peg_2818(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("helper"[i], stream.get(result_peg_2818.getPosition()))){
                result_peg_2818.nextPosition();
            } else {
                goto out_peg_2819;
            }
        }
        result_peg_2818.setValue((void*) "helper");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2818;
        stream.update(result_peg_2818.getPosition());
        
        
        return result_peg_2818;
        out_peg_2819:
        Result result_peg_2820(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("target"[i], stream.get(result_peg_2820.getPosition()))){
                result_peg_2820.nextPosition();
            } else {
                goto out_peg_2821;
            }
        }
        result_peg_2820.setValue((void*) "target");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2820;
        stream.update(result_peg_2820.getPosition());
        
        
        return result_peg_2820;
        out_peg_2821:
        Result result_peg_2822(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("partner"[i], stream.get(result_peg_2822.getPosition()))){
                result_peg_2822.nextPosition();
            } else {
                goto out_peg_2823;
            }
        }
        result_peg_2822.setValue((void*) "partner");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2822;
        stream.update(result_peg_2822.getPosition());
        
        
        return result_peg_2822;
        out_peg_2823:
        Result result_peg_2824(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("enemyNear"[i], stream.get(result_peg_2824.getPosition()))){
                result_peg_2824.nextPosition();
            } else {
                goto out_peg_2825;
            }
        }
        result_peg_2824.setValue((void*) "enemyNear");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2824;
        stream.update(result_peg_2824.getPosition());
        
        
        return result_peg_2824;
        out_peg_2825:
        Result result_peg_2826(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("enemy"[i], stream.get(result_peg_2826.getPosition()))){
                result_peg_2826.nextPosition();
            } else {
                goto out_peg_2827;
            }
        }
        result_peg_2826.setValue((void*) "enemy");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2826;
        stream.update(result_peg_2826.getPosition());
        
        
        return result_peg_2826;
        out_peg_2827:
        Result result_peg_2828(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("playerid"[i], stream.get(result_peg_2828.getPosition()))){
                result_peg_2828.nextPosition();
            } else {
                goto out_peg_2829;
            }
        }
        result_peg_2828.setValue((void*) "playerid");
        
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = result_peg_2828;
        stream.update(result_peg_2828.getPosition());
        
        
        return result_peg_2828;
        out_peg_2829:
    
        if (column_peg_2813.chunk12 == 0){
            column_peg_2813.chunk12 = new Chunk12();
        }
        column_peg_2813.chunk12->chunk_helper__name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_2831 = stream.getColumn(position);
    if (column_peg_2831.chunk12 != 0 && column_peg_2831.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_2831.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_2841(stream, "helper__identifier");
    int myposition = position;
    
    
    
    Result result_peg_2832(myposition);
        
        result_peg_2832 = rule_function(stream, result_peg_2832.getPosition());
        if (result_peg_2832.error()){
            goto out_peg_2833;
        }
        
        if (column_peg_2831.chunk12 == 0){
            column_peg_2831.chunk12 = new Chunk12();
        }
        column_peg_2831.chunk12->chunk_helper__identifier = result_peg_2832;
        stream.update(result_peg_2832.getPosition());
        
        
        return result_peg_2832;
        out_peg_2833:
        Result result_peg_2834(myposition);
        
        result_peg_2834 = rule_keyword(stream, result_peg_2834.getPosition());
        if (result_peg_2834.error()){
            goto out_peg_2835;
        }
        
        if (column_peg_2831.chunk12 == 0){
            column_peg_2831.chunk12 = new Chunk12();
        }
        column_peg_2831.chunk12->chunk_helper__identifier = result_peg_2834;
        stream.update(result_peg_2834.getPosition());
        
        
        return result_peg_2834;
        out_peg_2835:
        Result result_peg_2836(myposition);
        
        {
        
            Result result_peg_2839(result_peg_2836);
                result_peg_2839 = rule_keyword(stream, result_peg_2839.getPosition());
                if (result_peg_2839.error()){
                    goto not_peg_2838;
                }
                goto out_peg_2840;
                not_peg_2838:
                result_peg_2836.setValue((void*)0);
            
            
            
            result_peg_2836 = rule_identifier(stream, result_peg_2836.getPosition());
                if (result_peg_2836.error()){
                    goto out_peg_2840;
                }
            
            
        }
        
        if (column_peg_2831.chunk12 == 0){
            column_peg_2831.chunk12 = new Chunk12();
        }
        column_peg_2831.chunk12->chunk_helper__identifier = result_peg_2836;
        stream.update(result_peg_2836.getPosition());
        
        
        return result_peg_2836;
        out_peg_2840:
    
        if (column_peg_2831.chunk12 == 0){
            column_peg_2831.chunk12 = new Chunk12();
        }
        column_peg_2831.chunk12->chunk_helper__identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_2842 = stream.getColumn(position);
    if (column_peg_2842.chunk12 != 0 && column_peg_2842.chunk12->chunk_hitflag.calculated()){
        return column_peg_2842.chunk12->chunk_hitflag;
    }
    
    RuleTrace trace_peg_2867(stream, "hitflag");
    int myposition = position;
    
    
    
    Result result_peg_2843(myposition);
        
        {
        
            result_peg_2843.reset();
                do{
                    Result result_peg_2846(result_peg_2843.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("A"[i], stream.get(result_peg_2846.getPosition()))){
                                result_peg_2846.nextPosition();
                            } else {
                                goto out_peg_2849;
                            }
                        }
                        result_peg_2846.setValue((void*) "A");
                            
                    }
                    goto success_peg_2847;
                    out_peg_2849:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("M"[i], stream.get(result_peg_2846.getPosition()))){
                                result_peg_2846.nextPosition();
                            } else {
                                goto out_peg_2851;
                            }
                        }
                        result_peg_2846.setValue((void*) "M");
                            
                    }
                    goto success_peg_2847;
                    out_peg_2851:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("F"[i], stream.get(result_peg_2846.getPosition()))){
                                result_peg_2846.nextPosition();
                            } else {
                                goto out_peg_2853;
                            }
                        }
                        result_peg_2846.setValue((void*) "F");
                            
                    }
                    goto success_peg_2847;
                    out_peg_2853:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("D"[i], stream.get(result_peg_2846.getPosition()))){
                                result_peg_2846.nextPosition();
                            } else {
                                goto out_peg_2855;
                            }
                        }
                        result_peg_2846.setValue((void*) "D");
                            
                    }
                    goto success_peg_2847;
                    out_peg_2855:
                    goto loop_peg_2845;
                    success_peg_2847:
                    ;
                    result_peg_2843.addResult(result_peg_2846);
                } while (true);
                loop_peg_2845:
                if (result_peg_2843.matches() == 0){
                    goto out_peg_2856;
                }
            
            
            
            int save_peg_2858 = result_peg_2843.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2843.getPosition()))){
                            result_peg_2843.nextPosition();
                        } else {
                            goto out_peg_2861;
                        }
                    }
                    result_peg_2843.setValue((void*) "+");
                        
                }
                goto success_peg_2859;
                out_peg_2861:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2843.getPosition()))){
                            result_peg_2843.nextPosition();
                        } else {
                            goto out_peg_2863;
                        }
                    }
                    result_peg_2843.setValue((void*) "-");
                        
                }
                goto success_peg_2859;
                out_peg_2863:
                
                result_peg_2843 = Result(save_peg_2858);
                result_peg_2843.setValue((void*) 0);
                
                success_peg_2859:
                ;
            
            
            
            Result result_peg_2866(result_peg_2843);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2866.getPosition()))){
                        result_peg_2866.nextPosition();
                    } else {
                        goto not_peg_2865;
                    }
                }
                result_peg_2866.setValue((void*) ".");
                goto out_peg_2856;
                not_peg_2865:
                result_peg_2843.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSimpleIdentifier("A");
                    result_peg_2843.setValue(value);
                }
            
            
        }
        
        if (column_peg_2842.chunk12 == 0){
            column_peg_2842.chunk12 = new Chunk12();
        }
        column_peg_2842.chunk12->chunk_hitflag = result_peg_2843;
        stream.update(result_peg_2843.getPosition());
        
        
        return result_peg_2843;
        out_peg_2856:
    
        if (column_peg_2842.chunk12 == 0){
            column_peg_2842.chunk12 = new Chunk12();
        }
        column_peg_2842.chunk12->chunk_hitflag = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_2868 = stream.getColumn(position);
    if (column_peg_2868.chunk13 != 0 && column_peg_2868.chunk13->chunk_keyword.calculated()){
        return column_peg_2868.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_2875(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_2869(myposition);
        
        {
        
            result_peg_2869 = rule_keyword_real(stream, result_peg_2869.getPosition());
                if (result_peg_2869.error()){
                    goto out_peg_2871;
                }
            
            Result result_peg_2870 = result_peg_2869;
            
            Result result_peg_2874(result_peg_2869);
                result_peg_2874 = rule_alpha_digit(stream, result_peg_2874.getPosition());
                if (result_peg_2874.error()){
                    goto not_peg_2873;
                }
                goto out_peg_2871;
                not_peg_2873:
                result_peg_2869.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_2870.getValues();
                    result_peg_2869.setValue(value);
                }
            
            
        }
        
        if (column_peg_2868.chunk13 == 0){
            column_peg_2868.chunk13 = new Chunk13();
        }
        column_peg_2868.chunk13->chunk_keyword = result_peg_2869;
        stream.update(result_peg_2869.getPosition());
        
        
        return result_peg_2869;
        out_peg_2871:
    
        if (column_peg_2868.chunk13 == 0){
            column_peg_2868.chunk13 = new Chunk13();
        }
        column_peg_2868.chunk13->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_2876 = stream.getColumn(position);
    if (column_peg_2876.chunk13 != 0 && column_peg_2876.chunk13->chunk_keyword_real.calculated()){
        return column_peg_2876.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_3297(stream, "keyword_real");
    int myposition = position;
    
    
    
    Result result_peg_2877(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_2877.getPosition()))){
                        result_peg_2877.nextPosition();
                    } else {
                        goto out_peg_2879;
                    }
                }
                result_peg_2877.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_2877.reset();
                    do{
                        Result result_peg_2892(result_peg_2877.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2892.getPosition()))){
                                    result_peg_2892.nextPosition();
                                } else {
                                    goto out_peg_2895;
                                }
                            }
                            result_peg_2892.setValue((void*) " ");
                                
                        }
                        goto success_peg_2893;
                        out_peg_2895:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2892.getPosition()))){
                                    result_peg_2892.nextPosition();
                                } else {
                                    goto out_peg_2897;
                                }
                            }
                            result_peg_2892.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2893;
                        out_peg_2897:
                        goto loop_peg_2891;
                        success_peg_2893:
                        ;
                        result_peg_2877.addResult(result_peg_2892);
                    } while (true);
                    loop_peg_2891:
                    if (result_peg_2877.matches() == 0){
                        goto out_peg_2890;
                    }
                    
                }
                goto success_peg_2881;
                out_peg_2890:
                goto out_peg_2879;
                success_peg_2881:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_2877.getPosition()))){
                        result_peg_2877.nextPosition();
                    } else {
                        goto out_peg_2879;
                    }
                }
                result_peg_2877.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel y");
                    result_peg_2877.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_2877;
        stream.update(result_peg_2877.getPosition());
        
        
        return result_peg_2877;
        out_peg_2879:
        Result result_peg_2899(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_2899.getPosition()))){
                        result_peg_2899.nextPosition();
                    } else {
                        goto out_peg_2901;
                    }
                }
                result_peg_2899.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_2899.reset();
                    do{
                        Result result_peg_2914(result_peg_2899.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2914.getPosition()))){
                                    result_peg_2914.nextPosition();
                                } else {
                                    goto out_peg_2917;
                                }
                            }
                            result_peg_2914.setValue((void*) " ");
                                
                        }
                        goto success_peg_2915;
                        out_peg_2917:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2914.getPosition()))){
                                    result_peg_2914.nextPosition();
                                } else {
                                    goto out_peg_2919;
                                }
                            }
                            result_peg_2914.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2915;
                        out_peg_2919:
                        goto loop_peg_2913;
                        success_peg_2915:
                        ;
                        result_peg_2899.addResult(result_peg_2914);
                    } while (true);
                    loop_peg_2913:
                    if (result_peg_2899.matches() == 0){
                        goto out_peg_2912;
                    }
                    
                }
                goto success_peg_2903;
                out_peg_2912:
                goto out_peg_2901;
                success_peg_2903:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_2899.getPosition()))){
                        result_peg_2899.nextPosition();
                    } else {
                        goto out_peg_2901;
                    }
                }
                result_peg_2899.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel x");
                    result_peg_2899.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_2899;
        stream.update(result_peg_2899.getPosition());
        
        
        return result_peg_2899;
        out_peg_2901:
        Result result_peg_2921(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_2921.getPosition()))){
                        result_peg_2921.nextPosition();
                    } else {
                        goto out_peg_2923;
                    }
                }
                result_peg_2921.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_2921.reset();
                    do{
                        Result result_peg_2936(result_peg_2921.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2936.getPosition()))){
                                    result_peg_2936.nextPosition();
                                } else {
                                    goto out_peg_2939;
                                }
                            }
                            result_peg_2936.setValue((void*) " ");
                                
                        }
                        goto success_peg_2937;
                        out_peg_2939:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2936.getPosition()))){
                                    result_peg_2936.nextPosition();
                                } else {
                                    goto out_peg_2941;
                                }
                            }
                            result_peg_2936.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2937;
                        out_peg_2941:
                        goto loop_peg_2935;
                        success_peg_2937:
                        ;
                        result_peg_2921.addResult(result_peg_2936);
                    } while (true);
                    loop_peg_2935:
                    if (result_peg_2921.matches() == 0){
                        goto out_peg_2934;
                    }
                    
                }
                goto success_peg_2925;
                out_peg_2934:
                goto out_peg_2923;
                success_peg_2925:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_2921.getPosition()))){
                        result_peg_2921.nextPosition();
                    } else {
                        goto out_peg_2923;
                    }
                }
                result_peg_2921.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos y");
                    result_peg_2921.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_2921;
        stream.update(result_peg_2921.getPosition());
        
        
        return result_peg_2921;
        out_peg_2923:
        Result result_peg_2943(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_2943.getPosition()))){
                        result_peg_2943.nextPosition();
                    } else {
                        goto out_peg_2945;
                    }
                }
                result_peg_2943.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_2943.reset();
                    do{
                        Result result_peg_2958(result_peg_2943.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2958.getPosition()))){
                                    result_peg_2958.nextPosition();
                                } else {
                                    goto out_peg_2961;
                                }
                            }
                            result_peg_2958.setValue((void*) " ");
                                
                        }
                        goto success_peg_2959;
                        out_peg_2961:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2958.getPosition()))){
                                    result_peg_2958.nextPosition();
                                } else {
                                    goto out_peg_2963;
                                }
                            }
                            result_peg_2958.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2959;
                        out_peg_2963:
                        goto loop_peg_2957;
                        success_peg_2959:
                        ;
                        result_peg_2943.addResult(result_peg_2958);
                    } while (true);
                    loop_peg_2957:
                    if (result_peg_2943.matches() == 0){
                        goto out_peg_2956;
                    }
                    
                }
                goto success_peg_2947;
                out_peg_2956:
                goto out_peg_2945;
                success_peg_2947:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_2943.getPosition()))){
                        result_peg_2943.nextPosition();
                    } else {
                        goto out_peg_2945;
                    }
                }
                result_peg_2943.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos x");
                    result_peg_2943.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_2943;
        stream.update(result_peg_2943.getPosition());
        
        
        return result_peg_2943;
        out_peg_2945:
        Result result_peg_2965(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_2965.getPosition()))){
                        result_peg_2965.nextPosition();
                    } else {
                        goto out_peg_2967;
                    }
                }
                result_peg_2965.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_2965.reset();
                    do{
                        Result result_peg_2980(result_peg_2965.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2980.getPosition()))){
                                    result_peg_2980.nextPosition();
                                } else {
                                    goto out_peg_2983;
                                }
                            }
                            result_peg_2980.setValue((void*) " ");
                                
                        }
                        goto success_peg_2981;
                        out_peg_2983:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2980.getPosition()))){
                                    result_peg_2980.nextPosition();
                                } else {
                                    goto out_peg_2985;
                                }
                            }
                            result_peg_2980.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2981;
                        out_peg_2985:
                        goto loop_peg_2979;
                        success_peg_2981:
                        ;
                        result_peg_2965.addResult(result_peg_2980);
                    } while (true);
                    loop_peg_2979:
                    if (result_peg_2965.matches() == 0){
                        goto out_peg_2978;
                    }
                    
                }
                goto success_peg_2969;
                out_peg_2978:
                goto out_peg_2967;
                success_peg_2969:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_2965.getPosition()))){
                        result_peg_2965.nextPosition();
                    } else {
                        goto out_peg_2967;
                    }
                }
                result_peg_2965.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist x");
                    result_peg_2965.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_2965;
        stream.update(result_peg_2965.getPosition());
        
        
        return result_peg_2965;
        out_peg_2967:
        Result result_peg_2987(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_2987.getPosition()))){
                        result_peg_2987.nextPosition();
                    } else {
                        goto out_peg_2989;
                    }
                }
                result_peg_2987.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_2987.reset();
                    do{
                        Result result_peg_3002(result_peg_2987.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3002.getPosition()))){
                                    result_peg_3002.nextPosition();
                                } else {
                                    goto out_peg_3005;
                                }
                            }
                            result_peg_3002.setValue((void*) " ");
                                
                        }
                        goto success_peg_3003;
                        out_peg_3005:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3002.getPosition()))){
                                    result_peg_3002.nextPosition();
                                } else {
                                    goto out_peg_3007;
                                }
                            }
                            result_peg_3002.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3003;
                        out_peg_3007:
                        goto loop_peg_3001;
                        success_peg_3003:
                        ;
                        result_peg_2987.addResult(result_peg_3002);
                    } while (true);
                    loop_peg_3001:
                    if (result_peg_2987.matches() == 0){
                        goto out_peg_3000;
                    }
                    
                }
                goto success_peg_2991;
                out_peg_3000:
                goto out_peg_2989;
                success_peg_2991:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_2987.getPosition()))){
                        result_peg_2987.nextPosition();
                    } else {
                        goto out_peg_2989;
                    }
                }
                result_peg_2987.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist y");
                    result_peg_2987.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_2987;
        stream.update(result_peg_2987.getPosition());
        
        
        return result_peg_2987;
        out_peg_2989:
        Result result_peg_3009(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3009.getPosition()))){
                        result_peg_3009.nextPosition();
                    } else {
                        goto out_peg_3011;
                    }
                }
                result_peg_3009.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3009.reset();
                    do{
                        Result result_peg_3024(result_peg_3009.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3024.getPosition()))){
                                    result_peg_3024.nextPosition();
                                } else {
                                    goto out_peg_3027;
                                }
                            }
                            result_peg_3024.setValue((void*) " ");
                                
                        }
                        goto success_peg_3025;
                        out_peg_3027:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3024.getPosition()))){
                                    result_peg_3024.nextPosition();
                                } else {
                                    goto out_peg_3029;
                                }
                            }
                            result_peg_3024.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3025;
                        out_peg_3029:
                        goto loop_peg_3023;
                        success_peg_3025:
                        ;
                        result_peg_3009.addResult(result_peg_3024);
                    } while (true);
                    loop_peg_3023:
                    if (result_peg_3009.matches() == 0){
                        goto out_peg_3022;
                    }
                    
                }
                goto success_peg_3013;
                out_peg_3022:
                goto out_peg_3011;
                success_peg_3013:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3009.getPosition()))){
                        result_peg_3009.nextPosition();
                    } else {
                        goto out_peg_3011;
                    }
                }
                result_peg_3009.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist x");
                    result_peg_3009.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3009;
        stream.update(result_peg_3009.getPosition());
        
        
        return result_peg_3009;
        out_peg_3011:
        Result result_peg_3031(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3031.getPosition()))){
                        result_peg_3031.nextPosition();
                    } else {
                        goto out_peg_3033;
                    }
                }
                result_peg_3031.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3031.reset();
                    do{
                        Result result_peg_3046(result_peg_3031.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3046.getPosition()))){
                                    result_peg_3046.nextPosition();
                                } else {
                                    goto out_peg_3049;
                                }
                            }
                            result_peg_3046.setValue((void*) " ");
                                
                        }
                        goto success_peg_3047;
                        out_peg_3049:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3046.getPosition()))){
                                    result_peg_3046.nextPosition();
                                } else {
                                    goto out_peg_3051;
                                }
                            }
                            result_peg_3046.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3047;
                        out_peg_3051:
                        goto loop_peg_3045;
                        success_peg_3047:
                        ;
                        result_peg_3031.addResult(result_peg_3046);
                    } while (true);
                    loop_peg_3045:
                    if (result_peg_3031.matches() == 0){
                        goto out_peg_3044;
                    }
                    
                }
                goto success_peg_3035;
                out_peg_3044:
                goto out_peg_3033;
                success_peg_3035:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3031.getPosition()))){
                        result_peg_3031.nextPosition();
                    } else {
                        goto out_peg_3033;
                    }
                }
                result_peg_3031.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist y");
                    result_peg_3031.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3031;
        stream.update(result_peg_3031.getPosition());
        
        
        return result_peg_3031;
        out_peg_3033:
        Result result_peg_3053(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3053.getPosition()))){
                        result_peg_3053.nextPosition();
                    } else {
                        goto out_peg_3055;
                    }
                }
                result_peg_3053.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3053.reset();
                    do{
                        Result result_peg_3068(result_peg_3053.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3068.getPosition()))){
                                    result_peg_3068.nextPosition();
                                } else {
                                    goto out_peg_3071;
                                }
                            }
                            result_peg_3068.setValue((void*) " ");
                                
                        }
                        goto success_peg_3069;
                        out_peg_3071:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3068.getPosition()))){
                                    result_peg_3068.nextPosition();
                                } else {
                                    goto out_peg_3073;
                                }
                            }
                            result_peg_3068.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3069;
                        out_peg_3073:
                        goto loop_peg_3067;
                        success_peg_3069:
                        ;
                        result_peg_3053.addResult(result_peg_3068);
                    } while (true);
                    loop_peg_3067:
                    if (result_peg_3053.matches() == 0){
                        goto out_peg_3066;
                    }
                    
                }
                goto success_peg_3057;
                out_peg_3066:
                goto out_peg_3055;
                success_peg_3057:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3053.getPosition()))){
                        result_peg_3053.nextPosition();
                    } else {
                        goto out_peg_3055;
                    }
                }
                result_peg_3053.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist x");
                    result_peg_3053.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3053;
        stream.update(result_peg_3053.getPosition());
        
        
        return result_peg_3053;
        out_peg_3055:
        Result result_peg_3075(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3075.getPosition()))){
                        result_peg_3075.nextPosition();
                    } else {
                        goto out_peg_3077;
                    }
                }
                result_peg_3075.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3075.reset();
                    do{
                        Result result_peg_3090(result_peg_3075.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3090.getPosition()))){
                                    result_peg_3090.nextPosition();
                                } else {
                                    goto out_peg_3093;
                                }
                            }
                            result_peg_3090.setValue((void*) " ");
                                
                        }
                        goto success_peg_3091;
                        out_peg_3093:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3090.getPosition()))){
                                    result_peg_3090.nextPosition();
                                } else {
                                    goto out_peg_3095;
                                }
                            }
                            result_peg_3090.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3091;
                        out_peg_3095:
                        goto loop_peg_3089;
                        success_peg_3091:
                        ;
                        result_peg_3075.addResult(result_peg_3090);
                    } while (true);
                    loop_peg_3089:
                    if (result_peg_3075.matches() == 0){
                        goto out_peg_3088;
                    }
                    
                }
                goto success_peg_3079;
                out_peg_3088:
                goto out_peg_3077;
                success_peg_3079:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3075.getPosition()))){
                        result_peg_3075.nextPosition();
                    } else {
                        goto out_peg_3077;
                    }
                }
                result_peg_3075.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist y");
                    result_peg_3075.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3075;
        stream.update(result_peg_3075.getPosition());
        
        
        return result_peg_3075;
        out_peg_3077:
        Result result_peg_3097(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3097.getPosition()))){
                        result_peg_3097.nextPosition();
                    } else {
                        goto out_peg_3099;
                    }
                }
                result_peg_3097.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3097.reset();
                    do{
                        Result result_peg_3112(result_peg_3097.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3112.getPosition()))){
                                    result_peg_3112.nextPosition();
                                } else {
                                    goto out_peg_3115;
                                }
                            }
                            result_peg_3112.setValue((void*) " ");
                                
                        }
                        goto success_peg_3113;
                        out_peg_3115:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3112.getPosition()))){
                                    result_peg_3112.nextPosition();
                                } else {
                                    goto out_peg_3117;
                                }
                            }
                            result_peg_3112.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3113;
                        out_peg_3117:
                        goto loop_peg_3111;
                        success_peg_3113:
                        ;
                        result_peg_3097.addResult(result_peg_3112);
                    } while (true);
                    loop_peg_3111:
                    if (result_peg_3097.matches() == 0){
                        goto out_peg_3110;
                    }
                    
                }
                goto success_peg_3101;
                out_peg_3110:
                goto out_peg_3099;
                success_peg_3101:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3097.getPosition()))){
                        result_peg_3097.nextPosition();
                    } else {
                        goto out_peg_3099;
                    }
                }
                result_peg_3097.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist x");
                    result_peg_3097.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3097;
        stream.update(result_peg_3097.getPosition());
        
        
        return result_peg_3097;
        out_peg_3099:
        Result result_peg_3119(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3119.getPosition()))){
                        result_peg_3119.nextPosition();
                    } else {
                        goto out_peg_3121;
                    }
                }
                result_peg_3119.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3119.reset();
                    do{
                        Result result_peg_3134(result_peg_3119.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3134.getPosition()))){
                                    result_peg_3134.nextPosition();
                                } else {
                                    goto out_peg_3137;
                                }
                            }
                            result_peg_3134.setValue((void*) " ");
                                
                        }
                        goto success_peg_3135;
                        out_peg_3137:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3134.getPosition()))){
                                    result_peg_3134.nextPosition();
                                } else {
                                    goto out_peg_3139;
                                }
                            }
                            result_peg_3134.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3135;
                        out_peg_3139:
                        goto loop_peg_3133;
                        success_peg_3135:
                        ;
                        result_peg_3119.addResult(result_peg_3134);
                    } while (true);
                    loop_peg_3133:
                    if (result_peg_3119.matches() == 0){
                        goto out_peg_3132;
                    }
                    
                }
                goto success_peg_3123;
                out_peg_3132:
                goto out_peg_3121;
                success_peg_3123:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3119.getPosition()))){
                        result_peg_3119.nextPosition();
                    } else {
                        goto out_peg_3121;
                    }
                }
                result_peg_3119.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist y");
                    result_peg_3119.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3119;
        stream.update(result_peg_3119.getPosition());
        
        
        return result_peg_3119;
        out_peg_3121:
        Result result_peg_3141(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3141.getPosition()))){
                        result_peg_3141.nextPosition();
                    } else {
                        goto out_peg_3143;
                    }
                }
                result_peg_3141.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3141.reset();
                    do{
                        Result result_peg_3156(result_peg_3141.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3156.getPosition()))){
                                    result_peg_3156.nextPosition();
                                } else {
                                    goto out_peg_3159;
                                }
                            }
                            result_peg_3156.setValue((void*) " ");
                                
                        }
                        goto success_peg_3157;
                        out_peg_3159:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3156.getPosition()))){
                                    result_peg_3156.nextPosition();
                                } else {
                                    goto out_peg_3161;
                                }
                            }
                            result_peg_3156.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3157;
                        out_peg_3161:
                        goto loop_peg_3155;
                        success_peg_3157:
                        ;
                        result_peg_3141.addResult(result_peg_3156);
                    } while (true);
                    loop_peg_3155:
                    if (result_peg_3141.matches() == 0){
                        goto out_peg_3154;
                    }
                    
                }
                goto success_peg_3145;
                out_peg_3154:
                goto out_peg_3143;
                success_peg_3145:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3141.getPosition()))){
                        result_peg_3141.nextPosition();
                    } else {
                        goto out_peg_3143;
                    }
                }
                result_peg_3141.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3141.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3141;
        stream.update(result_peg_3141.getPosition());
        
        
        return result_peg_3141;
        out_peg_3143:
        Result result_peg_3163(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3163.getPosition()))){
                        result_peg_3163.nextPosition();
                    } else {
                        goto out_peg_3165;
                    }
                }
                result_peg_3163.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3163.reset();
                    do{
                        Result result_peg_3178(result_peg_3163.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3178.getPosition()))){
                                    result_peg_3178.nextPosition();
                                } else {
                                    goto out_peg_3181;
                                }
                            }
                            result_peg_3178.setValue((void*) " ");
                                
                        }
                        goto success_peg_3179;
                        out_peg_3181:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3178.getPosition()))){
                                    result_peg_3178.nextPosition();
                                } else {
                                    goto out_peg_3183;
                                }
                            }
                            result_peg_3178.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3179;
                        out_peg_3183:
                        goto loop_peg_3177;
                        success_peg_3179:
                        ;
                        result_peg_3163.addResult(result_peg_3178);
                    } while (true);
                    loop_peg_3177:
                    if (result_peg_3163.matches() == 0){
                        goto out_peg_3176;
                    }
                    
                }
                goto success_peg_3167;
                out_peg_3176:
                goto out_peg_3165;
                success_peg_3167:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3163.getPosition()))){
                        result_peg_3163.nextPosition();
                    } else {
                        goto out_peg_3165;
                    }
                }
                result_peg_3163.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos x");
                    result_peg_3163.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3163;
        stream.update(result_peg_3163.getPosition());
        
        
        return result_peg_3163;
        out_peg_3165:
        Result result_peg_3185(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3185.getPosition()))){
                        result_peg_3185.nextPosition();
                    } else {
                        goto out_peg_3187;
                    }
                }
                result_peg_3185.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3185.reset();
                    do{
                        Result result_peg_3200(result_peg_3185.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3200.getPosition()))){
                                    result_peg_3200.nextPosition();
                                } else {
                                    goto out_peg_3203;
                                }
                            }
                            result_peg_3200.setValue((void*) " ");
                                
                        }
                        goto success_peg_3201;
                        out_peg_3203:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3200.getPosition()))){
                                    result_peg_3200.nextPosition();
                                } else {
                                    goto out_peg_3205;
                                }
                            }
                            result_peg_3200.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3201;
                        out_peg_3205:
                        goto loop_peg_3199;
                        success_peg_3201:
                        ;
                        result_peg_3185.addResult(result_peg_3200);
                    } while (true);
                    loop_peg_3199:
                    if (result_peg_3185.matches() == 0){
                        goto out_peg_3198;
                    }
                    
                }
                goto success_peg_3189;
                out_peg_3198:
                goto out_peg_3187;
                success_peg_3189:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3185.getPosition()))){
                        result_peg_3185.nextPosition();
                    } else {
                        goto out_peg_3187;
                    }
                }
                result_peg_3185.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos y");
                    result_peg_3185.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3185;
        stream.update(result_peg_3185.getPosition());
        
        
        return result_peg_3185;
        out_peg_3187:
        Result result_peg_3207(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3207.getPosition()))){
                        result_peg_3207.nextPosition();
                    } else {
                        goto out_peg_3209;
                    }
                }
                result_peg_3207.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3207.reset();
                    do{
                        Result result_peg_3222(result_peg_3207.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3222.getPosition()))){
                                    result_peg_3222.nextPosition();
                                } else {
                                    goto out_peg_3225;
                                }
                            }
                            result_peg_3222.setValue((void*) " ");
                                
                        }
                        goto success_peg_3223;
                        out_peg_3225:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3222.getPosition()))){
                                    result_peg_3222.nextPosition();
                                } else {
                                    goto out_peg_3227;
                                }
                            }
                            result_peg_3222.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3223;
                        out_peg_3227:
                        goto loop_peg_3221;
                        success_peg_3223:
                        ;
                        result_peg_3207.addResult(result_peg_3222);
                    } while (true);
                    loop_peg_3221:
                    if (result_peg_3207.matches() == 0){
                        goto out_peg_3220;
                    }
                    
                }
                goto success_peg_3211;
                out_peg_3220:
                goto out_peg_3209;
                success_peg_3211:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3207.getPosition()))){
                        result_peg_3207.nextPosition();
                    } else {
                        goto out_peg_3209;
                    }
                }
                result_peg_3207.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3207.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3207;
        stream.update(result_peg_3207.getPosition());
        
        
        return result_peg_3207;
        out_peg_3209:
        Result result_peg_3229(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3229.getPosition()))){
                        result_peg_3229.nextPosition();
                    } else {
                        goto out_peg_3231;
                    }
                }
                result_peg_3229.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3229.reset();
                    do{
                        Result result_peg_3244(result_peg_3229.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3244.getPosition()))){
                                    result_peg_3244.nextPosition();
                                } else {
                                    goto out_peg_3247;
                                }
                            }
                            result_peg_3244.setValue((void*) " ");
                                
                        }
                        goto success_peg_3245;
                        out_peg_3247:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3244.getPosition()))){
                                    result_peg_3244.nextPosition();
                                } else {
                                    goto out_peg_3249;
                                }
                            }
                            result_peg_3244.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3245;
                        out_peg_3249:
                        goto loop_peg_3243;
                        success_peg_3245:
                        ;
                        result_peg_3229.addResult(result_peg_3244);
                    } while (true);
                    loop_peg_3243:
                    if (result_peg_3229.matches() == 0){
                        goto out_peg_3242;
                    }
                    
                }
                goto success_peg_3233;
                out_peg_3242:
                goto out_peg_3231;
                success_peg_3233:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3229.getPosition()))){
                        result_peg_3229.nextPosition();
                    } else {
                        goto out_peg_3231;
                    }
                }
                result_peg_3229.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist y");
                    result_peg_3229.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3229;
        stream.update(result_peg_3229.getPosition());
        
        
        return result_peg_3229;
        out_peg_3231:
        Result result_peg_3251(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3251.getPosition()))){
                        result_peg_3251.nextPosition();
                    } else {
                        goto out_peg_3253;
                    }
                }
                result_peg_3251.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3251.reset();
                    do{
                        Result result_peg_3266(result_peg_3251.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3266.getPosition()))){
                                    result_peg_3266.nextPosition();
                                } else {
                                    goto out_peg_3269;
                                }
                            }
                            result_peg_3266.setValue((void*) " ");
                                
                        }
                        goto success_peg_3267;
                        out_peg_3269:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3266.getPosition()))){
                                    result_peg_3266.nextPosition();
                                } else {
                                    goto out_peg_3271;
                                }
                            }
                            result_peg_3266.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3267;
                        out_peg_3271:
                        goto loop_peg_3265;
                        success_peg_3267:
                        ;
                        result_peg_3251.addResult(result_peg_3266);
                    } while (true);
                    loop_peg_3265:
                    if (result_peg_3251.matches() == 0){
                        goto out_peg_3264;
                    }
                    
                }
                goto success_peg_3255;
                out_peg_3264:
                goto out_peg_3253;
                success_peg_3255:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3251.getPosition()))){
                        result_peg_3251.nextPosition();
                    } else {
                        goto out_peg_3253;
                    }
                }
                result_peg_3251.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist x");
                    result_peg_3251.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3251;
        stream.update(result_peg_3251.getPosition());
        
        
        return result_peg_3251;
        out_peg_3253:
        Result result_peg_3273(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3273.getPosition()))){
                        result_peg_3273.nextPosition();
                    } else {
                        goto out_peg_3275;
                    }
                }
                result_peg_3273.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3273.reset();
                    do{
                        Result result_peg_3288(result_peg_3273.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3288.getPosition()))){
                                    result_peg_3288.nextPosition();
                                } else {
                                    goto out_peg_3291;
                                }
                            }
                            result_peg_3288.setValue((void*) " ");
                                
                        }
                        goto success_peg_3289;
                        out_peg_3291:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3288.getPosition()))){
                                    result_peg_3288.nextPosition();
                                } else {
                                    goto out_peg_3293;
                                }
                            }
                            result_peg_3288.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3289;
                        out_peg_3293:
                        goto loop_peg_3287;
                        success_peg_3289:
                        ;
                        result_peg_3273.addResult(result_peg_3288);
                    } while (true);
                    loop_peg_3287:
                    if (result_peg_3273.matches() == 0){
                        goto out_peg_3286;
                    }
                    
                }
                goto success_peg_3277;
                out_peg_3286:
                goto out_peg_3275;
                success_peg_3277:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3273.getPosition()))){
                        result_peg_3273.nextPosition();
                    } else {
                        goto out_peg_3275;
                    }
                }
                result_peg_3273.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist y");
                    result_peg_3273.setValue(value);
                }
            
            
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3273;
        stream.update(result_peg_3273.getPosition());
        
        
        return result_peg_3273;
        out_peg_3275:
        Result result_peg_3295(myposition);
        
        result_peg_3295 = rule_hitflag(stream, result_peg_3295.getPosition());
        if (result_peg_3295.error()){
            goto out_peg_3296;
        }
        
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = result_peg_3295;
        stream.update(result_peg_3295.getPosition());
        
        
        return result_peg_3295;
        out_peg_3296:
    
        if (column_peg_2876.chunk13 == 0){
            column_peg_2876.chunk13 = new Chunk13();
        }
        column_peg_2876.chunk13->chunk_keyword_real = errorResult;
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

        
