

#include "mugen/ast/all.h"
#include <map>
#include "gc.h"
// #include "cmd-functions.h"
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

    Value(const Value & him):
    which(him.which),
    value(0){
        if (him.isData()){
            value = him.value;
        }
        if (him.isList()){
            values = him.values;
        }
    }

    explicit Value(const void * value):
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

    Value & operator=(const void * what){
        this->value = what;
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
    Result chunk_compare_equal;
};

struct Chunk10{
    Result chunk_hitdef__attack__attribute;
    Result chunk_hitdef__attack__attribute__item;
    Result chunk_hitdef__attack__type;
    Result chunk_hitdef__attack__movement;
    Result chunk_hitdef__attribute;
};

struct Chunk11{
    Result chunk_hitdef__attribute__item;
    Result chunk_args;
    Result chunk_function_name;
    Result chunk_function_rest;
    Result chunk_keys;
};

struct Chunk12{
    Result chunk_key_value_list;
    Result chunk_key;
    Result chunk_key_rest;
    Result chunk_key_real;
    Result chunk_key_modifier;
};

struct Chunk13{
    Result chunk_key_name;
    Result chunk_value;
    Result chunk_helper;
    Result chunk_helper__expression;
    Result chunk_helper__name;
};

struct Chunk14{
    Result chunk_helper__identifier;
    Result chunk_hitflag;
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
        ,chunk13(0)
        ,chunk14(0){
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
    Chunk14 * chunk14;

    int hitCount(){
        return 0;
    }

    int maxHits(){
        return 74;
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
        delete chunk14;
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
Result rule_compare_equal(Stream &, const int);
Result rule_hitdef__attack__attribute(Stream &, const int);
Result rule_hitdef__attack__attribute__item(Stream &, const int, Value attribute);
Result rule_hitdef__attack__type(Stream &, const int);
Result rule_hitdef__attack__movement(Stream &, const int);
Result rule_hitdef__attribute(Stream &, const int);
Result rule_hitdef__attribute__item(Stream &, const int);
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



Ast::Key * makeKeyModifier(Ast::Key * in, Ast::KeyModifier::ModifierType type, int ticks = 0);

/* in cmd-functions.h */
template<class X>
X as(const Value & value){
    return (X) value.getValue();
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

/* end in cmd-functions.h */

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

/*
Ast::Value * makeExpression(){
    Ast::Value * object = new Ast::Expression();
    GC::save(object);
    return object;
}
*/

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
        Ast::Value * value = as<Ast::Value*>(Value((*it).getValue()));
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
    return makeFunction(name, Value(makeValueList(arg1, Value())));
}

Ast::Value * makeFunction1(const std::string & name, const Value & arg1){
    return makeFunction(name, Value(makeValueList(arg1, Value())));
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
        ids.push_back(*as<std::string*>(Value((*it).getValue())));
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

Ast::Identifier * makeSimpleIdentifier(int line, int column, const char * name){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(line, column, name);
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

Ast::Value * copyValue(const Value & value){
    Ast::Value * copied = (Ast::Value*) as<Ast::Value*>(value)->copy();
    GC::save(copied);
    return copied;
}

Ast::Section * makeSection(const Value & str){
    Ast::Section * object = new Ast::Section(as<std::string*>(str));
    GC::save(object);
    return object;
}

Ast::Key * makeKeyModifier(Ast::Key * in, Ast::KeyModifier::ModifierType type, int ticks){
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

Ast::Key * applyKeyModifiers(const Value & mods, Ast::Key * key){
    for (Value::iterator it = mods.getValues().begin(); it != mods.getValues().end(); it++){
        KeyModifier * modifier = (KeyModifier*) (*it).getValue();
        key = modifier->apply(key);
        delete modifier;
    }
    return key;
}

Ast::HitDefAttackAttribute * makeHitDefAttackAttribute(){
    Ast::HitDefAttackAttribute * object = new Ast::HitDefAttackAttribute();
    GC::save(object);
    return object;
}

Ast::HitDefAttribute * makeHitDefAttribute(const Value & input){
    std::ostringstream out;
    for (Value::iterator it = input.getValues().begin(); it != input.getValues().end(); it++){
        out << (char*) (*it).getValue();
    }
    Ast::HitDefAttribute * object = new Ast::HitDefAttribute(out.str());
    GC::save(object);
    return object;
}

Ast::Value * makeAnimElem(const Value & line, const Value & arg1, const Value & comparison, const Value & arg2){
    /* animelemno == arg1 && animelemtime(arg1) compare arg2 */
    Ast::Value * element;
    Ast::Value * time;
    element = makeExpressionEquals(Value(makeSimpleIdentifier(getCurrentLine(line), getCurrentColumn(line), "anim")), arg1);
    /* make a copy of arg1 because values can only have one parent */
    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
    time = as<compare_func>(comparison)(Value(makeFunction1(std::string("animelemtime"), Value(copyValue(arg1)))), arg2);
    return makeExpressionAnd(Value(element), Value(time));
}

Ast::Value * makeAnimElem(const Value & line, const Value & arg1){
    Value comparison = Value((void*) makeExpressionEquals);
    Value arg2 = Value(makeNumber(0));
    return makeAnimElem(line, arg1, comparison, arg2);
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
                        result_peg_8.setValue(Value((void*) "\n"));
                            
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
                        result_peg_8.setValue(Value((void*) "\r"));
                            
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
                                            result_peg_48.setValue(Value((void*) " "));
                                                
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
                                            result_peg_48.setValue(Value((void*) "\t"));
                                                
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
                                            result_peg_48.setValue(Value((void*) "\n"));
                                                
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
                                            result_peg_48.setValue(Value((void*) "\r"));
                                                
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
                                result_peg_16.setValue(Value((void*) 0));
                                
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
                                result_peg_108.setValue(Value((void*) " "));
                                    
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
                                result_peg_108.setValue(Value((void*) "\t"));
                                    
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
                                result_peg_108.setValue(Value((void*) "\n"));
                                    
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
                                result_peg_108.setValue(Value((void*) "\r"));
                                    
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
                    result_peg_2.setValue(Value((void *) '\0'));
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
    
    RuleTrace trace_peg_67(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_14(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_17;
                                }
                            }
                            result_peg_14.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_15;
                        out_peg_17:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_14.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_15;
                        out_peg_19:
                        goto loop_peg_13;
                        success_peg_15:
                        ;
                        result_peg_2.addResult(result_peg_14);
                    } while (true);
                    loop_peg_13:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_20;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_20;
                }
            
            
            
            Result result_peg_22(result_peg_2.getPosition());
                result_peg_22 = rule_line_end(stream, result_peg_22.getPosition());
                if (result_peg_22.error()){
                    goto out_peg_20;
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_20:
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
                            result_peg_35.setValue(Value((void*) " "));
                                
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
                            result_peg_35.setValue(Value((void*) "\t"));
                                
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
            
            
            
            result_peg_23 = rule_section(stream, result_peg_23.getPosition());
                if (result_peg_23.error()){
                    goto out_peg_41;
                }
            
            Result result_peg_42 = result_peg_23;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_42.getValues());
                    result_peg_23.setValue(value);
                }
            
            
        }
        stream.update(result_peg_23.getPosition());
        
        return result_peg_23;
        out_peg_41:
        Result result_peg_43(myposition);
        
        {
        
            {
                    
                    result_peg_43.reset();
                    do{
                        Result result_peg_55(result_peg_43.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    goto out_peg_58;
                                }
                            }
                            result_peg_55.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_56;
                        out_peg_58:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    goto out_peg_60;
                                }
                            }
                            result_peg_55.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_56;
                        out_peg_60:
                        goto loop_peg_54;
                        success_peg_56:
                        ;
                        result_peg_43.addResult(result_peg_55);
                    } while (true);
                    loop_peg_54:
                    ;
                            
                }
                goto success_peg_45;
                goto out_peg_61;
                success_peg_45:
                ;
            
            
            
            {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_43.getPosition()))){
                            result_peg_43.nextPosition();
                        } else {
                            goto out_peg_64;
                        }
                    }
                    result_peg_43.setValue(Value((void*) "\n"));
                        
                }
                goto success_peg_62;
                out_peg_64:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_43.getPosition()))){
                            result_peg_43.nextPosition();
                        } else {
                            goto out_peg_66;
                        }
                    }
                    result_peg_43.setValue(Value((void*) "\r"));
                        
                }
                goto success_peg_62;
                out_peg_66:
                goto out_peg_61;
                success_peg_62:
                ;
            
            
        }
        stream.update(result_peg_43.getPosition());
        
        return result_peg_43;
        out_peg_61:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_line_end.calculated()){
        return column_peg_1.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_15(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2.reset();
        do{
            Result result_peg_4(result_peg_2.getPosition());
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
                result_peg_4.setValue(Value((void*) "\n"));
                    
            }
            goto success_peg_5;
            out_peg_7:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        goto out_peg_9;
                    }
                }
                result_peg_4.setValue(Value((void*) "\r"));
                    
            }
            goto success_peg_5;
            out_peg_9:
            goto loop_peg_3;
            success_peg_5:
            ;
            result_peg_2.addResult(result_peg_4);
        } while (true);
        loop_peg_3:
        if (result_peg_2.matches() == 0){
            goto out_peg_10;
        }
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_10:
        Result result_peg_11(myposition);
        
        {
        
            Result result_peg_13(result_peg_11.getPosition());
                if ('\0' == stream.get(result_peg_13.getPosition())){
                    result_peg_13.nextPosition();
                    result_peg_13.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_14;
                }
            
            
            
            
            
            
        }
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_14:
    
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_whitespace.calculated()){
        return column_peg_1.chunk0->chunk_whitespace;
    }
    
    RuleTrace trace_peg_5(stream, "whitespace");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2.reset();
        do{
            Result result_peg_4(result_peg_2.getPosition());
            result_peg_4 = rule_sw(stream, result_peg_4.getPosition());
            if (result_peg_4.error()){
                goto loop_peg_3;
            }
            result_peg_2.addResult(result_peg_4);
        } while (true);
        loop_peg_3:
        ;
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_whitespace = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
    
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_sw.calculated()){
        return column_peg_1.chunk0->chunk_sw;
    }
    
    RuleTrace trace_peg_11(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    goto out_peg_5;
                }
            }
            result_peg_2.setValue(Value((void*) " "));
                
        }
        goto success_peg_3;
        out_peg_5:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    goto out_peg_7;
                }
            }
            result_peg_2.setValue(Value((void*) "\t"));
                
        }
        goto success_peg_3;
        out_peg_7:
        goto out_peg_8;
        success_peg_3:
        ;
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_sw = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_8:
        Result result_peg_9(myposition);
        
        result_peg_9 = rule_comment(stream, result_peg_9.getPosition());
        if (result_peg_9.error()){
            goto out_peg_10;
        }
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_sw = result_peg_9;
        stream.update(result_peg_9.getPosition());
        
        
        return result_peg_9;
        out_peg_10:
    
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_comment.calculated()){
        return column_peg_1.chunk1->chunk_comment;
    }
    
    RuleTrace trace_peg_60(stream, "comment");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) ";"));
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_6(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_9(result_peg_6);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_9.getPosition()))){
                                        result_peg_9.nextPosition();
                                    } else {
                                        goto out_peg_12;
                                    }
                                }
                                result_peg_9.setValue(Value((void*) "\n"));
                                    
                            }
                            goto success_peg_10;
                            out_peg_12:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_9.getPosition()))){
                                        result_peg_9.nextPosition();
                                    } else {
                                        goto out_peg_14;
                                    }
                                }
                                result_peg_9.setValue(Value((void*) "\r"));
                                    
                            }
                            goto success_peg_10;
                            out_peg_14:
                            goto not_peg_8;
                            success_peg_10:
                            ;
                            goto loop_peg_5;
                            not_peg_8:
                            result_peg_6.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_15 = stream.get(result_peg_6.getPosition());
                            if (temp_peg_15 != '\0'){
                                result_peg_6.setValue(Value((void*) (long) temp_peg_15));
                                result_peg_6.nextPosition();
                            } else {
                                goto loop_peg_5;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_16(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_16.getPosition()))){
                        result_peg_16.nextPosition();
                    } else {
                        goto out_peg_18;
                    }
                }
                result_peg_16.setValue(Value((void*) "="));
            
            
            
            result_peg_16.reset();
                do{
                    Result result_peg_20(result_peg_16.getPosition());
                    {
                    
                        Result result_peg_23(result_peg_20);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_23.getPosition()))){
                                        result_peg_23.nextPosition();
                                    } else {
                                        goto out_peg_26;
                                    }
                                }
                                result_peg_23.setValue(Value((void*) "\n"));
                                    
                            }
                            goto success_peg_24;
                            out_peg_26:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_23.getPosition()))){
                                        result_peg_23.nextPosition();
                                    } else {
                                        goto out_peg_28;
                                    }
                                }
                                result_peg_23.setValue(Value((void*) "\r"));
                                    
                            }
                            goto success_peg_24;
                            out_peg_28:
                            goto not_peg_22;
                            success_peg_24:
                            ;
                            goto loop_peg_19;
                            not_peg_22:
                            result_peg_20.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_29 = stream.get(result_peg_20.getPosition());
                            if (temp_peg_29 != '\0'){
                                result_peg_20.setValue(Value((void*) (long) temp_peg_29));
                                result_peg_20.nextPosition();
                            } else {
                                goto loop_peg_19;
                            }
                        
                        
                    }
                    result_peg_16.addResult(result_peg_20);
                } while (true);
                loop_peg_19:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_18:
        Result result_peg_30(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_32;
                    }
                }
                result_peg_30.setValue(Value((void*) "-"));
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_32;
                    }
                }
                result_peg_30.setValue(Value((void*) "-"));
            
            
            
            result_peg_30.reset();
                do{
                    Result result_peg_35(result_peg_30.getPosition());
                    {
                    
                        Result result_peg_38(result_peg_35);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_38.getPosition()))){
                                        result_peg_38.nextPosition();
                                    } else {
                                        goto out_peg_41;
                                    }
                                }
                                result_peg_38.setValue(Value((void*) "\n"));
                                    
                            }
                            goto success_peg_39;
                            out_peg_41:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_38.getPosition()))){
                                        result_peg_38.nextPosition();
                                    } else {
                                        goto out_peg_43;
                                    }
                                }
                                result_peg_38.setValue(Value((void*) "\r"));
                                    
                            }
                            goto success_peg_39;
                            out_peg_43:
                            goto not_peg_37;
                            success_peg_39:
                            ;
                            goto loop_peg_34;
                            not_peg_37:
                            result_peg_35.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_44 = stream.get(result_peg_35.getPosition());
                            if (temp_peg_44 != '\0'){
                                result_peg_35.setValue(Value((void*) (long) temp_peg_44));
                                result_peg_35.nextPosition();
                            } else {
                                goto loop_peg_34;
                            }
                        
                        
                    }
                    result_peg_30.addResult(result_peg_35);
                } while (true);
                loop_peg_34:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_30;
        stream.update(result_peg_30.getPosition());
        
        
        return result_peg_30;
        out_peg_32:
        Result result_peg_45(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_45.getPosition()))){
                        result_peg_45.nextPosition();
                    } else {
                        goto out_peg_47;
                    }
                }
                result_peg_45.setValue(Value((void*) ":"));
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_45.getPosition()))){
                        result_peg_45.nextPosition();
                    } else {
                        goto out_peg_47;
                    }
                }
                result_peg_45.setValue(Value((void*) ":"));
            
            
            
            result_peg_45.reset();
                do{
                    Result result_peg_50(result_peg_45.getPosition());
                    {
                    
                        Result result_peg_53(result_peg_50);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_53.getPosition()))){
                                        result_peg_53.nextPosition();
                                    } else {
                                        goto out_peg_56;
                                    }
                                }
                                result_peg_53.setValue(Value((void*) "\n"));
                                    
                            }
                            goto success_peg_54;
                            out_peg_56:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_53.getPosition()))){
                                        result_peg_53.nextPosition();
                                    } else {
                                        goto out_peg_58;
                                    }
                                }
                                result_peg_53.setValue(Value((void*) "\r"));
                                    
                            }
                            goto success_peg_54;
                            out_peg_58:
                            goto not_peg_52;
                            success_peg_54:
                            ;
                            goto loop_peg_49;
                            not_peg_52:
                            result_peg_50.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_59 = stream.get(result_peg_50.getPosition());
                            if (temp_peg_59 != '\0'){
                                result_peg_50.setValue(Value((void*) (long) temp_peg_59));
                                result_peg_50.nextPosition();
                            } else {
                                goto loop_peg_49;
                            }
                        
                        
                    }
                    result_peg_45.addResult(result_peg_50);
                } while (true);
                loop_peg_49:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_45;
        stream.update(result_peg_45.getPosition());
        
        
        return result_peg_45;
        out_peg_47:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_section.calculated()){
        return column_peg_1.chunk1->chunk_section;
    }
    
    RuleTrace trace_peg_69(stream, "section");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_section_title(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeSection(result_peg_3.getValues());
                    result_peg_2.setValue(value);
                }
                ast = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_whitespace(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_9(result_peg_2.getPosition());
                    {
                    
                        {
                                
                                result_peg_9.reset();
                                do{
                                    Result result_peg_41(result_peg_9.getPosition());
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar(" "[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    goto out_peg_52;
                                                }
                                            }
                                            result_peg_41.setValue(Value((void*) " "));
                                                
                                        }
                                        goto success_peg_50;
                                        out_peg_52:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\t"[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    goto out_peg_54;
                                                }
                                            }
                                            result_peg_41.setValue(Value((void*) "\t"));
                                                
                                        }
                                        goto success_peg_50;
                                        out_peg_54:
                                        goto out_peg_49;
                                        success_peg_50:
                                        ;
                                        
                                    }
                                    goto success_peg_42;
                                    out_peg_49:
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    goto out_peg_64;
                                                }
                                            }
                                            result_peg_41.setValue(Value((void*) "\n"));
                                                
                                        }
                                        goto success_peg_62;
                                        out_peg_64:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    goto out_peg_66;
                                                }
                                            }
                                            result_peg_41.setValue(Value((void*) "\r"));
                                                
                                        }
                                        goto success_peg_62;
                                        out_peg_66:
                                        goto out_peg_61;
                                        success_peg_62:
                                        ;
                                        
                                    }
                                    goto success_peg_42;
                                    out_peg_61:
                                    goto loop_peg_40;
                                    success_peg_42:
                                    ;
                                    result_peg_9.addResult(result_peg_41);
                                } while (true);
                                loop_peg_40:
                                ;
                                        
                            }
                            goto success_peg_11;
                            goto loop_peg_8;
                            success_peg_11:
                            ;
                        
                        
                        
                        result_peg_9 = rule_section_item(stream, result_peg_9.getPosition(), ast);
                            if (result_peg_9.error()){
                                goto loop_peg_8;
                            }
                        
                        
                        
                        result_peg_9 = rule_whitespace(stream, result_peg_9.getPosition());
                            if (result_peg_9.error()){
                                goto loop_peg_8;
                            }
                        
                        
                        
                        result_peg_9 = rule_line_end(stream, result_peg_9.getPosition());
                            if (result_peg_9.error()){
                                goto loop_peg_8;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_section = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_section = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    RuleTrace trace_peg_41(stream, "section_item");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_14(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_17;
                                }
                            }
                            result_peg_14.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_15;
                        out_peg_17:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_14.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_15;
                        out_peg_19:
                        goto loop_peg_13;
                        success_peg_15:
                        ;
                        result_peg_2.addResult(result_peg_14);
                    } while (true);
                    loop_peg_13:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_20;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_20;
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_20:
        Result result_peg_21(myposition);
        
        {
        
            {
                    
                    result_peg_21.reset();
                    do{
                        Result result_peg_33(result_peg_21.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_33.getPosition()))){
                                    result_peg_33.nextPosition();
                                } else {
                                    goto out_peg_36;
                                }
                            }
                            result_peg_33.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_34;
                        out_peg_36:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_33.getPosition()))){
                                    result_peg_33.nextPosition();
                                } else {
                                    goto out_peg_38;
                                }
                            }
                            result_peg_33.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_34;
                        out_peg_38:
                        goto loop_peg_32;
                        success_peg_34:
                        ;
                        result_peg_21.addResult(result_peg_33);
                    } while (true);
                    loop_peg_32:
                    ;
                            
                }
                goto success_peg_23;
                goto out_peg_39;
                success_peg_23:
                ;
            
            
            
            result_peg_21 = rule_assignment(stream, result_peg_21.getPosition());
                if (result_peg_21.error()){
                    goto out_peg_39;
                }
                data = result_peg_21.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_21.setValue(value);
                }
            
            
        }
        stream.update(result_peg_21.getPosition());
        
        return result_peg_21;
        out_peg_39:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_section_title.calculated()){
        return column_peg_1.chunk1->chunk_section_title;
    }
    
    RuleTrace trace_peg_30(stream, "section_title");
    int myposition = position;
    
    
    Value name;
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "["));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_24(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_27(result_peg_24);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    goto not_peg_26;
                                }
                            }
                            result_peg_27.setValue(Value((void*) "]"));
                            goto loop_peg_23;
                            not_peg_26:
                            result_peg_24.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_28 = stream.get(result_peg_24.getPosition());
                            if (temp_peg_28 != '\0'){
                                result_peg_24.setValue(Value((void*) (long) temp_peg_28));
                                result_peg_24.nextPosition();
                            } else {
                                goto loop_peg_23;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_24);
                } while (true);
                loop_peg_23:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
                name = result_peg_2.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "]"));
            
            
            
            {
                    Value value((void*) 0);
                    value = toString(name);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_section_title = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_section_title = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_assignment.calculated()){
        return column_peg_1.chunk1->chunk_assignment;
    }
    
    RuleTrace trace_peg_327(stream, "assignment");
    int myposition = position;
    
    
    Value all;
        Value line;
        Value own;
        Value exp;
        Value name;
        Value index;
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 7; i++){
                    if (compareCharCase("command"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "command"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_4;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_keys(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                all = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("command", all);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_41(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_43 = stream.getLineInfo(result_peg_41.getPosition());
                line = &line_info_peg_43;
            
            
            
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("guardsound"[i], stream.get(result_peg_41.getPosition()))){
                        result_peg_41.nextPosition();
                    } else {
                        goto out_peg_45;
                    }
                }
                result_peg_41.setValue(Value((void*) "guardsound"));
            
            
            
            {
                    
                    result_peg_41.reset();
                    do{
                        Result result_peg_57(result_peg_41.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_57.getPosition()))){
                                    result_peg_57.nextPosition();
                                } else {
                                    goto out_peg_60;
                                }
                            }
                            result_peg_57.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_58;
                        out_peg_60:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_57.getPosition()))){
                                    result_peg_57.nextPosition();
                                } else {
                                    goto out_peg_62;
                                }
                            }
                            result_peg_57.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_58;
                        out_peg_62:
                        goto loop_peg_56;
                        success_peg_58:
                        ;
                        result_peg_41.addResult(result_peg_57);
                    } while (true);
                    loop_peg_56:
                    ;
                            
                }
                goto success_peg_47;
                goto out_peg_45;
                success_peg_47:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_41.getPosition()))){
                        result_peg_41.nextPosition();
                    } else {
                        goto out_peg_45;
                    }
                }
                result_peg_41.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_41.reset();
                    do{
                        Result result_peg_75(result_peg_41.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_75.getPosition()))){
                                    result_peg_75.nextPosition();
                                } else {
                                    goto out_peg_78;
                                }
                            }
                            result_peg_75.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_76;
                        out_peg_78:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_75.getPosition()))){
                                    result_peg_75.nextPosition();
                                } else {
                                    goto out_peg_80;
                                }
                            }
                            result_peg_75.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_76;
                        out_peg_80:
                        goto loop_peg_74;
                        success_peg_76:
                        ;
                        result_peg_41.addResult(result_peg_75);
                    } while (true);
                    loop_peg_74:
                    ;
                            
                }
                goto success_peg_65;
                goto out_peg_45;
                success_peg_65:
                ;
            
            
            
            int save_peg_82 = result_peg_41.getPosition();
                
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_41.getPosition()))){
                        result_peg_41.nextPosition();
                    } else {
                        
                        result_peg_41 = Result(save_peg_82);
                        result_peg_41.setValue(Value((void*) 0));
                        
                    }
                }
                result_peg_41.setValue(Value((void*) "s"));
                own = result_peg_41.getValues();
            
            
            
            result_peg_41 = rule_expr(stream, result_peg_41.getPosition());
                if (result_peg_41.error()){
                    goto out_peg_45;
                }
                exp = result_peg_41.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    if (own.getValue() != 0){
                                            value = makeAttribute("guardsound-own", exp);
                                        } else {
                                            value = makeAttribute("guardsound", exp);
                                        }
                    result_peg_41.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_41;
        stream.update(result_peg_41.getPosition());
        
        
        return result_peg_41;
        out_peg_45:
        Result result_peg_84(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_86 = stream.getLineInfo(result_peg_84.getPosition());
                line = &line_info_peg_86;
            
            
            
            for (int i = 0; i < 7; i++){
                    if (compareCharCase("sparkno"[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        goto out_peg_88;
                    }
                }
                result_peg_84.setValue(Value((void*) "sparkno"));
            
            
            
            {
                    
                    result_peg_84.reset();
                    do{
                        Result result_peg_100(result_peg_84.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_100.getPosition()))){
                                    result_peg_100.nextPosition();
                                } else {
                                    goto out_peg_103;
                                }
                            }
                            result_peg_100.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_101;
                        out_peg_103:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_100.getPosition()))){
                                    result_peg_100.nextPosition();
                                } else {
                                    goto out_peg_105;
                                }
                            }
                            result_peg_100.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_101;
                        out_peg_105:
                        goto loop_peg_99;
                        success_peg_101:
                        ;
                        result_peg_84.addResult(result_peg_100);
                    } while (true);
                    loop_peg_99:
                    ;
                            
                }
                goto success_peg_90;
                goto out_peg_88;
                success_peg_90:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        goto out_peg_88;
                    }
                }
                result_peg_84.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_84.reset();
                    do{
                        Result result_peg_118(result_peg_84.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_118.getPosition()))){
                                    result_peg_118.nextPosition();
                                } else {
                                    goto out_peg_121;
                                }
                            }
                            result_peg_118.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_119;
                        out_peg_121:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_118.getPosition()))){
                                    result_peg_118.nextPosition();
                                } else {
                                    goto out_peg_123;
                                }
                            }
                            result_peg_118.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_119;
                        out_peg_123:
                        goto loop_peg_117;
                        success_peg_119:
                        ;
                        result_peg_84.addResult(result_peg_118);
                    } while (true);
                    loop_peg_117:
                    ;
                            
                }
                goto success_peg_108;
                goto out_peg_88;
                success_peg_108:
                ;
            
            
            
            int save_peg_125 = result_peg_84.getPosition();
                
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        
                        result_peg_84 = Result(save_peg_125);
                        result_peg_84.setValue(Value((void*) 0));
                        
                    }
                }
                result_peg_84.setValue(Value((void*) "s"));
            
            
            
            result_peg_84 = rule_expr(stream, result_peg_84.getPosition());
                if (result_peg_84.error()){
                    goto out_peg_88;
                }
                exp = result_peg_84.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("sparkno", exp);
                    result_peg_84.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_84;
        stream.update(result_peg_84.getPosition());
        
        
        return result_peg_84;
        out_peg_88:
        Result result_peg_127(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_129 = stream.getLineInfo(result_peg_127.getPosition());
                line = &line_info_peg_129;
            
            
            
            result_peg_127 = rule_identifier(stream, result_peg_127.getPosition());
                if (result_peg_127.error()){
                    goto out_peg_131;
                }
                name = result_peg_127.getValues();
            
            
            
            {
                    
                    result_peg_127.reset();
                    do{
                        Result result_peg_143(result_peg_127.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_143.getPosition()))){
                                    result_peg_143.nextPosition();
                                } else {
                                    goto out_peg_146;
                                }
                            }
                            result_peg_143.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_144;
                        out_peg_146:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_143.getPosition()))){
                                    result_peg_143.nextPosition();
                                } else {
                                    goto out_peg_148;
                                }
                            }
                            result_peg_143.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_144;
                        out_peg_148:
                        goto loop_peg_142;
                        success_peg_144:
                        ;
                        result_peg_127.addResult(result_peg_143);
                    } while (true);
                    loop_peg_142:
                    ;
                            
                }
                goto success_peg_133;
                goto out_peg_131;
                success_peg_133:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_127.getPosition()))){
                        result_peg_127.nextPosition();
                    } else {
                        goto out_peg_131;
                    }
                }
                result_peg_127.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_127.reset();
                    do{
                        Result result_peg_161(result_peg_127.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_161.getPosition()))){
                                    result_peg_161.nextPosition();
                                } else {
                                    goto out_peg_164;
                                }
                            }
                            result_peg_161.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_162;
                        out_peg_164:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_161.getPosition()))){
                                    result_peg_161.nextPosition();
                                } else {
                                    goto out_peg_166;
                                }
                            }
                            result_peg_161.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_162;
                        out_peg_166:
                        goto loop_peg_160;
                        success_peg_162:
                        ;
                        result_peg_127.addResult(result_peg_161);
                    } while (true);
                    loop_peg_160:
                    ;
                            
                }
                goto success_peg_151;
                goto out_peg_131;
                success_peg_151:
                ;
            
            
            
            result_peg_127 = rule_expr(stream, result_peg_127.getPosition());
                if (result_peg_127.error()){
                    goto out_peg_131;
                }
                exp = result_peg_127.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
                    result_peg_127.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_127;
        stream.update(result_peg_127.getPosition());
        
        
        return result_peg_127;
        out_peg_131:
        Result result_peg_168(myposition);
        
        {
        
            result_peg_168 = rule_identifier(stream, result_peg_168.getPosition());
                if (result_peg_168.error()){
                    goto out_peg_170;
                }
                name = result_peg_168.getValues();
            
            
            
            {
                    
                    result_peg_168.reset();
                    do{
                        Result result_peg_182(result_peg_168.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_182.getPosition()))){
                                    result_peg_182.nextPosition();
                                } else {
                                    goto out_peg_185;
                                }
                            }
                            result_peg_182.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_183;
                        out_peg_185:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_182.getPosition()))){
                                    result_peg_182.nextPosition();
                                } else {
                                    goto out_peg_187;
                                }
                            }
                            result_peg_182.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_183;
                        out_peg_187:
                        goto loop_peg_181;
                        success_peg_183:
                        ;
                        result_peg_168.addResult(result_peg_182);
                    } while (true);
                    loop_peg_181:
                    ;
                            
                }
                goto success_peg_172;
                goto out_peg_170;
                success_peg_172:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_168.getPosition()))){
                        result_peg_168.nextPosition();
                    } else {
                        goto out_peg_170;
                    }
                }
                result_peg_168.setValue(Value((void*) "!="));
            
            
            
            {
                    
                    result_peg_168.reset();
                    do{
                        Result result_peg_200(result_peg_168.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_200.getPosition()))){
                                    result_peg_200.nextPosition();
                                } else {
                                    goto out_peg_203;
                                }
                            }
                            result_peg_200.setValue(Value((void*) " "));
                                
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
                            result_peg_200.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_201;
                        out_peg_205:
                        goto loop_peg_199;
                        success_peg_201:
                        ;
                        result_peg_168.addResult(result_peg_200);
                    } while (true);
                    loop_peg_199:
                    ;
                            
                }
                goto success_peg_190;
                goto out_peg_170;
                success_peg_190:
                ;
            
            
            
            result_peg_168 = rule_expr(stream, result_peg_168.getPosition());
                if (result_peg_168.error()){
                    goto out_peg_170;
                }
                exp = result_peg_168.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name, Value(negateExpression(exp)));
                    result_peg_168.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_168;
        stream.update(result_peg_168.getPosition());
        
        
        return result_peg_168;
        out_peg_170:
        Result result_peg_207(myposition);
        
        {
        
            result_peg_207 = rule_identifier(stream, result_peg_207.getPosition());
                if (result_peg_207.error()){
                    goto out_peg_209;
                }
                name = result_peg_207.getValues();
            
            
            
            {
                    
                    result_peg_207.reset();
                    do{
                        Result result_peg_221(result_peg_207.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_221.getPosition()))){
                                    result_peg_221.nextPosition();
                                } else {
                                    goto out_peg_224;
                                }
                            }
                            result_peg_221.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_222;
                        out_peg_224:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_221.getPosition()))){
                                    result_peg_221.nextPosition();
                                } else {
                                    goto out_peg_226;
                                }
                            }
                            result_peg_221.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_222;
                        out_peg_226:
                        goto loop_peg_220;
                        success_peg_222:
                        ;
                        result_peg_207.addResult(result_peg_221);
                    } while (true);
                    loop_peg_220:
                    ;
                            
                }
                goto success_peg_211;
                goto out_peg_209;
                success_peg_211:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_207.getPosition()))){
                        result_peg_207.nextPosition();
                    } else {
                        goto out_peg_209;
                    }
                }
                result_peg_207.setValue(Value((void*) "="));
            
            
            
            result_peg_207 = rule_whitespace(stream, result_peg_207.getPosition());
                if (result_peg_207.error()){
                    goto out_peg_209;
                }
            
            
            
            Result result_peg_230(result_peg_207.getPosition());
                result_peg_230 = rule_line_end(stream, result_peg_230.getPosition());
                if (result_peg_230.error()){
                    goto out_peg_209;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name);
                    result_peg_207.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_207;
        stream.update(result_peg_207.getPosition());
        
        
        return result_peg_207;
        out_peg_209:
        Result result_peg_231(myposition);
        
        {
        
            result_peg_231 = rule_identifier(stream, result_peg_231.getPosition());
                if (result_peg_231.error()){
                    goto out_peg_233;
                }
                name = result_peg_231.getValues();
            
            
            
            {
                    
                    result_peg_231.reset();
                    do{
                        Result result_peg_245(result_peg_231.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_245.getPosition()))){
                                    result_peg_245.nextPosition();
                                } else {
                                    goto out_peg_248;
                                }
                            }
                            result_peg_245.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_246;
                        out_peg_248:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_245.getPosition()))){
                                    result_peg_245.nextPosition();
                                } else {
                                    goto out_peg_250;
                                }
                            }
                            result_peg_245.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_246;
                        out_peg_250:
                        goto loop_peg_244;
                        success_peg_246:
                        ;
                        result_peg_231.addResult(result_peg_245);
                    } while (true);
                    loop_peg_244:
                    ;
                            
                }
                goto success_peg_235;
                goto out_peg_233;
                success_peg_235:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_231.getPosition()))){
                        result_peg_231.nextPosition();
                    } else {
                        goto out_peg_233;
                    }
                }
                result_peg_231.setValue(Value((void*) "("));
            
            
            
            {
                    
                    result_peg_231.reset();
                    do{
                        Result result_peg_263(result_peg_231.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_263.getPosition()))){
                                    result_peg_263.nextPosition();
                                } else {
                                    goto out_peg_266;
                                }
                            }
                            result_peg_263.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_264;
                        out_peg_266:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_263.getPosition()))){
                                    result_peg_263.nextPosition();
                                } else {
                                    goto out_peg_268;
                                }
                            }
                            result_peg_263.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_264;
                        out_peg_268:
                        goto loop_peg_262;
                        success_peg_264:
                        ;
                        result_peg_231.addResult(result_peg_263);
                    } while (true);
                    loop_peg_262:
                    ;
                            
                }
                goto success_peg_253;
                goto out_peg_233;
                success_peg_253:
                ;
            
            
            
            result_peg_231 = rule_integer(stream, result_peg_231.getPosition());
                if (result_peg_231.error()){
                    goto out_peg_233;
                }
                index = result_peg_231.getValues();
            
            
            
            {
                    
                    result_peg_231.reset();
                    do{
                        Result result_peg_281(result_peg_231.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_281.getPosition()))){
                                    result_peg_281.nextPosition();
                                } else {
                                    goto out_peg_284;
                                }
                            }
                            result_peg_281.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_282;
                        out_peg_284:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_281.getPosition()))){
                                    result_peg_281.nextPosition();
                                } else {
                                    goto out_peg_286;
                                }
                            }
                            result_peg_281.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_282;
                        out_peg_286:
                        goto loop_peg_280;
                        success_peg_282:
                        ;
                        result_peg_231.addResult(result_peg_281);
                    } while (true);
                    loop_peg_280:
                    ;
                            
                }
                goto success_peg_271;
                goto out_peg_233;
                success_peg_271:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_231.getPosition()))){
                        result_peg_231.nextPosition();
                    } else {
                        goto out_peg_233;
                    }
                }
                result_peg_231.setValue(Value((void*) ")"));
            
            
            
            {
                    
                    result_peg_231.reset();
                    do{
                        Result result_peg_299(result_peg_231.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_299.getPosition()))){
                                    result_peg_299.nextPosition();
                                } else {
                                    goto out_peg_302;
                                }
                            }
                            result_peg_299.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_300;
                        out_peg_302:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_299.getPosition()))){
                                    result_peg_299.nextPosition();
                                } else {
                                    goto out_peg_304;
                                }
                            }
                            result_peg_299.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_300;
                        out_peg_304:
                        goto loop_peg_298;
                        success_peg_300:
                        ;
                        result_peg_231.addResult(result_peg_299);
                    } while (true);
                    loop_peg_298:
                    ;
                            
                }
                goto success_peg_289;
                goto out_peg_233;
                success_peg_289:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_231.getPosition()))){
                        result_peg_231.nextPosition();
                    } else {
                        goto out_peg_233;
                    }
                }
                result_peg_231.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_231.reset();
                    do{
                        Result result_peg_317(result_peg_231.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_317.getPosition()))){
                                    result_peg_317.nextPosition();
                                } else {
                                    goto out_peg_320;
                                }
                            }
                            result_peg_317.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_318;
                        out_peg_320:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_317.getPosition()))){
                                    result_peg_317.nextPosition();
                                } else {
                                    goto out_peg_322;
                                }
                            }
                            result_peg_317.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_318;
                        out_peg_322:
                        goto loop_peg_316;
                        success_peg_318:
                        ;
                        result_peg_231.addResult(result_peg_317);
                    } while (true);
                    loop_peg_316:
                    ;
                            
                }
                goto success_peg_307;
                goto out_peg_233;
                success_peg_307:
                ;
            
            
            
            result_peg_231 = rule_expr(stream, result_peg_231.getPosition());
                if (result_peg_231.error()){
                    goto out_peg_233;
                }
                exp = result_peg_231.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(name, index, exp);
                    result_peg_231.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_231;
        stream.update(result_peg_231.getPosition());
        
        
        return result_peg_231;
        out_peg_233:
        Result result_peg_324(myposition);
        
        {
        
            for (int i = 0; i < 4; i++){
                    if (compareCharCase("ctrl"[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
                result_peg_324.setValue(Value((void*) "ctrl"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(Value(makeSimpleIdentifier("ctrl")), Value(makeNumber(1)));
                    result_peg_324.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = result_peg_324;
        stream.update(result_peg_324.getPosition());
        
        
        return result_peg_324;
        out_peg_326:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_assignment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_identifier.calculated()){
        return column_peg_1.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_11(stream, "identifier");
    int myposition = position;
    
    
    Value line;
        Value first;
        Value rest;
    Result result_peg_2(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_4 = stream.getLineInfo(result_peg_2.getPosition());
                line = &line_info_peg_4;
            
            
            
            result_peg_2 = rule_name(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_6;
                }
                first = result_peg_2.getValues();
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_9(result_peg_2.getPosition());
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    goto loop_peg_8;
                                }
                            }
                            result_peg_9.setValue(Value((void*) "."));
                        
                        
                        
                        result_peg_9 = rule_name(stream, result_peg_9.getPosition());
                            if (result_peg_9.error()){
                                goto loop_peg_8;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                ;
                rest = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_identifier = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_6:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_integer.calculated()){
        return column_peg_1.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_19(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            int save_peg_4 = result_peg_2.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_7;
                        }
                    }
                    result_peg_2.setValue(Value((void*) "-"));
                        
                }
                goto success_peg_5;
                out_peg_7:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_9;
                        }
                    }
                    result_peg_2.setValue(Value((void*) "+"));
                        
                }
                goto success_peg_5;
                out_peg_9:
                
                result_peg_2 = Result(save_peg_4);
                result_peg_2.setValue(Value((void*) 0));
                
                success_peg_5:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_12(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_17 = stream.get(result_peg_12.getPosition());
                        if (letter_peg_17 != '\0' && strchr("0123456789", letter_peg_17) != NULL){
                            result_peg_12.nextPosition();
                            result_peg_12.setValue(Value((void*) (long) letter_peg_17));
                        } else {
                            goto out_peg_16;
                        }
                        
                    }
                    goto success_peg_13;
                    out_peg_16:
                    goto loop_peg_11;
                    success_peg_13:
                    ;
                    result_peg_2.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                if (result_peg_2.matches() == 0){
                    goto out_peg_18;
                }
            
            Result result_peg_10 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeInteger(result_peg_3.getValues(), result_peg_10.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_integer = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_18:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_float.calculated()){
        return column_peg_1.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_46(stream, "float");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_2(myposition);
        
        {
        
            int save_peg_4 = result_peg_2.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_7;
                        }
                    }
                    result_peg_2.setValue(Value((void*) "-"));
                        
                }
                goto success_peg_5;
                out_peg_7:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_9;
                        }
                    }
                    result_peg_2.setValue(Value((void*) "+"));
                        
                }
                goto success_peg_5;
                out_peg_9:
                
                result_peg_2 = Result(save_peg_4);
                result_peg_2.setValue(Value((void*) 0));
                
                success_peg_5:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_12(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_17 = stream.get(result_peg_12.getPosition());
                        if (letter_peg_17 != '\0' && strchr("0123456789", letter_peg_17) != NULL){
                            result_peg_12.nextPosition();
                            result_peg_12.setValue(Value((void*) (long) letter_peg_17));
                        } else {
                            goto out_peg_16;
                        }
                        
                    }
                    goto success_peg_13;
                    out_peg_16:
                    goto loop_peg_11;
                    success_peg_13:
                    ;
                    result_peg_2.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                ;
                left = result_peg_2.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_19;
                    }
                }
                result_peg_2.setValue(Value((void*) "."));
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_22(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_27 = stream.get(result_peg_22.getPosition());
                        if (letter_peg_27 != '\0' && strchr("0123456789", letter_peg_27) != NULL){
                            result_peg_22.nextPosition();
                            result_peg_22.setValue(Value((void*) (long) letter_peg_27));
                        } else {
                            goto out_peg_26;
                        }
                        
                    }
                    goto success_peg_23;
                    out_peg_26:
                    goto loop_peg_21;
                    success_peg_23:
                    ;
                    result_peg_2.addResult(result_peg_22);
                } while (true);
                loop_peg_21:
                if (result_peg_2.matches() == 0){
                    goto out_peg_19;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_3.getValues(), Value(parseDouble(left,right)));
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_float = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_19:
        Result result_peg_28(myposition);
        
        {
        
            int save_peg_30 = result_peg_28.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_28.getPosition()))){
                            result_peg_28.nextPosition();
                        } else {
                            goto out_peg_33;
                        }
                    }
                    result_peg_28.setValue(Value((void*) "-"));
                        
                }
                goto success_peg_31;
                out_peg_33:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_28.getPosition()))){
                            result_peg_28.nextPosition();
                        } else {
                            goto out_peg_35;
                        }
                    }
                    result_peg_28.setValue(Value((void*) "+"));
                        
                }
                goto success_peg_31;
                out_peg_35:
                
                result_peg_28 = Result(save_peg_30);
                result_peg_28.setValue(Value((void*) 0));
                
                success_peg_31:
                ;
            
            Result result_peg_29 = result_peg_28;
            
            result_peg_28.reset();
                do{
                    Result result_peg_38(result_peg_28.getPosition());
                    {
                        
                        char letter_peg_43 = stream.get(result_peg_38.getPosition());
                        if (letter_peg_43 != '\0' && strchr("0123456789", letter_peg_43) != NULL){
                            result_peg_38.nextPosition();
                            result_peg_38.setValue(Value((void*) (long) letter_peg_43));
                        } else {
                            goto out_peg_42;
                        }
                        
                    }
                    goto success_peg_39;
                    out_peg_42:
                    goto loop_peg_37;
                    success_peg_39:
                    ;
                    result_peg_28.addResult(result_peg_38);
                } while (true);
                loop_peg_37:
                if (result_peg_28.matches() == 0){
                    goto out_peg_44;
                }
                left = result_peg_28.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_44;
                    }
                }
                result_peg_28.setValue(Value((void*) "."));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_29.getValues(), Value(parseDouble(left)));
                    result_peg_28.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_float = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_44:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_float = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_string.calculated()){
        return column_peg_1.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_16(stream, "string");
    int myposition = position;
    
    
    Value contents;
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "\""));
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_10(result_peg_7);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\""[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    goto not_peg_9;
                                }
                            }
                            result_peg_10.setValue(Value((void*) "\""));
                            goto loop_peg_6;
                            not_peg_9:
                            result_peg_7.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_13(result_peg_7);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_13.getPosition()))){
                                    result_peg_13.nextPosition();
                                } else {
                                    goto not_peg_12;
                                }
                            }
                            result_peg_13.setValue(Value((void*) "\n"));
                            goto loop_peg_6;
                            not_peg_12:
                            result_peg_7.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_14 = stream.get(result_peg_7.getPosition());
                            if (temp_peg_14 != '\0'){
                                result_peg_7.setValue(Value((void*) (long) temp_peg_14));
                                result_peg_7.nextPosition();
                            } else {
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
                contents = result_peg_2.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "\""));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeString(contents);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_string = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_string = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_range.calculated()){
        return column_peg_1.chunk2->chunk_range;
    }
    
    RuleTrace trace_peg_302(stream, "range");
    int myposition = position;
    
    
    Value low;
        Value high;
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "["));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            result_peg_2 = rule_expr_c(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                low = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_4;
                success_peg_24:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_52(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_52.getPosition()))){
                                    result_peg_52.nextPosition();
                                } else {
                                    goto out_peg_55;
                                }
                            }
                            result_peg_52.setValue(Value((void*) " "));
                                
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
                            result_peg_52.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_53;
                        out_peg_57:
                        goto loop_peg_51;
                        success_peg_53:
                        ;
                        result_peg_2.addResult(result_peg_52);
                    } while (true);
                    loop_peg_51:
                    ;
                            
                }
                goto success_peg_42;
                goto out_peg_4;
                success_peg_42:
                ;
            
            
            
            result_peg_2 = rule_expr_c(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                high = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_70(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_70.getPosition()))){
                                    result_peg_70.nextPosition();
                                } else {
                                    goto out_peg_73;
                                }
                            }
                            result_peg_70.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_71;
                        out_peg_73:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_70.getPosition()))){
                                    result_peg_70.nextPosition();
                                } else {
                                    goto out_peg_75;
                                }
                            }
                            result_peg_70.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_71;
                        out_peg_75:
                        goto loop_peg_69;
                        success_peg_71:
                        ;
                        result_peg_2.addResult(result_peg_70);
                    } while (true);
                    loop_peg_69:
                    ;
                            
                }
                goto success_peg_60;
                goto out_peg_4;
                success_peg_60:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "]"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllInclusive, low, high);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_range = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_77(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_77.getPosition()))){
                        result_peg_77.nextPosition();
                    } else {
                        goto out_peg_79;
                    }
                }
                result_peg_77.setValue(Value((void*) "("));
            
            
            
            {
                    
                    result_peg_77.reset();
                    do{
                        Result result_peg_91(result_peg_77.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_91.getPosition()))){
                                    result_peg_91.nextPosition();
                                } else {
                                    goto out_peg_94;
                                }
                            }
                            result_peg_91.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_92;
                        out_peg_94:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_91.getPosition()))){
                                    result_peg_91.nextPosition();
                                } else {
                                    goto out_peg_96;
                                }
                            }
                            result_peg_91.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_92;
                        out_peg_96:
                        goto loop_peg_90;
                        success_peg_92:
                        ;
                        result_peg_77.addResult(result_peg_91);
                    } while (true);
                    loop_peg_90:
                    ;
                            
                }
                goto success_peg_81;
                goto out_peg_79;
                success_peg_81:
                ;
            
            
            
            result_peg_77 = rule_expr_c(stream, result_peg_77.getPosition());
                if (result_peg_77.error()){
                    goto out_peg_79;
                }
                low = result_peg_77.getValues();
            
            
            
            {
                    
                    result_peg_77.reset();
                    do{
                        Result result_peg_109(result_peg_77.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_109.getPosition()))){
                                    result_peg_109.nextPosition();
                                } else {
                                    goto out_peg_112;
                                }
                            }
                            result_peg_109.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_110;
                        out_peg_112:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_109.getPosition()))){
                                    result_peg_109.nextPosition();
                                } else {
                                    goto out_peg_114;
                                }
                            }
                            result_peg_109.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_110;
                        out_peg_114:
                        goto loop_peg_108;
                        success_peg_110:
                        ;
                        result_peg_77.addResult(result_peg_109);
                    } while (true);
                    loop_peg_108:
                    ;
                            
                }
                goto success_peg_99;
                goto out_peg_79;
                success_peg_99:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_77.getPosition()))){
                        result_peg_77.nextPosition();
                    } else {
                        goto out_peg_79;
                    }
                }
                result_peg_77.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_77.reset();
                    do{
                        Result result_peg_127(result_peg_77.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_127.getPosition()))){
                                    result_peg_127.nextPosition();
                                } else {
                                    goto out_peg_130;
                                }
                            }
                            result_peg_127.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_128;
                        out_peg_130:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_127.getPosition()))){
                                    result_peg_127.nextPosition();
                                } else {
                                    goto out_peg_132;
                                }
                            }
                            result_peg_127.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_128;
                        out_peg_132:
                        goto loop_peg_126;
                        success_peg_128:
                        ;
                        result_peg_77.addResult(result_peg_127);
                    } while (true);
                    loop_peg_126:
                    ;
                            
                }
                goto success_peg_117;
                goto out_peg_79;
                success_peg_117:
                ;
            
            
            
            result_peg_77 = rule_expr_c(stream, result_peg_77.getPosition());
                if (result_peg_77.error()){
                    goto out_peg_79;
                }
                high = result_peg_77.getValues();
            
            
            
            {
                    
                    result_peg_77.reset();
                    do{
                        Result result_peg_145(result_peg_77.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_145.getPosition()))){
                                    result_peg_145.nextPosition();
                                } else {
                                    goto out_peg_148;
                                }
                            }
                            result_peg_145.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_146;
                        out_peg_148:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_145.getPosition()))){
                                    result_peg_145.nextPosition();
                                } else {
                                    goto out_peg_150;
                                }
                            }
                            result_peg_145.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_146;
                        out_peg_150:
                        goto loop_peg_144;
                        success_peg_146:
                        ;
                        result_peg_77.addResult(result_peg_145);
                    } while (true);
                    loop_peg_144:
                    ;
                            
                }
                goto success_peg_135;
                goto out_peg_79;
                success_peg_135:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_77.getPosition()))){
                        result_peg_77.nextPosition();
                    } else {
                        goto out_peg_79;
                    }
                }
                result_peg_77.setValue(Value((void*) ")"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllExclusive, low, high);
                    result_peg_77.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_range = result_peg_77;
        stream.update(result_peg_77.getPosition());
        
        
        return result_peg_77;
        out_peg_79:
        Result result_peg_152(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_152.getPosition()))){
                        result_peg_152.nextPosition();
                    } else {
                        goto out_peg_154;
                    }
                }
                result_peg_152.setValue(Value((void*) "("));
            
            
            
            {
                    
                    result_peg_152.reset();
                    do{
                        Result result_peg_166(result_peg_152.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_166.getPosition()))){
                                    result_peg_166.nextPosition();
                                } else {
                                    goto out_peg_169;
                                }
                            }
                            result_peg_166.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_167;
                        out_peg_169:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_166.getPosition()))){
                                    result_peg_166.nextPosition();
                                } else {
                                    goto out_peg_171;
                                }
                            }
                            result_peg_166.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_167;
                        out_peg_171:
                        goto loop_peg_165;
                        success_peg_167:
                        ;
                        result_peg_152.addResult(result_peg_166);
                    } while (true);
                    loop_peg_165:
                    ;
                            
                }
                goto success_peg_156;
                goto out_peg_154;
                success_peg_156:
                ;
            
            
            
            result_peg_152 = rule_expr_c(stream, result_peg_152.getPosition());
                if (result_peg_152.error()){
                    goto out_peg_154;
                }
                low = result_peg_152.getValues();
            
            
            
            {
                    
                    result_peg_152.reset();
                    do{
                        Result result_peg_184(result_peg_152.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_184.getPosition()))){
                                    result_peg_184.nextPosition();
                                } else {
                                    goto out_peg_187;
                                }
                            }
                            result_peg_184.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_185;
                        out_peg_187:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_184.getPosition()))){
                                    result_peg_184.nextPosition();
                                } else {
                                    goto out_peg_189;
                                }
                            }
                            result_peg_184.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_185;
                        out_peg_189:
                        goto loop_peg_183;
                        success_peg_185:
                        ;
                        result_peg_152.addResult(result_peg_184);
                    } while (true);
                    loop_peg_183:
                    ;
                            
                }
                goto success_peg_174;
                goto out_peg_154;
                success_peg_174:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_152.getPosition()))){
                        result_peg_152.nextPosition();
                    } else {
                        goto out_peg_154;
                    }
                }
                result_peg_152.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_152.reset();
                    do{
                        Result result_peg_202(result_peg_152.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_202.getPosition()))){
                                    result_peg_202.nextPosition();
                                } else {
                                    goto out_peg_205;
                                }
                            }
                            result_peg_202.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_203;
                        out_peg_205:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_202.getPosition()))){
                                    result_peg_202.nextPosition();
                                } else {
                                    goto out_peg_207;
                                }
                            }
                            result_peg_202.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_203;
                        out_peg_207:
                        goto loop_peg_201;
                        success_peg_203:
                        ;
                        result_peg_152.addResult(result_peg_202);
                    } while (true);
                    loop_peg_201:
                    ;
                            
                }
                goto success_peg_192;
                goto out_peg_154;
                success_peg_192:
                ;
            
            
            
            result_peg_152 = rule_expr_c(stream, result_peg_152.getPosition());
                if (result_peg_152.error()){
                    goto out_peg_154;
                }
                high = result_peg_152.getValues();
            
            
            
            {
                    
                    result_peg_152.reset();
                    do{
                        Result result_peg_220(result_peg_152.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    goto out_peg_223;
                                }
                            }
                            result_peg_220.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_221;
                        out_peg_223:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    goto out_peg_225;
                                }
                            }
                            result_peg_220.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_221;
                        out_peg_225:
                        goto loop_peg_219;
                        success_peg_221:
                        ;
                        result_peg_152.addResult(result_peg_220);
                    } while (true);
                    loop_peg_219:
                    ;
                            
                }
                goto success_peg_210;
                goto out_peg_154;
                success_peg_210:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_152.getPosition()))){
                        result_peg_152.nextPosition();
                    } else {
                        goto out_peg_154;
                    }
                }
                result_peg_152.setValue(Value((void*) "]"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                    result_peg_152.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_range = result_peg_152;
        stream.update(result_peg_152.getPosition());
        
        
        return result_peg_152;
        out_peg_154:
        Result result_peg_227(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_227.getPosition()))){
                        result_peg_227.nextPosition();
                    } else {
                        goto out_peg_229;
                    }
                }
                result_peg_227.setValue(Value((void*) "["));
            
            
            
            {
                    
                    result_peg_227.reset();
                    do{
                        Result result_peg_241(result_peg_227.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_241.getPosition()))){
                                    result_peg_241.nextPosition();
                                } else {
                                    goto out_peg_244;
                                }
                            }
                            result_peg_241.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_242;
                        out_peg_244:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_241.getPosition()))){
                                    result_peg_241.nextPosition();
                                } else {
                                    goto out_peg_246;
                                }
                            }
                            result_peg_241.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_242;
                        out_peg_246:
                        goto loop_peg_240;
                        success_peg_242:
                        ;
                        result_peg_227.addResult(result_peg_241);
                    } while (true);
                    loop_peg_240:
                    ;
                            
                }
                goto success_peg_231;
                goto out_peg_229;
                success_peg_231:
                ;
            
            
            
            result_peg_227 = rule_expr_c(stream, result_peg_227.getPosition());
                if (result_peg_227.error()){
                    goto out_peg_229;
                }
                low = result_peg_227.getValues();
            
            
            
            {
                    
                    result_peg_227.reset();
                    do{
                        Result result_peg_259(result_peg_227.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_259.getPosition()))){
                                    result_peg_259.nextPosition();
                                } else {
                                    goto out_peg_262;
                                }
                            }
                            result_peg_259.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_260;
                        out_peg_262:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_259.getPosition()))){
                                    result_peg_259.nextPosition();
                                } else {
                                    goto out_peg_264;
                                }
                            }
                            result_peg_259.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_260;
                        out_peg_264:
                        goto loop_peg_258;
                        success_peg_260:
                        ;
                        result_peg_227.addResult(result_peg_259);
                    } while (true);
                    loop_peg_258:
                    ;
                            
                }
                goto success_peg_249;
                goto out_peg_229;
                success_peg_249:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_227.getPosition()))){
                        result_peg_227.nextPosition();
                    } else {
                        goto out_peg_229;
                    }
                }
                result_peg_227.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_227.reset();
                    do{
                        Result result_peg_277(result_peg_227.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_277.getPosition()))){
                                    result_peg_277.nextPosition();
                                } else {
                                    goto out_peg_280;
                                }
                            }
                            result_peg_277.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_278;
                        out_peg_280:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_277.getPosition()))){
                                    result_peg_277.nextPosition();
                                } else {
                                    goto out_peg_282;
                                }
                            }
                            result_peg_277.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_278;
                        out_peg_282:
                        goto loop_peg_276;
                        success_peg_278:
                        ;
                        result_peg_227.addResult(result_peg_277);
                    } while (true);
                    loop_peg_276:
                    ;
                            
                }
                goto success_peg_267;
                goto out_peg_229;
                success_peg_267:
                ;
            
            
            
            result_peg_227 = rule_expr_c(stream, result_peg_227.getPosition());
                if (result_peg_227.error()){
                    goto out_peg_229;
                }
                high = result_peg_227.getValues();
            
            
            
            {
                    
                    result_peg_227.reset();
                    do{
                        Result result_peg_295(result_peg_227.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_295.getPosition()))){
                                    result_peg_295.nextPosition();
                                } else {
                                    goto out_peg_298;
                                }
                            }
                            result_peg_295.setValue(Value((void*) " "));
                                
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
                            result_peg_295.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_296;
                        out_peg_300:
                        goto loop_peg_294;
                        success_peg_296:
                        ;
                        result_peg_227.addResult(result_peg_295);
                    } while (true);
                    loop_peg_294:
                    ;
                            
                }
                goto success_peg_285;
                goto out_peg_229;
                success_peg_285:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_227.getPosition()))){
                        result_peg_227.nextPosition();
                    } else {
                        goto out_peg_229;
                    }
                }
                result_peg_227.setValue(Value((void*) ")"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                    result_peg_227.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_range = result_peg_227;
        stream.update(result_peg_227.getPosition());
        
        
        return result_peg_227;
        out_peg_229:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_range = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk3 != 0 && column_peg_1.chunk3->chunk_name.calculated()){
        return column_peg_1.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_13(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    char letter_peg_8 = stream.get(result_peg_2.getPosition());
                    if (letter_peg_8 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_8) != NULL){
                        result_peg_2.nextPosition();
                        result_peg_2.setValue(Value((void*) (long) letter_peg_8));
                    } else {
                        goto out_peg_7;
                    }
                    
                }
                goto success_peg_4;
                out_peg_7:
                goto out_peg_9;
                success_peg_4:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_12(result_peg_2.getPosition());
                    result_peg_12 = rule_alpha_digit(stream, result_peg_12.getPosition());
                    if (result_peg_12.error()){
                        goto loop_peg_11;
                    }
                    result_peg_2.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                ;
            
            Result result_peg_10 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_3.getValues().getValue(),result_peg_10.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_name = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_9:
    
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk3 != 0 && column_peg_1.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_1.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_16(stream, "alpha_digit");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
            
            char letter_peg_7 = stream.get(result_peg_2.getPosition());
            if (letter_peg_7 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_7) != NULL){
                result_peg_2.nextPosition();
                result_peg_2.setValue(Value((void*) (long) letter_peg_7));
            } else {
                goto out_peg_6;
            }
            
        }
        goto success_peg_3;
        out_peg_6:
        goto out_peg_8;
        success_peg_3:
        ;
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_alpha_digit = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_8:
        Result result_peg_9(myposition);
        
        {
            
            char letter_peg_14 = stream.get(result_peg_9.getPosition());
            if (letter_peg_14 != '\0' && strchr("0123456789", letter_peg_14) != NULL){
                result_peg_9.nextPosition();
                result_peg_9.setValue(Value((void*) (long) letter_peg_14));
            } else {
                goto out_peg_13;
            }
            
        }
        goto success_peg_10;
        out_peg_13:
        goto out_peg_15;
        success_peg_10:
        ;
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_alpha_digit = result_peg_9;
        stream.update(result_peg_9.getPosition());
        
        
        return result_peg_9;
        out_peg_15:
    
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_alpha_digit = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk3 != 0 && column_peg_1.chunk3->chunk_valuelist.calculated()){
        return column_peg_1.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_119(stream, "valuelist");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr_c(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                first = result_peg_2.getValues();
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                    
                        {
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_19(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_22;
                                            }
                                        }
                                        result_peg_19.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_22:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_24;
                                            }
                                        }
                                        result_peg_19.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_24:
                                    goto loop_peg_18;
                                    success_peg_20:
                                    ;
                                    result_peg_7.addResult(result_peg_19);
                                } while (true);
                                loop_peg_18:
                                ;
                                        
                            }
                            goto success_peg_9;
                            goto loop_peg_6;
                            success_peg_9:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    goto loop_peg_6;
                                }
                            }
                            result_peg_7.setValue(Value((void*) ","));
                        
                        
                        
                        {
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_37(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_40;
                                            }
                                        }
                                        result_peg_37.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_40:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_42;
                                            }
                                        }
                                        result_peg_37.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_42:
                                    goto loop_peg_36;
                                    success_peg_38:
                                    ;
                                    result_peg_7.addResult(result_peg_37);
                                } while (true);
                                loop_peg_36:
                                ;
                                        
                            }
                            goto success_peg_27;
                            goto loop_peg_6;
                            success_peg_27:
                            ;
                        
                        
                        
                        result_peg_7 = rule_expr_c(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
                rest = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_valuelist = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_43(myposition);
        
        {
        
            {
                    
                    result_peg_43.reset();
                    do{
                        Result result_peg_55(result_peg_43.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    goto out_peg_58;
                                }
                            }
                            result_peg_55.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_56;
                        out_peg_58:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    goto out_peg_60;
                                }
                            }
                            result_peg_55.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_56;
                        out_peg_60:
                        goto loop_peg_54;
                        success_peg_56:
                        ;
                        result_peg_43.addResult(result_peg_55);
                    } while (true);
                    loop_peg_54:
                    ;
                            
                }
                goto success_peg_45;
                goto out_peg_61;
                success_peg_45:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_43.getPosition()))){
                        result_peg_43.nextPosition();
                    } else {
                        goto out_peg_61;
                    }
                }
                result_peg_43.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_43.reset();
                    do{
                        Result result_peg_74(result_peg_43.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    goto out_peg_77;
                                }
                            }
                            result_peg_74.setValue(Value((void*) " "));
                                
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
                            result_peg_74.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_75;
                        out_peg_79:
                        goto loop_peg_73;
                        success_peg_75:
                        ;
                        result_peg_43.addResult(result_peg_74);
                    } while (true);
                    loop_peg_73:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_61;
                success_peg_64:
                ;
            
            
            
            result_peg_43 = rule_expr_c(stream, result_peg_43.getPosition());
                if (result_peg_43.error()){
                    goto out_peg_61;
                }
                first = result_peg_43.getValues();
            
            
            
            result_peg_43.reset();
                do{
                    Result result_peg_83(result_peg_43.getPosition());
                    {
                    
                        {
                                
                                result_peg_83.reset();
                                do{
                                    Result result_peg_95(result_peg_83.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_95.getPosition()))){
                                                result_peg_95.nextPosition();
                                            } else {
                                                goto out_peg_98;
                                            }
                                        }
                                        result_peg_95.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_96;
                                    out_peg_98:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_95.getPosition()))){
                                                result_peg_95.nextPosition();
                                            } else {
                                                goto out_peg_100;
                                            }
                                        }
                                        result_peg_95.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_96;
                                    out_peg_100:
                                    goto loop_peg_94;
                                    success_peg_96:
                                    ;
                                    result_peg_83.addResult(result_peg_95);
                                } while (true);
                                loop_peg_94:
                                ;
                                        
                            }
                            goto success_peg_85;
                            goto loop_peg_82;
                            success_peg_85:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_83.getPosition()))){
                                    result_peg_83.nextPosition();
                                } else {
                                    goto loop_peg_82;
                                }
                            }
                            result_peg_83.setValue(Value((void*) ","));
                        
                        
                        
                        {
                                
                                result_peg_83.reset();
                                do{
                                    Result result_peg_113(result_peg_83.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_113.getPosition()))){
                                                result_peg_113.nextPosition();
                                            } else {
                                                goto out_peg_116;
                                            }
                                        }
                                        result_peg_113.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_114;
                                    out_peg_116:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_113.getPosition()))){
                                                result_peg_113.nextPosition();
                                            } else {
                                                goto out_peg_118;
                                            }
                                        }
                                        result_peg_113.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_114;
                                    out_peg_118:
                                    goto loop_peg_112;
                                    success_peg_114:
                                    ;
                                    result_peg_83.addResult(result_peg_113);
                                } while (true);
                                loop_peg_112:
                                ;
                                        
                            }
                            goto success_peg_103;
                            goto loop_peg_82;
                            success_peg_103:
                            ;
                        
                        
                        
                        result_peg_83 = rule_expr_c(stream, result_peg_83.getPosition());
                            if (result_peg_83.error()){
                                goto loop_peg_82;
                            }
                        
                        
                    }
                    result_peg_43.addResult(result_peg_83);
                } while (true);
                loop_peg_82:
                ;
                rest = result_peg_43.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_43.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_valuelist = result_peg_43;
        stream.update(result_peg_43.getPosition());
        
        
        return result_peg_43;
        out_peg_61:
    
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk3 != 0 && column_peg_1.chunk3->chunk_expr.calculated()){
        return column_peg_1.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_28(stream, "expr");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr_c(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            Result result_peg_7(result_peg_2);
                {
                
                    {
                            
                            result_peg_7.reset();
                            do{
                                Result result_peg_19(result_peg_7.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                            result_peg_19.nextPosition();
                                        } else {
                                            goto out_peg_22;
                                        }
                                    }
                                    result_peg_19.setValue(Value((void*) " "));
                                        
                                }
                                goto success_peg_20;
                                out_peg_22:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                            result_peg_19.nextPosition();
                                        } else {
                                            goto out_peg_24;
                                        }
                                    }
                                    result_peg_19.setValue(Value((void*) "\t"));
                                        
                                }
                                goto success_peg_20;
                                out_peg_24:
                                goto loop_peg_18;
                                success_peg_20:
                                ;
                                result_peg_7.addResult(result_peg_19);
                            } while (true);
                            loop_peg_18:
                            ;
                                    
                        }
                        goto success_peg_9;
                        goto not_peg_6;
                        success_peg_9:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_7.getPosition()))){
                                result_peg_7.nextPosition();
                            } else {
                                goto not_peg_6;
                            }
                        }
                        result_peg_7.setValue(Value((void*) ","));
                    
                    
                }
                goto out_peg_4;
                not_peg_6:
                result_peg_2.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_3.getValues();
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_expr = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_25(myposition);
        
        {
        
            result_peg_25 = rule_valuelist(stream, result_peg_25.getPosition());
                if (result_peg_25.error()){
                    goto out_peg_27;
                }
            
            Result result_peg_26 = result_peg_25;
            
            result_peg_25 = rule_expr2_rest(stream, result_peg_25.getPosition(), result_peg_26.getValues());
                if (result_peg_25.error()){
                    goto out_peg_27;
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_expr = result_peg_25;
        stream.update(result_peg_25.getPosition());
        
        
        return result_peg_25;
        out_peg_27:
    
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_expr = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk3 != 0 && column_peg_1.chunk3->chunk_expr_c.calculated()){
        return column_peg_1.chunk3->chunk_expr_c;
    }
    
    RuleTrace trace_peg_5(stream, "expr_c");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr2(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_expr_c = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_expr_c = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk4 != 0 && column_peg_1.chunk4->chunk_expr2.calculated()){
        return column_peg_1.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_5(stream, "expr2");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr3(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr2_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr2 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr2 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk4 != 0 && column_peg_1.chunk4->chunk_expr3.calculated()){
        return column_peg_1.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_5(stream, "expr3");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr4(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr3_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr3 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr3 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk4 != 0 && column_peg_1.chunk4->chunk_expr4.calculated()){
        return column_peg_1.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_5(stream, "expr4");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr5(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr4_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr4 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr4 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk4 != 0 && column_peg_1.chunk4->chunk_expr5.calculated()){
        return column_peg_1.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_5(stream, "expr5");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr6(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr5_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr5 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr5 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk4 != 0 && column_peg_1.chunk4->chunk_expr6.calculated()){
        return column_peg_1.chunk4->chunk_expr6;
    }
    
    RuleTrace trace_peg_5(stream, "expr6");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr7(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr6_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr6 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk4 == 0){
            column_peg_1.chunk4 = new Chunk4();
        }
        column_peg_1.chunk4->chunk_expr6 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk5 != 0 && column_peg_1.chunk5->chunk_expr7.calculated()){
        return column_peg_1.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_5(stream, "expr7");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr8(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr7_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr7 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr7 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk5 != 0 && column_peg_1.chunk5->chunk_expr8.calculated()){
        return column_peg_1.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_5(stream, "expr8");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr9(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr8_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr8 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr8 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk5 != 0 && column_peg_1.chunk5->chunk_expr9.calculated()){
        return column_peg_1.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_5(stream, "expr9");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr10(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr9_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr9 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr9 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk5 != 0 && column_peg_1.chunk5->chunk_expr10.calculated()){
        return column_peg_1.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_5(stream, "expr10");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr11(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr10_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr10 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr10 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk5 != 0 && column_peg_1.chunk5->chunk_expr11.calculated()){
        return column_peg_1.chunk5->chunk_expr11;
    }
    
    RuleTrace trace_peg_5(stream, "expr11");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr12(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr11_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr11 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk5 == 0){
            column_peg_1.chunk5 = new Chunk5();
        }
        column_peg_1.chunk5->chunk_expr11 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk6 != 0 && column_peg_1.chunk6->chunk_expr12.calculated()){
        return column_peg_1.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_5(stream, "expr12");
    int myposition = position;
    
    
    Value left;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr13(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                left = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_expr12_rest(stream, result_peg_2.getPosition(), left);
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
        }
        
        if (column_peg_1.chunk6 == 0){
            column_peg_1.chunk6 = new Chunk6();
        }
        column_peg_1.chunk6->chunk_expr12 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk6 == 0){
            column_peg_1.chunk6 = new Chunk6();
        }
        column_peg_1.chunk6->chunk_expr12 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk6 != 0 && column_peg_1.chunk6->chunk_expr13.calculated()){
        return column_peg_1.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_25(stream, "expr13");
    int myposition = position;
    
    
    Value exp;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    result_peg_5 = rule_unary(stream, result_peg_5.getPosition());
                    if (result_peg_5.error()){
                        goto loop_peg_4;
                    }
                    result_peg_2.addResult(result_peg_5);
                } while (true);
                loop_peg_4:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_17(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_17.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_18;
                        out_peg_20:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    goto out_peg_22;
                                }
                            }
                            result_peg_17.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_18;
                        out_peg_22:
                        goto loop_peg_16;
                        success_peg_18:
                        ;
                        result_peg_2.addResult(result_peg_17);
                    } while (true);
                    loop_peg_16:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_23;
                success_peg_7:
                ;
            
            
            
            result_peg_2 = rule_expr13_real(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_23;
                }
                exp = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeUnaryExpression(result_peg_3.getValues(), exp);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk6 == 0){
            column_peg_1.chunk6 = new Chunk6();
        }
        column_peg_1.chunk6->chunk_expr13 = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_23:
    
        if (column_peg_1.chunk6 == 0){
            column_peg_1.chunk6 = new Chunk6();
        }
        column_peg_1.chunk6->chunk_expr13 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("||"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "||"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr2(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionOr(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr2_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("^^"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "^^"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr3(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionXOr(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr3_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("&&"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "&&"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr4(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAnd(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr4_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("|"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "|"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr5(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseOr(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr5_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("^"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "^"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr6(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseXOr(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr6_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("&"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "&"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr7(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseAnd(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr7_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar(":="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) ":="));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr8(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAssignment(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_83(stream, "expr8_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr9(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionEquals(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
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
                            result_peg_54.setValue(Value((void*) " "));
                                
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
                            result_peg_54.setValue(Value((void*) "\t"));
                                
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
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_42.getPosition()))){
                        result_peg_42.nextPosition();
                    } else {
                        goto out_peg_60;
                    }
                }
                result_peg_42.setValue(Value((void*) "!="));
            
            
            
            {
                    
                    result_peg_42.reset();
                    do{
                        Result result_peg_73(result_peg_42.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    goto out_peg_76;
                                }
                            }
                            result_peg_73.setValue(Value((void*) " "));
                                
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
                            result_peg_73.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_74;
                        out_peg_78:
                        goto loop_peg_72;
                        success_peg_74:
                        ;
                        result_peg_42.addResult(result_peg_73);
                    } while (true);
                    loop_peg_72:
                    ;
                            
                }
                goto success_peg_63;
                goto out_peg_60;
                success_peg_63:
                ;
            
            
            
            result_peg_42 = rule_expr9(stream, result_peg_42.getPosition());
                if (result_peg_42.error()){
                    goto out_peg_60;
                }
                right = result_peg_42.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionUnequals(left, right);
                    result_peg_42.setValue(value);
                }
                new_left = result_peg_42.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_42.getPosition();
        goto tail_peg_3;
        out_peg_60:
        Result result_peg_81(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_81.setValue(value);
                }
            
            
        }
        stream.update(result_peg_81.getPosition());
        
        return result_peg_81;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr9_rest");
    int myposition = position;
    
    tail_peg_3:
    Value maker;
        Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            result_peg_2 = rule_compare(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                maker = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr10(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    {
                            typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                            value = as<compare_func>(maker)(left, right);
                            }
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk8 != 0 && column_peg_1.chunk8->chunk_compare.calculated()){
        return column_peg_1.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_14(stream, "compare");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar("<="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "<="));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThanEquals;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_compare = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_5(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar(">="[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
                result_peg_5.setValue(Value((void*) ">="));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThanEquals;
                    result_peg_5.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_compare = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_7:
        Result result_peg_8(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("<"[i], stream.get(result_peg_8.getPosition()))){
                        result_peg_8.nextPosition();
                    } else {
                        goto out_peg_10;
                    }
                }
                result_peg_8.setValue(Value((void*) "<"));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThan;
                    result_peg_8.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_compare = result_peg_8;
        stream.update(result_peg_8.getPosition());
        
        
        return result_peg_8;
        out_peg_10:
        Result result_peg_11(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_11.getPosition()))){
                        result_peg_11.nextPosition();
                    } else {
                        goto out_peg_13;
                    }
                }
                result_peg_11.setValue(Value((void*) ">"));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThan;
                    result_peg_11.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_compare = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_13:
    
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk8 != 0 && column_peg_1.chunk8->chunk_all_compare.calculated()){
        return column_peg_1.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_6(stream, "all_compare");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2 = rule_compare(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_all_compare = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        result_peg_4 = rule_compare_equal(stream, result_peg_4.getPosition());
        if (result_peg_4.error()){
            goto out_peg_5;
        }
        
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_all_compare = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
    
        if (column_peg_1.chunk8 == 0){
            column_peg_1.chunk8 = new Chunk8();
        }
        column_peg_1.chunk8->chunk_all_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_83(stream, "expr10_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "+"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr11(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAdd(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
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
                            result_peg_54.setValue(Value((void*) " "));
                                
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
                            result_peg_54.setValue(Value((void*) "\t"));
                                
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
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_42.getPosition()))){
                        result_peg_42.nextPosition();
                    } else {
                        goto out_peg_60;
                    }
                }
                result_peg_42.setValue(Value((void*) "-"));
            
            
            
            {
                    
                    result_peg_42.reset();
                    do{
                        Result result_peg_73(result_peg_42.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    goto out_peg_76;
                                }
                            }
                            result_peg_73.setValue(Value((void*) " "));
                                
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
                            result_peg_73.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_74;
                        out_peg_78:
                        goto loop_peg_72;
                        success_peg_74:
                        ;
                        result_peg_42.addResult(result_peg_73);
                    } while (true);
                    loop_peg_72:
                    ;
                            
                }
                goto success_peg_63;
                goto out_peg_60;
                success_peg_63:
                ;
            
            
            
            result_peg_42 = rule_expr11(stream, result_peg_42.getPosition());
                if (result_peg_42.error()){
                    goto out_peg_60;
                }
                right = result_peg_42.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionSubtract(left, right);
                    result_peg_42.setValue(value);
                }
                new_left = result_peg_42.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_42.getPosition();
        goto tail_peg_3;
        out_peg_60:
        Result result_peg_81(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_81.setValue(value);
                }
            
            
        }
        stream.update(result_peg_81.getPosition());
        
        return result_peg_81;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_122(stream, "expr11_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("*"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "*"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr12(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionMultiply(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
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
                            result_peg_54.setValue(Value((void*) " "));
                                
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
                            result_peg_54.setValue(Value((void*) "\t"));
                                
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
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_42.getPosition()))){
                        result_peg_42.nextPosition();
                    } else {
                        goto out_peg_60;
                    }
                }
                result_peg_42.setValue(Value((void*) "/"));
            
            
            
            {
                    
                    result_peg_42.reset();
                    do{
                        Result result_peg_73(result_peg_42.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    goto out_peg_76;
                                }
                            }
                            result_peg_73.setValue(Value((void*) " "));
                                
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
                            result_peg_73.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_74;
                        out_peg_78:
                        goto loop_peg_72;
                        success_peg_74:
                        ;
                        result_peg_42.addResult(result_peg_73);
                    } while (true);
                    loop_peg_72:
                    ;
                            
                }
                goto success_peg_63;
                goto out_peg_60;
                success_peg_63:
                ;
            
            
            
            result_peg_42 = rule_expr12(stream, result_peg_42.getPosition());
                if (result_peg_42.error()){
                    goto out_peg_60;
                }
                right = result_peg_42.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionDivide(left, right);
                    result_peg_42.setValue(value);
                }
                new_left = result_peg_42.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_42.getPosition();
        goto tail_peg_3;
        out_peg_60:
        Result result_peg_81(myposition);
        {
        
            {
                    
                    result_peg_81.reset();
                    do{
                        Result result_peg_93(result_peg_81.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_93.getPosition()))){
                                    result_peg_93.nextPosition();
                                } else {
                                    goto out_peg_96;
                                }
                            }
                            result_peg_93.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_94;
                        out_peg_96:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_93.getPosition()))){
                                    result_peg_93.nextPosition();
                                } else {
                                    goto out_peg_98;
                                }
                            }
                            result_peg_93.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_94;
                        out_peg_98:
                        goto loop_peg_92;
                        success_peg_94:
                        ;
                        result_peg_81.addResult(result_peg_93);
                    } while (true);
                    loop_peg_92:
                    ;
                            
                }
                goto success_peg_83;
                goto out_peg_99;
                success_peg_83:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("%"[i], stream.get(result_peg_81.getPosition()))){
                        result_peg_81.nextPosition();
                    } else {
                        goto out_peg_99;
                    }
                }
                result_peg_81.setValue(Value((void*) "%"));
            
            
            
            {
                    
                    result_peg_81.reset();
                    do{
                        Result result_peg_112(result_peg_81.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_112.getPosition()))){
                                    result_peg_112.nextPosition();
                                } else {
                                    goto out_peg_115;
                                }
                            }
                            result_peg_112.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_113;
                        out_peg_115:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_112.getPosition()))){
                                    result_peg_112.nextPosition();
                                } else {
                                    goto out_peg_117;
                                }
                            }
                            result_peg_112.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_113;
                        out_peg_117:
                        goto loop_peg_111;
                        success_peg_113:
                        ;
                        result_peg_81.addResult(result_peg_112);
                    } while (true);
                    loop_peg_111:
                    ;
                            
                }
                goto success_peg_102;
                goto out_peg_99;
                success_peg_102:
                ;
            
            
            
            result_peg_81 = rule_expr12(stream, result_peg_81.getPosition());
                if (result_peg_81.error()){
                    goto out_peg_99;
                }
                right = result_peg_81.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionModulo(left, right);
                    result_peg_81.setValue(value);
                }
                new_left = result_peg_81.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_81.getPosition();
        goto tail_peg_3;
        out_peg_99:
        Result result_peg_120(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_120.setValue(value);
                }
            
            
        }
        stream.update(result_peg_120.getPosition());
        
        return result_peg_120;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "expr12_rest");
    int myposition = position;
    
    tail_peg_3:
    Value right;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("**"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "**"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_expr13(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionPower(left, right);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk9 != 0 && column_peg_1.chunk9->chunk_unary.calculated()){
        return column_peg_1.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_19(stream, "unary");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("!"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "!"));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Not;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_unary = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_5(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
                result_peg_5.setValue(Value((void*) "-"));
            
            
            
            Result result_peg_10(result_peg_5);
                {
                    
                    char letter_peg_15 = stream.get(result_peg_10.getPosition());
                    if (letter_peg_15 != '\0' && strchr("0123456789", letter_peg_15) != NULL){
                        result_peg_10.nextPosition();
                        result_peg_10.setValue(Value((void*) (long) letter_peg_15));
                    } else {
                        goto out_peg_14;
                    }
                    
                }
                goto success_peg_11;
                out_peg_14:
                goto not_peg_9;
                success_peg_11:
                ;
                goto out_peg_7;
                not_peg_9:
                result_peg_5.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Minus;
                    result_peg_5.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_unary = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_7:
        Result result_peg_16(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_16.getPosition()))){
                        result_peg_16.nextPosition();
                    } else {
                        goto out_peg_18;
                    }
                }
                result_peg_16.setValue(Value((void*) "~"));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Negation;
                    result_peg_16.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_unary = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_18:
    
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_unary = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk9 != 0 && column_peg_1.chunk9->chunk_expr13_real.calculated()){
        return column_peg_1.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_47(stream, "expr13_real");
    int myposition = position;
    
    
    Value e;
    Result result_peg_2(myposition);
        
        result_peg_2 = rule_helper(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_expr13_real = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        result_peg_4 = rule_function(stream, result_peg_4.getPosition());
        if (result_peg_4.error()){
            goto out_peg_5;
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_expr13_real = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        result_peg_6 = rule_value(stream, result_peg_6.getPosition());
        if (result_peg_6.error()){
            goto out_peg_7;
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_expr13_real = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
        Result result_peg_8(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_8.getPosition()))){
                        result_peg_8.nextPosition();
                    } else {
                        goto out_peg_10;
                    }
                }
                result_peg_8.setValue(Value((void*) "("));
            
            
            
            {
                    
                    result_peg_8.reset();
                    do{
                        Result result_peg_22(result_peg_8.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    goto out_peg_25;
                                }
                            }
                            result_peg_22.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_23;
                        out_peg_25:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    goto out_peg_27;
                                }
                            }
                            result_peg_22.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_23;
                        out_peg_27:
                        goto loop_peg_21;
                        success_peg_23:
                        ;
                        result_peg_8.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
                    ;
                            
                }
                goto success_peg_12;
                goto out_peg_10;
                success_peg_12:
                ;
            
            
            
            result_peg_8 = rule_expr(stream, result_peg_8.getPosition());
                if (result_peg_8.error()){
                    goto out_peg_10;
                }
                e = result_peg_8.getValues();
            
            
            
            {
                    
                    result_peg_8.reset();
                    do{
                        Result result_peg_40(result_peg_8.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_43;
                                }
                            }
                            result_peg_40.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_41;
                        out_peg_43:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_45;
                                }
                            }
                            result_peg_40.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_41;
                        out_peg_45:
                        goto loop_peg_39;
                        success_peg_41:
                        ;
                        result_peg_8.addResult(result_peg_40);
                    } while (true);
                    loop_peg_39:
                    ;
                            
                }
                goto success_peg_30;
                goto out_peg_10;
                success_peg_30:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_8.getPosition()))){
                        result_peg_8.nextPosition();
                    } else {
                        goto out_peg_10;
                    }
                }
                result_peg_8.setValue(Value((void*) ")"));
            
            
            
            {
                    Value value((void*) 0);
                    value = e;
                    result_peg_8.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_expr13_real = result_peg_8;
        stream.update(result_peg_8.getPosition());
        
        
        return result_peg_8;
        out_peg_10:
    
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_expr13_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk9 != 0 && column_peg_1.chunk9->chunk_function.calculated()){
        return column_peg_1.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_390(stream, "function");
    int myposition = position;
    
    
    Value name;
        Value args;
        Value arg1;
        Value line;
        Value comparison;
        Value arg2;
        Value id;
        Value state;
        Value attributes;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_function_name(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                name = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "("));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_4;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_args(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                args = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_52(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_52.getPosition()))){
                                    result_peg_52.nextPosition();
                                } else {
                                    goto out_peg_55;
                                }
                            }
                            result_peg_52.setValue(Value((void*) " "));
                                
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
                            result_peg_52.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_53;
                        out_peg_57:
                        goto loop_peg_51;
                        success_peg_53:
                        ;
                        result_peg_2.addResult(result_peg_52);
                    } while (true);
                    loop_peg_51:
                    ;
                            
                }
                goto success_peg_42;
                goto out_peg_4;
                success_peg_42:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) ")"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction(name, args);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_59(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_59.getPosition()))){
                        result_peg_59.nextPosition();
                    } else {
                        goto out_peg_61;
                    }
                }
                result_peg_59.setValue(Value((void*) "teammode"));
                name = result_peg_59.getValues();
            
            
            
            {
                    
                    result_peg_59.reset();
                    do{
                        Result result_peg_73(result_peg_59.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    goto out_peg_76;
                                }
                            }
                            result_peg_73.setValue(Value((void*) " "));
                                
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
                            result_peg_73.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_74;
                        out_peg_78:
                        goto loop_peg_72;
                        success_peg_74:
                        ;
                        result_peg_59.addResult(result_peg_73);
                    } while (true);
                    loop_peg_72:
                    ;
                            
                }
                goto success_peg_63;
                goto out_peg_61;
                success_peg_63:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_59.getPosition()))){
                        result_peg_59.nextPosition();
                    } else {
                        goto out_peg_61;
                    }
                }
                result_peg_59.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_59.reset();
                    do{
                        Result result_peg_91(result_peg_59.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_91.getPosition()))){
                                    result_peg_91.nextPosition();
                                } else {
                                    goto out_peg_94;
                                }
                            }
                            result_peg_91.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_92;
                        out_peg_94:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_91.getPosition()))){
                                    result_peg_91.nextPosition();
                                } else {
                                    goto out_peg_96;
                                }
                            }
                            result_peg_91.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_92;
                        out_peg_96:
                        goto loop_peg_90;
                        success_peg_92:
                        ;
                        result_peg_59.addResult(result_peg_91);
                    } while (true);
                    loop_peg_90:
                    ;
                            
                }
                goto success_peg_81;
                goto out_peg_61;
                success_peg_81:
                ;
            
            
            
            result_peg_59 = rule_identifier(stream, result_peg_59.getPosition());
                if (result_peg_59.error()){
                    goto out_peg_61;
                }
                arg1 = result_peg_59.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(name, arg1);
                    result_peg_59.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = result_peg_59;
        stream.update(result_peg_59.getPosition());
        
        
        return result_peg_59;
        out_peg_61:
        Result result_peg_98(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_98.getPosition()))){
                        result_peg_98.nextPosition();
                    } else {
                        goto out_peg_100;
                    }
                }
                result_peg_98.setValue(Value((void*) "teammode"));
                name = result_peg_98.getValues();
            
            
            
            {
                    
                    result_peg_98.reset();
                    do{
                        Result result_peg_112(result_peg_98.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_112.getPosition()))){
                                    result_peg_112.nextPosition();
                                } else {
                                    goto out_peg_115;
                                }
                            }
                            result_peg_112.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_113;
                        out_peg_115:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_112.getPosition()))){
                                    result_peg_112.nextPosition();
                                } else {
                                    goto out_peg_117;
                                }
                            }
                            result_peg_112.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_113;
                        out_peg_117:
                        goto loop_peg_111;
                        success_peg_113:
                        ;
                        result_peg_98.addResult(result_peg_112);
                    } while (true);
                    loop_peg_111:
                    ;
                            
                }
                goto success_peg_102;
                goto out_peg_100;
                success_peg_102:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_98.getPosition()))){
                        result_peg_98.nextPosition();
                    } else {
                        goto out_peg_100;
                    }
                }
                result_peg_98.setValue(Value((void*) "!="));
            
            
            
            {
                    
                    result_peg_98.reset();
                    do{
                        Result result_peg_130(result_peg_98.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_130.getPosition()))){
                                    result_peg_130.nextPosition();
                                } else {
                                    goto out_peg_133;
                                }
                            }
                            result_peg_130.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_131;
                        out_peg_133:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_130.getPosition()))){
                                    result_peg_130.nextPosition();
                                } else {
                                    goto out_peg_135;
                                }
                            }
                            result_peg_130.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_131;
                        out_peg_135:
                        goto loop_peg_129;
                        success_peg_131:
                        ;
                        result_peg_98.addResult(result_peg_130);
                    } while (true);
                    loop_peg_129:
                    ;
                            
                }
                goto success_peg_120;
                goto out_peg_100;
                success_peg_120:
                ;
            
            
            
            result_peg_98 = rule_identifier(stream, result_peg_98.getPosition());
                if (result_peg_98.error()){
                    goto out_peg_100;
                }
                arg1 = result_peg_98.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(std::string("teammode!="), arg1);
                    result_peg_98.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = result_peg_98;
        stream.update(result_peg_98.getPosition());
        
        
        return result_peg_98;
        out_peg_100:
        Result result_peg_137(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_139 = stream.getLineInfo(result_peg_137.getPosition());
                line = &line_info_peg_139;
            
            
            
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_137.getPosition()))){
                        result_peg_137.nextPosition();
                    } else {
                        goto out_peg_141;
                    }
                }
                result_peg_137.setValue(Value((void*) "animelem"));
            
            
            
            {
                    
                    result_peg_137.reset();
                    do{
                        Result result_peg_153(result_peg_137.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_153.getPosition()))){
                                    result_peg_153.nextPosition();
                                } else {
                                    goto out_peg_156;
                                }
                            }
                            result_peg_153.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_154;
                        out_peg_156:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_153.getPosition()))){
                                    result_peg_153.nextPosition();
                                } else {
                                    goto out_peg_158;
                                }
                            }
                            result_peg_153.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_154;
                        out_peg_158:
                        goto loop_peg_152;
                        success_peg_154:
                        ;
                        result_peg_137.addResult(result_peg_153);
                    } while (true);
                    loop_peg_152:
                    ;
                            
                }
                goto success_peg_143;
                goto out_peg_141;
                success_peg_143:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_137.getPosition()))){
                        result_peg_137.nextPosition();
                    } else {
                        goto out_peg_141;
                    }
                }
                result_peg_137.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_137.reset();
                    do{
                        Result result_peg_171(result_peg_137.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_171.getPosition()))){
                                    result_peg_171.nextPosition();
                                } else {
                                    goto out_peg_174;
                                }
                            }
                            result_peg_171.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_172;
                        out_peg_174:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_171.getPosition()))){
                                    result_peg_171.nextPosition();
                                } else {
                                    goto out_peg_176;
                                }
                            }
                            result_peg_171.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_172;
                        out_peg_176:
                        goto loop_peg_170;
                        success_peg_172:
                        ;
                        result_peg_137.addResult(result_peg_171);
                    } while (true);
                    loop_peg_170:
                    ;
                            
                }
                goto success_peg_161;
                goto out_peg_141;
                success_peg_161:
                ;
            
            
            
            result_peg_137 = rule_integer(stream, result_peg_137.getPosition());
                if (result_peg_137.error()){
                    goto out_peg_141;
                }
                arg1 = result_peg_137.getValues();
            
            
            
            {
                    
                    result_peg_137.reset();
                    do{
                        Result result_peg_189(result_peg_137.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_189.getPosition()))){
                                    result_peg_189.nextPosition();
                                } else {
                                    goto out_peg_192;
                                }
                            }
                            result_peg_189.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_190;
                        out_peg_192:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_189.getPosition()))){
                                    result_peg_189.nextPosition();
                                } else {
                                    goto out_peg_194;
                                }
                            }
                            result_peg_189.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_190;
                        out_peg_194:
                        goto loop_peg_188;
                        success_peg_190:
                        ;
                        result_peg_137.addResult(result_peg_189);
                    } while (true);
                    loop_peg_188:
                    ;
                            
                }
                goto success_peg_179;
                goto out_peg_141;
                success_peg_179:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_137.getPosition()))){
                        result_peg_137.nextPosition();
                    } else {
                        goto out_peg_141;
                    }
                }
                result_peg_137.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_137.reset();
                    do{
                        Result result_peg_207(result_peg_137.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_207.getPosition()))){
                                    result_peg_207.nextPosition();
                                } else {
                                    goto out_peg_210;
                                }
                            }
                            result_peg_207.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_208;
                        out_peg_210:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_207.getPosition()))){
                                    result_peg_207.nextPosition();
                                } else {
                                    goto out_peg_212;
                                }
                            }
                            result_peg_207.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_208;
                        out_peg_212:
                        goto loop_peg_206;
                        success_peg_208:
                        ;
                        result_peg_137.addResult(result_peg_207);
                    } while (true);
                    loop_peg_206:
                    ;
                            
                }
                goto success_peg_197;
                goto out_peg_141;
                success_peg_197:
                ;
            
            
            
            result_peg_137 = rule_all_compare(stream, result_peg_137.getPosition());
                if (result_peg_137.error()){
                    goto out_peg_141;
                }
                comparison = result_peg_137.getValues();
            
            
            
            {
                    
                    result_peg_137.reset();
                    do{
                        Result result_peg_225(result_peg_137.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_225.getPosition()))){
                                    result_peg_225.nextPosition();
                                } else {
                                    goto out_peg_228;
                                }
                            }
                            result_peg_225.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_226;
                        out_peg_228:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_225.getPosition()))){
                                    result_peg_225.nextPosition();
                                } else {
                                    goto out_peg_230;
                                }
                            }
                            result_peg_225.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_226;
                        out_peg_230:
                        goto loop_peg_224;
                        success_peg_226:
                        ;
                        result_peg_137.addResult(result_peg_225);
                    } while (true);
                    loop_peg_224:
                    ;
                            
                }
                goto success_peg_215;
                goto out_peg_141;
                success_peg_215:
                ;
            
            
            
            result_peg_137 = rule_integer(stream, result_peg_137.getPosition());
                if (result_peg_137.error()){
                    goto out_peg_141;
                }
                arg2 = result_peg_137.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAnimElem(line, arg1, comparison, arg2);
                    result_peg_137.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = result_peg_137;
        stream.update(result_peg_137.getPosition());
        
        
        return result_peg_137;
        out_peg_141:
        Result result_peg_232(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_234 = stream.getLineInfo(result_peg_232.getPosition());
                line = &line_info_peg_234;
            
            
            
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_232.getPosition()))){
                        result_peg_232.nextPosition();
                    } else {
                        goto out_peg_236;
                    }
                }
                result_peg_232.setValue(Value((void*) "animelem"));
            
            
            
            {
                    
                    result_peg_232.reset();
                    do{
                        Result result_peg_248(result_peg_232.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_248.getPosition()))){
                                    result_peg_248.nextPosition();
                                } else {
                                    goto out_peg_251;
                                }
                            }
                            result_peg_248.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_249;
                        out_peg_251:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_248.getPosition()))){
                                    result_peg_248.nextPosition();
                                } else {
                                    goto out_peg_253;
                                }
                            }
                            result_peg_248.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_249;
                        out_peg_253:
                        goto loop_peg_247;
                        success_peg_249:
                        ;
                        result_peg_232.addResult(result_peg_248);
                    } while (true);
                    loop_peg_247:
                    ;
                            
                }
                goto success_peg_238;
                goto out_peg_236;
                success_peg_238:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_232.getPosition()))){
                        result_peg_232.nextPosition();
                    } else {
                        goto out_peg_236;
                    }
                }
                result_peg_232.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_232.reset();
                    do{
                        Result result_peg_266(result_peg_232.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_266.getPosition()))){
                                    result_peg_266.nextPosition();
                                } else {
                                    goto out_peg_269;
                                }
                            }
                            result_peg_266.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_267;
                        out_peg_269:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_266.getPosition()))){
                                    result_peg_266.nextPosition();
                                } else {
                                    goto out_peg_271;
                                }
                            }
                            result_peg_266.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_267;
                        out_peg_271:
                        goto loop_peg_265;
                        success_peg_267:
                        ;
                        result_peg_232.addResult(result_peg_266);
                    } while (true);
                    loop_peg_265:
                    ;
                            
                }
                goto success_peg_256;
                goto out_peg_236;
                success_peg_256:
                ;
            
            
            
            result_peg_232 = rule_integer(stream, result_peg_232.getPosition());
                if (result_peg_232.error()){
                    goto out_peg_236;
                }
                arg1 = result_peg_232.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAnimElem(line, arg1);
                    result_peg_232.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = result_peg_232;
        stream.update(result_peg_232.getPosition());
        
        
        return result_peg_232;
        out_peg_236:
        Result result_peg_273(myposition);
        
        {
        
            for (int i = 0; i < 11; i++){
                    if (compareCharCase("projcontact"[i], stream.get(result_peg_273.getPosition()))){
                        result_peg_273.nextPosition();
                    } else {
                        goto out_peg_275;
                    }
                }
                result_peg_273.setValue(Value((void*) "projcontact"));
                name = result_peg_273.getValues();
            
            
            
            int save_peg_277 = result_peg_273.getPosition();
                
                result_peg_273 = rule_integer(stream, result_peg_273.getPosition());
                if (result_peg_273.error()){
                    
                    result_peg_273 = Result(save_peg_277);
                    result_peg_273.setValue(Value((void*) 0));
                    
                }
                id = result_peg_273.getValues();
            
            
            
            {
                    
                    result_peg_273.reset();
                    do{
                        Result result_peg_289(result_peg_273.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_289.getPosition()))){
                                    result_peg_289.nextPosition();
                                } else {
                                    goto out_peg_292;
                                }
                            }
                            result_peg_289.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_290;
                        out_peg_292:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_289.getPosition()))){
                                    result_peg_289.nextPosition();
                                } else {
                                    goto out_peg_294;
                                }
                            }
                            result_peg_289.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_290;
                        out_peg_294:
                        goto loop_peg_288;
                        success_peg_290:
                        ;
                        result_peg_273.addResult(result_peg_289);
                    } while (true);
                    loop_peg_288:
                    ;
                            
                }
                goto success_peg_279;
                goto out_peg_275;
                success_peg_279:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_273.getPosition()))){
                        result_peg_273.nextPosition();
                    } else {
                        goto out_peg_275;
                    }
                }
                result_peg_273.setValue(Value((void*) "="));
            
            
            
            {
                    
                    result_peg_273.reset();
                    do{
                        Result result_peg_307(result_peg_273.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_307.getPosition()))){
                                    result_peg_307.nextPosition();
                                } else {
                                    goto out_peg_310;
                                }
                            }
                            result_peg_307.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_308;
                        out_peg_310:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_307.getPosition()))){
                                    result_peg_307.nextPosition();
                                } else {
                                    goto out_peg_312;
                                }
                            }
                            result_peg_307.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_308;
                        out_peg_312:
                        goto loop_peg_306;
                        success_peg_308:
                        ;
                        result_peg_273.addResult(result_peg_307);
                    } while (true);
                    loop_peg_306:
                    ;
                            
                }
                goto success_peg_297;
                goto out_peg_275;
                success_peg_297:
                ;
            
            
            
            result_peg_273 = rule_integer(stream, result_peg_273.getPosition());
                if (result_peg_273.error()){
                    goto out_peg_275;
                }
                arg1 = result_peg_273.getValues();
            
            
            
            result_peg_273 = rule_function_rest(stream, result_peg_273.getPosition());
                if (result_peg_273.error()){
                    goto out_peg_275;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    std::string name = combine("projcontact", id);
                                    value = makeFunction1(name, arg1);
                    result_peg_273.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = result_peg_273;
        stream.update(result_peg_273.getPosition());
        
        
        return result_peg_273;
        out_peg_275:
        Result result_peg_315(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("hitdefattr"[i], stream.get(result_peg_315.getPosition()))){
                        result_peg_315.nextPosition();
                    } else {
                        goto out_peg_317;
                    }
                }
                result_peg_315.setValue(Value((void*) "hitdefattr"));
            
            
            
            {
                    
                    result_peg_315.reset();
                    do{
                        Result result_peg_329(result_peg_315.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_329.getPosition()))){
                                    result_peg_329.nextPosition();
                                } else {
                                    goto out_peg_332;
                                }
                            }
                            result_peg_329.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_330;
                        out_peg_332:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_329.getPosition()))){
                                    result_peg_329.nextPosition();
                                } else {
                                    goto out_peg_334;
                                }
                            }
                            result_peg_329.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_330;
                        out_peg_334:
                        goto loop_peg_328;
                        success_peg_330:
                        ;
                        result_peg_315.addResult(result_peg_329);
                    } while (true);
                    loop_peg_328:
                    ;
                            
                }
                goto success_peg_319;
                goto out_peg_317;
                success_peg_319:
                ;
            
            
            
            result_peg_315 = rule_compare_equal(stream, result_peg_315.getPosition());
                if (result_peg_315.error()){
                    goto out_peg_317;
                }
                comparison = result_peg_315.getValues();
            
            
            
            {
                    
                    result_peg_315.reset();
                    do{
                        Result result_peg_347(result_peg_315.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_347.getPosition()))){
                                    result_peg_347.nextPosition();
                                } else {
                                    goto out_peg_350;
                                }
                            }
                            result_peg_347.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_348;
                        out_peg_350:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_347.getPosition()))){
                                    result_peg_347.nextPosition();
                                } else {
                                    goto out_peg_352;
                                }
                            }
                            result_peg_347.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_348;
                        out_peg_352:
                        goto loop_peg_346;
                        success_peg_348:
                        ;
                        result_peg_315.addResult(result_peg_347);
                    } while (true);
                    loop_peg_346:
                    ;
                            
                }
                goto success_peg_337;
                goto out_peg_317;
                success_peg_337:
                ;
            
            
            
            result_peg_315 = rule_hitdef__attribute(stream, result_peg_315.getPosition());
                if (result_peg_315.error()){
                    goto out_peg_317;
                }
                state = result_peg_315.getValues();
            
            
            
            {
                    
                    result_peg_315.reset();
                    do{
                        Result result_peg_365(result_peg_315.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_365.getPosition()))){
                                    result_peg_365.nextPosition();
                                } else {
                                    goto out_peg_368;
                                }
                            }
                            result_peg_365.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_366;
                        out_peg_368:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_365.getPosition()))){
                                    result_peg_365.nextPosition();
                                } else {
                                    goto out_peg_370;
                                }
                            }
                            result_peg_365.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_366;
                        out_peg_370:
                        goto loop_peg_364;
                        success_peg_366:
                        ;
                        result_peg_315.addResult(result_peg_365);
                    } while (true);
                    loop_peg_364:
                    ;
                            
                }
                goto success_peg_355;
                goto out_peg_317;
                success_peg_355:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_315.getPosition()))){
                        result_peg_315.nextPosition();
                    } else {
                        goto out_peg_317;
                    }
                }
                result_peg_315.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_315.reset();
                    do{
                        Result result_peg_383(result_peg_315.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_383.getPosition()))){
                                    result_peg_383.nextPosition();
                                } else {
                                    goto out_peg_386;
                                }
                            }
                            result_peg_383.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_384;
                        out_peg_386:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_383.getPosition()))){
                                    result_peg_383.nextPosition();
                                } else {
                                    goto out_peg_388;
                                }
                            }
                            result_peg_383.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_384;
                        out_peg_388:
                        goto loop_peg_382;
                        success_peg_384:
                        ;
                        result_peg_315.addResult(result_peg_383);
                    } while (true);
                    loop_peg_382:
                    ;
                            
                }
                goto success_peg_373;
                goto out_peg_317;
                success_peg_373:
                ;
            
            
            
            result_peg_315 = rule_hitdef__attack__attribute(stream, result_peg_315.getPosition());
                if (result_peg_315.error()){
                    goto out_peg_317;
                }
                attributes = result_peg_315.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                                     Ast::Value * compare_state = as<compare_func>(comparison)(Value(makeSimpleIdentifier("hitdefattr:state")), state);
                                     Ast::Value * compare_attribute = as<compare_func>(comparison)(Value(makeSimpleIdentifier("hitdefattr:attribute")), attributes);
                                     value = makeExpressionAnd(Value(compare_state), Value(compare_attribute));
                    result_peg_315.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = result_peg_315;
        stream.update(result_peg_315.getPosition());
        
        
        return result_peg_315;
        out_peg_317:
    
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_function = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_compare_equal(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk9 != 0 && column_peg_1.chunk9->chunk_compare_equal.calculated()){
        return column_peg_1.chunk9->chunk_compare_equal;
    }
    
    RuleTrace trace_peg_8(stream, "compare_equal");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "="));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) makeExpressionEquals;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_compare_equal = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_5(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
                result_peg_5.setValue(Value((void*) "!="));
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) makeExpressionUnequals;
                    result_peg_5.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_compare_equal = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_7:
    
        if (column_peg_1.chunk9 == 0){
            column_peg_1.chunk9 = new Chunk9();
        }
        column_peg_1.chunk9->chunk_compare_equal = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk10 != 0 && column_peg_1.chunk10->chunk_hitdef__attack__attribute.calculated()){
        return column_peg_1.chunk10->chunk_hitdef__attack__attribute;
    }
    
    RuleTrace trace_peg_44(stream, "hitdef__attack__attribute");
    int myposition = position;
    
    
    Value attribute;
    Result result_peg_2(myposition);
        
        {
        
            {
                    Value value((void*) 0);
                    value = makeHitDefAttackAttribute();
                    result_peg_2.setValue(value);
                }
                attribute = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_hitdef__attack__attribute__item(stream, result_peg_2.getPosition(), attribute);
                if (result_peg_2.error()){
                    goto out_peg_5;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_8(result_peg_2.getPosition());
                    {
                    
                        {
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_20(result_peg_8.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_20.getPosition()))){
                                                result_peg_20.nextPosition();
                                            } else {
                                                goto out_peg_23;
                                            }
                                        }
                                        result_peg_20.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_21;
                                    out_peg_23:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_20.getPosition()))){
                                                result_peg_20.nextPosition();
                                            } else {
                                                goto out_peg_25;
                                            }
                                        }
                                        result_peg_20.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_21;
                                    out_peg_25:
                                    goto loop_peg_19;
                                    success_peg_21:
                                    ;
                                    result_peg_8.addResult(result_peg_20);
                                } while (true);
                                loop_peg_19:
                                ;
                                        
                            }
                            goto success_peg_10;
                            goto loop_peg_7;
                            success_peg_10:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    goto loop_peg_7;
                                }
                            }
                            result_peg_8.setValue(Value((void*) ","));
                        
                        
                        
                        {
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_38(result_peg_8.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_38.getPosition()))){
                                                result_peg_38.nextPosition();
                                            } else {
                                                goto out_peg_41;
                                            }
                                        }
                                        result_peg_38.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_39;
                                    out_peg_41:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_38.getPosition()))){
                                                result_peg_38.nextPosition();
                                            } else {
                                                goto out_peg_43;
                                            }
                                        }
                                        result_peg_38.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_39;
                                    out_peg_43:
                                    goto loop_peg_37;
                                    success_peg_39:
                                    ;
                                    result_peg_8.addResult(result_peg_38);
                                } while (true);
                                loop_peg_37:
                                ;
                                        
                            }
                            goto success_peg_28;
                            goto loop_peg_7;
                            success_peg_28:
                            ;
                        
                        
                        
                        result_peg_8 = rule_hitdef__attack__attribute__item(stream, result_peg_8.getPosition(), attribute);
                            if (result_peg_8.error()){
                                goto loop_peg_7;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::HitDefAttackAttribute*>(attribute)->reset(); value = attribute;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__attribute = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_5:
    
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute__item(Stream & stream, const int position, Value attribute){
    
    RuleTrace trace_peg_6(stream, "hitdef__attack__attribute__item");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_hitdef__attack__type(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2 = rule_hitdef__attack__movement(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_5 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    std::string * result = toString(as<const char *>(result_peg_3.getValues()));
                            std::string * other = toString(as<const char *>(result_peg_5.getValues()));
                            as<Ast::HitDefAttackAttribute*>(attribute)->addAttribute(*result + *other);
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_hitdef__attack__type(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk10 != 0 && column_peg_1.chunk10->chunk_hitdef__attack__type.calculated()){
        return column_peg_1.chunk10->chunk_hitdef__attack__type;
    }
    
    RuleTrace trace_peg_8(stream, "hitdef__attack__type");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("n"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        result_peg_2.setValue(Value((void*) "n"));
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__type = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("s"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        result_peg_4.setValue(Value((void*) "s"));
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__type = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("h"[i], stream.get(result_peg_6.getPosition()))){
                result_peg_6.nextPosition();
            } else {
                goto out_peg_7;
            }
        }
        result_peg_6.setValue(Value((void*) "h"));
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__type = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
    
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__type = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__movement(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk10 != 0 && column_peg_1.chunk10->chunk_hitdef__attack__movement.calculated()){
        return column_peg_1.chunk10->chunk_hitdef__attack__movement;
    }
    
    RuleTrace trace_peg_8(stream, "hitdef__attack__movement");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("a"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        result_peg_2.setValue(Value((void*) "a"));
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__movement = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("t"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        result_peg_4.setValue(Value((void*) "t"));
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__movement = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("p"[i], stream.get(result_peg_6.getPosition()))){
                result_peg_6.nextPosition();
            } else {
                goto out_peg_7;
            }
        }
        result_peg_6.setValue(Value((void*) "p"));
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__movement = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
    
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attack__movement = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attribute(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk10 != 0 && column_peg_1.chunk10->chunk_hitdef__attribute.calculated()){
        return column_peg_1.chunk10->chunk_hitdef__attribute;
    }
    
    RuleTrace trace_peg_7(stream, "hitdef__attribute");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    result_peg_5 = rule_hitdef__attribute__item(stream, result_peg_5.getPosition());
                    if (result_peg_5.error()){
                        goto loop_peg_4;
                    }
                    result_peg_2.addResult(result_peg_5);
                } while (true);
                loop_peg_4:
                if (result_peg_2.matches() == 0){
                    goto out_peg_6;
                }
            
            Result result_peg_3 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeHitDefAttribute(result_peg_3.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attribute = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_6:
    
        if (column_peg_1.chunk10 == 0){
            column_peg_1.chunk10 = new Chunk10();
        }
        column_peg_1.chunk10->chunk_hitdef__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attribute__item(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk11 != 0 && column_peg_1.chunk11->chunk_hitdef__attribute__item.calculated()){
        return column_peg_1.chunk11->chunk_hitdef__attribute__item;
    }
    
    RuleTrace trace_peg_8(stream, "hitdef__attribute__item");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("s"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        result_peg_2.setValue(Value((void*) "s"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_hitdef__attribute__item = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("c"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        result_peg_4.setValue(Value((void*) "c"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_hitdef__attribute__item = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("a"[i], stream.get(result_peg_6.getPosition()))){
                result_peg_6.nextPosition();
            } else {
                goto out_peg_7;
            }
        }
        result_peg_6.setValue(Value((void*) "a"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_hitdef__attribute__item = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
    
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_hitdef__attribute__item = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk11 != 0 && column_peg_1.chunk11->chunk_args.calculated()){
        return column_peg_1.chunk11->chunk_args;
    }
    
    RuleTrace trace_peg_43(stream, "args");
    int myposition = position;
    
    
    Value expr1;
        Value expr_rest;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_expr_c(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                expr1 = result_peg_2.getValues();
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                    
                        {
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_19(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_22;
                                            }
                                        }
                                        result_peg_19.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_22:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_24;
                                            }
                                        }
                                        result_peg_19.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_24:
                                    goto loop_peg_18;
                                    success_peg_20:
                                    ;
                                    result_peg_7.addResult(result_peg_19);
                                } while (true);
                                loop_peg_18:
                                ;
                                        
                            }
                            goto success_peg_9;
                            goto loop_peg_6;
                            success_peg_9:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    goto loop_peg_6;
                                }
                            }
                            result_peg_7.setValue(Value((void*) ","));
                        
                        
                        
                        {
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_37(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_40;
                                            }
                                        }
                                        result_peg_37.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_40:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_42;
                                            }
                                        }
                                        result_peg_37.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_42:
                                    goto loop_peg_36;
                                    success_peg_38:
                                    ;
                                    result_peg_7.addResult(result_peg_37);
                                } while (true);
                                loop_peg_36:
                                ;
                                        
                            }
                            goto success_peg_27;
                            goto loop_peg_6;
                            success_peg_27:
                            ;
                        
                        
                        
                        result_peg_7 = rule_expr_c(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
                expr_rest = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(expr1, expr_rest);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_args = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_args = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk11 != 0 && column_peg_1.chunk11->chunk_function_name.calculated()){
        return column_peg_1.chunk11->chunk_function_name;
    }
    
    RuleTrace trace_peg_66(stream, "function_name");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("abs"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        result_peg_2.setValue(Value((void*) "abs"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("const"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        result_peg_4.setValue(Value((void*) "const"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        for (int i = 0; i < 13; i++){
            if (compareCharCase("selfanimexist"[i], stream.get(result_peg_6.getPosition()))){
                result_peg_6.nextPosition();
            } else {
                goto out_peg_7;
            }
        }
        result_peg_6.setValue(Value((void*) "selfanimexist"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
        Result result_peg_8(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("ifelse"[i], stream.get(result_peg_8.getPosition()))){
                result_peg_8.nextPosition();
            } else {
                goto out_peg_9;
            }
        }
        result_peg_8.setValue(Value((void*) "ifelse"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_8;
        stream.update(result_peg_8.getPosition());
        
        
        return result_peg_8;
        out_peg_9:
        Result result_peg_10(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("gethitvar"[i], stream.get(result_peg_10.getPosition()))){
                result_peg_10.nextPosition();
            } else {
                goto out_peg_11;
            }
        }
        result_peg_10.setValue(Value((void*) "gethitvar"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_10;
        stream.update(result_peg_10.getPosition());
        
        
        return result_peg_10;
        out_peg_11:
        Result result_peg_12(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("floor"[i], stream.get(result_peg_12.getPosition()))){
                result_peg_12.nextPosition();
            } else {
                goto out_peg_13;
            }
        }
        result_peg_12.setValue(Value((void*) "floor"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_12;
        stream.update(result_peg_12.getPosition());
        
        
        return result_peg_12;
        out_peg_13:
        Result result_peg_14(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("ceil"[i], stream.get(result_peg_14.getPosition()))){
                result_peg_14.nextPosition();
            } else {
                goto out_peg_15;
            }
        }
        result_peg_14.setValue(Value((void*) "ceil"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_14;
        stream.update(result_peg_14.getPosition());
        
        
        return result_peg_14;
        out_peg_15:
        Result result_peg_16(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("exp"[i], stream.get(result_peg_16.getPosition()))){
                result_peg_16.nextPosition();
            } else {
                goto out_peg_17;
            }
        }
        result_peg_16.setValue(Value((void*) "exp"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_17:
        Result result_peg_18(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("acos"[i], stream.get(result_peg_18.getPosition()))){
                result_peg_18.nextPosition();
            } else {
                goto out_peg_19;
            }
        }
        result_peg_18.setValue(Value((void*) "acos"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_18;
        stream.update(result_peg_18.getPosition());
        
        
        return result_peg_18;
        out_peg_19:
        Result result_peg_20(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("asin"[i], stream.get(result_peg_20.getPosition()))){
                result_peg_20.nextPosition();
            } else {
                goto out_peg_21;
            }
        }
        result_peg_20.setValue(Value((void*) "asin"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_20;
        stream.update(result_peg_20.getPosition());
        
        
        return result_peg_20;
        out_peg_21:
        Result result_peg_22(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("atan"[i], stream.get(result_peg_22.getPosition()))){
                result_peg_22.nextPosition();
            } else {
                goto out_peg_23;
            }
        }
        result_peg_22.setValue(Value((void*) "atan"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_22;
        stream.update(result_peg_22.getPosition());
        
        
        return result_peg_22;
        out_peg_23:
        Result result_peg_24(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("tan"[i], stream.get(result_peg_24.getPosition()))){
                result_peg_24.nextPosition();
            } else {
                goto out_peg_25;
            }
        }
        result_peg_24.setValue(Value((void*) "tan"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_24;
        stream.update(result_peg_24.getPosition());
        
        
        return result_peg_24;
        out_peg_25:
        Result result_peg_26(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("cos"[i], stream.get(result_peg_26.getPosition()))){
                result_peg_26.nextPosition();
            } else {
                goto out_peg_27;
            }
        }
        result_peg_26.setValue(Value((void*) "cos"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_26;
        stream.update(result_peg_26.getPosition());
        
        
        return result_peg_26;
        out_peg_27:
        Result result_peg_28(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("sin"[i], stream.get(result_peg_28.getPosition()))){
                result_peg_28.nextPosition();
            } else {
                goto out_peg_29;
            }
        }
        result_peg_28.setValue(Value((void*) "sin"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_29:
        Result result_peg_30(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("log"[i], stream.get(result_peg_30.getPosition()))){
                result_peg_30.nextPosition();
            } else {
                goto out_peg_31;
            }
        }
        result_peg_30.setValue(Value((void*) "log"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_30;
        stream.update(result_peg_30.getPosition());
        
        
        return result_peg_30;
        out_peg_31:
        Result result_peg_32(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareCharCase("ln"[i], stream.get(result_peg_32.getPosition()))){
                result_peg_32.nextPosition();
            } else {
                goto out_peg_33;
            }
        }
        result_peg_32.setValue(Value((void*) "ln"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_32;
        stream.update(result_peg_32.getPosition());
        
        
        return result_peg_32;
        out_peg_33:
        Result result_peg_34(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("sysfvar"[i], stream.get(result_peg_34.getPosition()))){
                result_peg_34.nextPosition();
            } else {
                goto out_peg_35;
            }
        }
        result_peg_34.setValue(Value((void*) "sysfvar"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_34;
        stream.update(result_peg_34.getPosition());
        
        
        return result_peg_34;
        out_peg_35:
        Result result_peg_36(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("sysvar"[i], stream.get(result_peg_36.getPosition()))){
                result_peg_36.nextPosition();
            } else {
                goto out_peg_37;
            }
        }
        result_peg_36.setValue(Value((void*) "sysvar"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_36;
        stream.update(result_peg_36.getPosition());
        
        
        return result_peg_36;
        out_peg_37:
        Result result_peg_38(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("var"[i], stream.get(result_peg_38.getPosition()))){
                result_peg_38.nextPosition();
            } else {
                goto out_peg_39;
            }
        }
        result_peg_38.setValue(Value((void*) "var"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_38;
        stream.update(result_peg_38.getPosition());
        
        
        return result_peg_38;
        out_peg_39:
        Result result_peg_40(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numexplod"[i], stream.get(result_peg_40.getPosition()))){
                result_peg_40.nextPosition();
            } else {
                goto out_peg_41;
            }
        }
        result_peg_40.setValue(Value((void*) "numexplod"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_40;
        stream.update(result_peg_40.getPosition());
        
        
        return result_peg_40;
        out_peg_41:
        Result result_peg_42(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_42.getPosition()))){
                result_peg_42.nextPosition();
            } else {
                goto out_peg_43;
            }
        }
        result_peg_42.setValue(Value((void*) "numhelper"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_42;
        stream.update(result_peg_42.getPosition());
        
        
        return result_peg_42;
        out_peg_43:
        Result result_peg_44(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numprojid"[i], stream.get(result_peg_44.getPosition()))){
                result_peg_44.nextPosition();
            } else {
                goto out_peg_45;
            }
        }
        result_peg_44.setValue(Value((void*) "numprojid"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_44;
        stream.update(result_peg_44.getPosition());
        
        
        return result_peg_44;
        out_peg_45:
        Result result_peg_46(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("fvar"[i], stream.get(result_peg_46.getPosition()))){
                result_peg_46.nextPosition();
            } else {
                goto out_peg_47;
            }
        }
        result_peg_46.setValue(Value((void*) "fvar"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_46;
        stream.update(result_peg_46.getPosition());
        
        
        return result_peg_46;
        out_peg_47:
        Result result_peg_48(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("ishelper"[i], stream.get(result_peg_48.getPosition()))){
                result_peg_48.nextPosition();
            } else {
                goto out_peg_49;
            }
        }
        result_peg_48.setValue(Value((void*) "ishelper"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_48;
        stream.update(result_peg_48.getPosition());
        
        
        return result_peg_48;
        out_peg_49:
        Result result_peg_50(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numtarget"[i], stream.get(result_peg_50.getPosition()))){
                result_peg_50.nextPosition();
            } else {
                goto out_peg_51;
            }
        }
        result_peg_50.setValue(Value((void*) "numtarget"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_50;
        stream.update(result_peg_50.getPosition());
        
        
        return result_peg_50;
        out_peg_51:
        Result result_peg_52(myposition);
        
        for (int i = 0; i < 12; i++){
            if (compareCharCase("animelemtime"[i], stream.get(result_peg_52.getPosition()))){
                result_peg_52.nextPosition();
            } else {
                goto out_peg_53;
            }
        }
        result_peg_52.setValue(Value((void*) "animelemtime"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_52;
        stream.update(result_peg_52.getPosition());
        
        
        return result_peg_52;
        out_peg_53:
        Result result_peg_54(myposition);
        
        for (int i = 0; i < 10; i++){
            if (compareCharCase("animelemno"[i], stream.get(result_peg_54.getPosition()))){
                result_peg_54.nextPosition();
            } else {
                goto out_peg_55;
            }
        }
        result_peg_54.setValue(Value((void*) "animelemno"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_54;
        stream.update(result_peg_54.getPosition());
        
        
        return result_peg_54;
        out_peg_55:
        Result result_peg_56(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("animexist"[i], stream.get(result_peg_56.getPosition()))){
                result_peg_56.nextPosition();
            } else {
                goto out_peg_57;
            }
        }
        result_peg_56.setValue(Value((void*) "animexist"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_56;
        stream.update(result_peg_56.getPosition());
        
        
        return result_peg_56;
        out_peg_57:
        Result result_peg_58(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projguarded"[i], stream.get(result_peg_58.getPosition()))){
                result_peg_58.nextPosition();
            } else {
                goto out_peg_59;
            }
        }
        result_peg_58.setValue(Value((void*) "projguarded"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_58;
        stream.update(result_peg_58.getPosition());
        
        
        return result_peg_58;
        out_peg_59:
        Result result_peg_60(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projhittime"[i], stream.get(result_peg_60.getPosition()))){
                result_peg_60.nextPosition();
            } else {
                goto out_peg_61;
            }
        }
        result_peg_60.setValue(Value((void*) "projhittime"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_60;
        stream.update(result_peg_60.getPosition());
        
        
        return result_peg_60;
        out_peg_61:
        Result result_peg_62(myposition);
        
        for (int i = 0; i < 15; i++){
            if (compareCharCase("projcontacttime"[i], stream.get(result_peg_62.getPosition()))){
                result_peg_62.nextPosition();
            } else {
                goto out_peg_63;
            }
        }
        result_peg_62.setValue(Value((void*) "projcontacttime"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_62;
        stream.update(result_peg_62.getPosition());
        
        
        return result_peg_62;
        out_peg_63:
        Result result_peg_64(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_64.getPosition()))){
                result_peg_64.nextPosition();
            } else {
                goto out_peg_65;
            }
        }
        result_peg_64.setValue(Value((void*) "numhelper"));
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = result_peg_64;
        stream.update(result_peg_64.getPosition());
        
        
        return result_peg_64;
        out_peg_65:
    
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk11 != 0 && column_peg_1.chunk11->chunk_function_rest.calculated()){
        return column_peg_1.chunk11->chunk_function_rest;
    }
    
    RuleTrace trace_peg_59(stream, "function_rest");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_14(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_17;
                                }
                            }
                            result_peg_14.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_15;
                        out_peg_17:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_14.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_15;
                        out_peg_19:
                        goto loop_peg_13;
                        success_peg_15:
                        ;
                        result_peg_2.addResult(result_peg_14);
                    } while (true);
                    loop_peg_13:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_20;
                success_peg_4:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_20;
                    }
                }
                result_peg_2.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_33(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_33.getPosition()))){
                                    result_peg_33.nextPosition();
                                } else {
                                    goto out_peg_36;
                                }
                            }
                            result_peg_33.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_34;
                        out_peg_36:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_33.getPosition()))){
                                    result_peg_33.nextPosition();
                                } else {
                                    goto out_peg_38;
                                }
                            }
                            result_peg_33.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_34;
                        out_peg_38:
                        goto loop_peg_32;
                        success_peg_34:
                        ;
                        result_peg_2.addResult(result_peg_33);
                    } while (true);
                    loop_peg_32:
                    ;
                            
                }
                goto success_peg_23;
                goto out_peg_20;
                success_peg_23:
                ;
            
            
            
            int save_peg_40 = result_peg_2.getPosition();
                
                result_peg_2 = rule_all_compare(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    
                    result_peg_2 = Result(save_peg_40);
                    result_peg_2.setValue(Value((void*) 0));
                    
                }
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_52(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_52.getPosition()))){
                                    result_peg_52.nextPosition();
                                } else {
                                    goto out_peg_55;
                                }
                            }
                            result_peg_52.setValue(Value((void*) " "));
                                
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
                            result_peg_52.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_53;
                        out_peg_57:
                        goto loop_peg_51;
                        success_peg_53:
                        ;
                        result_peg_2.addResult(result_peg_52);
                    } while (true);
                    loop_peg_51:
                    ;
                            
                }
                goto success_peg_42;
                goto out_peg_20;
                success_peg_42:
                ;
            
            
            
            result_peg_2 = rule_value(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_20;
                }
            
            
        }
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_rest = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_20:
        Result result_peg_58(myposition);
        
        
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_rest = result_peg_58;
        stream.update(result_peg_58.getPosition());
        
        
        return result_peg_58;
    
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_function_rest = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk11 != 0 && column_peg_1.chunk11->chunk_keys.calculated()){
        return column_peg_1.chunk11->chunk_keys;
    }
    
    RuleTrace trace_peg_4(stream, "keys");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2 = rule_key_value_list(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_keys = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
    
        if (column_peg_1.chunk11 == 0){
            column_peg_1.chunk11 = new Chunk11();
        }
        column_peg_1.chunk11->chunk_keys = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk12 != 0 && column_peg_1.chunk12->chunk_key_value_list.calculated()){
        return column_peg_1.chunk12->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_45(stream, "key_value_list");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_key(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                first = result_peg_2.getValues();
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                    
                        {
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_19(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_22;
                                            }
                                        }
                                        result_peg_19.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_22:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_24;
                                            }
                                        }
                                        result_peg_19.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_24:
                                    goto loop_peg_18;
                                    success_peg_20:
                                    ;
                                    result_peg_7.addResult(result_peg_19);
                                } while (true);
                                loop_peg_18:
                                ;
                                        
                            }
                            goto success_peg_9;
                            goto loop_peg_6;
                            success_peg_9:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    goto loop_peg_6;
                                }
                            }
                            result_peg_7.setValue(Value((void*) ","));
                        
                        
                        
                        {
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_37(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_40;
                                            }
                                        }
                                        result_peg_37.setValue(Value((void*) " "));
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_40:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_42;
                                            }
                                        }
                                        result_peg_37.setValue(Value((void*) "\t"));
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_42:
                                    goto loop_peg_36;
                                    success_peg_38:
                                    ;
                                    result_peg_7.addResult(result_peg_37);
                                } while (true);
                                loop_peg_36:
                                ;
                                        
                            }
                            goto success_peg_27;
                            goto loop_peg_6;
                            success_peg_27:
                            ;
                        
                        
                        
                        result_peg_7 = rule_key(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
                rest = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyList(first, rest);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_value_list = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_43(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = makeEmptyKeyList();
                    result_peg_43.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_value_list = result_peg_43;
        stream.update(result_peg_43.getPosition());
        
        
        return result_peg_43;
    
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_value_list = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk12 != 0 && column_peg_1.chunk12->chunk_key.calculated()){
        return column_peg_1.chunk12->chunk_key;
    }
    
    RuleTrace trace_peg_9(stream, "key");
    int myposition = position;
    
    
    Value ok;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_key_real(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2 = rule_key_rest(stream, result_peg_2.getPosition(), result_peg_3.getValues());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                ok = result_peg_2.getValues();
            
            
            
            Result result_peg_8(result_peg_2);
                result_peg_8 = rule_identifier(stream, result_peg_8.getPosition());
                if (result_peg_8.error()){
                    goto not_peg_7;
                }
                goto out_peg_4;
                not_peg_7:
                result_peg_2.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = ok;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_44(stream, "key_rest");
    int myposition = position;
    
    tail_peg_3:
    Value another;
        Value new_left;
    Result result_peg_2(myposition);
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_15(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_18;
                                }
                            }
                            result_peg_15.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_16;
                        out_peg_18:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                    result_peg_15.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_15.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_16;
                        out_peg_20:
                        goto loop_peg_14;
                        success_peg_16:
                        ;
                        result_peg_2.addResult(result_peg_15);
                    } while (true);
                    loop_peg_14:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_21;
                success_peg_5:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_2.setValue(Value((void*) "+"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_2 = rule_key_real(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_21;
                }
                another = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyCombined(left, another);
                    result_peg_2.setValue(value);
                }
                new_left = result_peg_2.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2.getPosition();
        goto tail_peg_3;
        out_peg_21:
        Result result_peg_42(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk12 != 0 && column_peg_1.chunk12->chunk_key_real.calculated()){
        return column_peg_1.chunk12->chunk_key_real;
    }
    
    RuleTrace trace_peg_8(stream, "key_real");
    int myposition = position;
    
    
    Value mods;
        Value name;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    result_peg_5 = rule_key_modifier(stream, result_peg_5.getPosition());
                    if (result_peg_5.error()){
                        goto loop_peg_4;
                    }
                    result_peg_2.addResult(result_peg_5);
                } while (true);
                loop_peg_4:
                ;
                mods = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_key_name(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_7;
                }
                name = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = applyKeyModifiers(mods, makeKey(name));
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_real = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_7:
    
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk12 != 0 && column_peg_1.chunk12->chunk_key_modifier.calculated()){
        return column_peg_1.chunk12->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_22(stream, "key_modifier");
    int myposition = position;
    
    
    Value num;
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "~"));
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_12 = stream.get(result_peg_7.getPosition());
                        if (letter_peg_12 != '\0' && strchr("0123456789", letter_peg_12) != NULL){
                            result_peg_7.nextPosition();
                            result_peg_7.setValue(Value((void*) (long) letter_peg_12));
                        } else {
                            goto out_peg_11;
                        }
                        
                    }
                    goto success_peg_8;
                    out_peg_11:
                    goto loop_peg_6;
                    success_peg_8:
                    ;
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
                num = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new ReleaseKeyModifier((int) *parseDouble(num));
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_modifier = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_13(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("$"[i], stream.get(result_peg_13.getPosition()))){
                        result_peg_13.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
                result_peg_13.setValue(Value((void*) "$"));
            
            
            
            {
                    Value value((void*) 0);
                    value = new DirectionKeyModifier();
                    result_peg_13.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_modifier = result_peg_13;
        stream.update(result_peg_13.getPosition());
        
        
        return result_peg_13;
        out_peg_15:
        Result result_peg_16(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_16.getPosition()))){
                        result_peg_16.nextPosition();
                    } else {
                        goto out_peg_18;
                    }
                }
                result_peg_16.setValue(Value((void*) "/"));
            
            
            
            {
                    Value value((void*) 0);
                    value = new HeldDownKeyModifier();
                    result_peg_16.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_modifier = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_18:
        Result result_peg_19(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_19.getPosition()))){
                        result_peg_19.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
                result_peg_19.setValue(Value((void*) ">"));
            
            
            
            {
                    Value value((void*) 0);
                    value = new OnlyKeyModifier();
                    result_peg_19.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_modifier = result_peg_19;
        stream.update(result_peg_19.getPosition());
        
        
        return result_peg_19;
        out_peg_21:
    
        if (column_peg_1.chunk12 == 0){
            column_peg_1.chunk12 = new Chunk12();
        }
        column_peg_1.chunk12->chunk_key_modifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk13 != 0 && column_peg_1.chunk13->chunk_key_name.calculated()){
        return column_peg_1.chunk13->chunk_key_name;
    }
    
    RuleTrace trace_peg_32(stream, "key_name");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DB"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        result_peg_2.setValue(Value((void*) "DB"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("B"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        result_peg_4.setValue(Value((void*) "B"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DF"[i], stream.get(result_peg_6.getPosition()))){
                result_peg_6.nextPosition();
            } else {
                goto out_peg_7;
            }
        }
        result_peg_6.setValue(Value((void*) "DF"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
        Result result_peg_8(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("D"[i], stream.get(result_peg_8.getPosition()))){
                result_peg_8.nextPosition();
            } else {
                goto out_peg_9;
            }
        }
        result_peg_8.setValue(Value((void*) "D"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_8;
        stream.update(result_peg_8.getPosition());
        
        
        return result_peg_8;
        out_peg_9:
        Result result_peg_10(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("F"[i], stream.get(result_peg_10.getPosition()))){
                result_peg_10.nextPosition();
            } else {
                goto out_peg_11;
            }
        }
        result_peg_10.setValue(Value((void*) "F"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_10;
        stream.update(result_peg_10.getPosition());
        
        
        return result_peg_10;
        out_peg_11:
        Result result_peg_12(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UF"[i], stream.get(result_peg_12.getPosition()))){
                result_peg_12.nextPosition();
            } else {
                goto out_peg_13;
            }
        }
        result_peg_12.setValue(Value((void*) "UF"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_12;
        stream.update(result_peg_12.getPosition());
        
        
        return result_peg_12;
        out_peg_13:
        Result result_peg_14(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UB"[i], stream.get(result_peg_14.getPosition()))){
                result_peg_14.nextPosition();
            } else {
                goto out_peg_15;
            }
        }
        result_peg_14.setValue(Value((void*) "UB"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_14;
        stream.update(result_peg_14.getPosition());
        
        
        return result_peg_14;
        out_peg_15:
        Result result_peg_16(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("U"[i], stream.get(result_peg_16.getPosition()))){
                result_peg_16.nextPosition();
            } else {
                goto out_peg_17;
            }
        }
        result_peg_16.setValue(Value((void*) "U"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_17:
        Result result_peg_18(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("a"[i], stream.get(result_peg_18.getPosition()))){
                result_peg_18.nextPosition();
            } else {
                goto out_peg_19;
            }
        }
        result_peg_18.setValue(Value((void*) "a"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_18;
        stream.update(result_peg_18.getPosition());
        
        
        return result_peg_18;
        out_peg_19:
        Result result_peg_20(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("b"[i], stream.get(result_peg_20.getPosition()))){
                result_peg_20.nextPosition();
            } else {
                goto out_peg_21;
            }
        }
        result_peg_20.setValue(Value((void*) "b"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_20;
        stream.update(result_peg_20.getPosition());
        
        
        return result_peg_20;
        out_peg_21:
        Result result_peg_22(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("c"[i], stream.get(result_peg_22.getPosition()))){
                result_peg_22.nextPosition();
            } else {
                goto out_peg_23;
            }
        }
        result_peg_22.setValue(Value((void*) "c"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_22;
        stream.update(result_peg_22.getPosition());
        
        
        return result_peg_22;
        out_peg_23:
        Result result_peg_24(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("x"[i], stream.get(result_peg_24.getPosition()))){
                result_peg_24.nextPosition();
            } else {
                goto out_peg_25;
            }
        }
        result_peg_24.setValue(Value((void*) "x"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_24;
        stream.update(result_peg_24.getPosition());
        
        
        return result_peg_24;
        out_peg_25:
        Result result_peg_26(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("y"[i], stream.get(result_peg_26.getPosition()))){
                result_peg_26.nextPosition();
            } else {
                goto out_peg_27;
            }
        }
        result_peg_26.setValue(Value((void*) "y"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_26;
        stream.update(result_peg_26.getPosition());
        
        
        return result_peg_26;
        out_peg_27:
        Result result_peg_28(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("z"[i], stream.get(result_peg_28.getPosition()))){
                result_peg_28.nextPosition();
            } else {
                goto out_peg_29;
            }
        }
        result_peg_28.setValue(Value((void*) "z"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_29:
        Result result_peg_30(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("s"[i], stream.get(result_peg_30.getPosition()))){
                result_peg_30.nextPosition();
            } else {
                goto out_peg_31;
            }
        }
        result_peg_30.setValue(Value((void*) "s"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = result_peg_30;
        stream.update(result_peg_30.getPosition());
        
        
        return result_peg_30;
        out_peg_31:
    
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_key_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk13 != 0 && column_peg_1.chunk13->chunk_value.calculated()){
        return column_peg_1.chunk13->chunk_value;
    }
    
    RuleTrace trace_peg_19(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2 = rule_float(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        result_peg_4 = rule_integer(stream, result_peg_4.getPosition());
        if (result_peg_4.error()){
            goto out_peg_5;
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        result_peg_6 = rule_keyword(stream, result_peg_6.getPosition());
        if (result_peg_6.error()){
            goto out_peg_7;
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
        Result result_peg_8(myposition);
        
        {
        
            Result result_peg_11(result_peg_8);
                result_peg_11 = rule_keyword(stream, result_peg_11.getPosition());
                if (result_peg_11.error()){
                    goto not_peg_10;
                }
                goto out_peg_12;
                not_peg_10:
                result_peg_8.setValue(Value((void*)0));
            
            
            
            result_peg_8 = rule_identifier(stream, result_peg_8.getPosition());
                if (result_peg_8.error()){
                    goto out_peg_12;
                }
            
            
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = result_peg_8;
        stream.update(result_peg_8.getPosition());
        
        
        return result_peg_8;
        out_peg_12:
        Result result_peg_13(myposition);
        
        result_peg_13 = rule_range(stream, result_peg_13.getPosition());
        if (result_peg_13.error()){
            goto out_peg_14;
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = result_peg_13;
        stream.update(result_peg_13.getPosition());
        
        
        return result_peg_13;
        out_peg_14:
        Result result_peg_15(myposition);
        
        result_peg_15 = rule_string(stream, result_peg_15.getPosition());
        if (result_peg_15.error()){
            goto out_peg_16;
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = result_peg_15;
        stream.update(result_peg_15.getPosition());
        
        
        return result_peg_15;
        out_peg_16:
        Result result_peg_17(myposition);
        
        result_peg_17 = rule_hitflag(stream, result_peg_17.getPosition());
        if (result_peg_17.error()){
            goto out_peg_18;
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = result_peg_17;
        stream.update(result_peg_17.getPosition());
        
        
        return result_peg_17;
        out_peg_18:
    
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk13 != 0 && column_peg_1.chunk13->chunk_helper.calculated()){
        return column_peg_1.chunk13->chunk_helper;
    }
    
    RuleTrace trace_peg_60(stream, "helper");
    int myposition = position;
    
    
    Value name;
        Value id;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_helper__name(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                name = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            int save_peg_23 = result_peg_2.getPosition();
                
                result_peg_2 = rule_helper__expression(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    
                    result_peg_2 = Result(save_peg_23);
                    result_peg_2.setValue(Value((void*) 0));
                    
                }
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_35(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    goto out_peg_38;
                                }
                            }
                            result_peg_35.setValue(Value((void*) " "));
                                
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
                            result_peg_35.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_36;
                        out_peg_40:
                        goto loop_peg_34;
                        success_peg_36:
                        ;
                        result_peg_2.addResult(result_peg_35);
                    } while (true);
                    loop_peg_34:
                    ;
                            
                }
                goto success_peg_25;
                goto out_peg_4;
                success_peg_25:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) ","));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_53(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_53.getPosition()))){
                                    result_peg_53.nextPosition();
                                } else {
                                    goto out_peg_56;
                                }
                            }
                            result_peg_53.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_54;
                        out_peg_56:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_53.getPosition()))){
                                    result_peg_53.nextPosition();
                                } else {
                                    goto out_peg_58;
                                }
                            }
                            result_peg_53.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_54;
                        out_peg_58:
                        goto loop_peg_52;
                        success_peg_54:
                        ;
                        result_peg_2.addResult(result_peg_53);
                    } while (true);
                    loop_peg_52:
                    ;
                            
                }
                goto success_peg_43;
                goto out_peg_4;
                success_peg_43:
                ;
            
            
            
            result_peg_2 = rule_helper__identifier(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                id = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHelper(name, id);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk13 != 0 && column_peg_1.chunk13->chunk_helper__expression.calculated()){
        return column_peg_1.chunk13->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_40(stream, "helper__expression");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "("));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            result_peg_2 = rule_expr_c(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_4;
                success_peg_24:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) ")"));
            
            
        }
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__expression = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__expression = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk13 != 0 && column_peg_1.chunk13->chunk_helper__name.calculated()){
        return column_peg_1.chunk13->chunk_helper__name;
    }
    
    RuleTrace trace_peg_18(stream, "helper__name");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("parent"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        result_peg_2.setValue(Value((void*) "parent"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("root"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        result_peg_4.setValue(Value((void*) "root"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("helper"[i], stream.get(result_peg_6.getPosition()))){
                result_peg_6.nextPosition();
            } else {
                goto out_peg_7;
            }
        }
        result_peg_6.setValue(Value((void*) "helper"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_7:
        Result result_peg_8(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("target"[i], stream.get(result_peg_8.getPosition()))){
                result_peg_8.nextPosition();
            } else {
                goto out_peg_9;
            }
        }
        result_peg_8.setValue(Value((void*) "target"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_8;
        stream.update(result_peg_8.getPosition());
        
        
        return result_peg_8;
        out_peg_9:
        Result result_peg_10(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("partner"[i], stream.get(result_peg_10.getPosition()))){
                result_peg_10.nextPosition();
            } else {
                goto out_peg_11;
            }
        }
        result_peg_10.setValue(Value((void*) "partner"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_10;
        stream.update(result_peg_10.getPosition());
        
        
        return result_peg_10;
        out_peg_11:
        Result result_peg_12(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("enemyNear"[i], stream.get(result_peg_12.getPosition()))){
                result_peg_12.nextPosition();
            } else {
                goto out_peg_13;
            }
        }
        result_peg_12.setValue(Value((void*) "enemyNear"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_12;
        stream.update(result_peg_12.getPosition());
        
        
        return result_peg_12;
        out_peg_13:
        Result result_peg_14(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("enemy"[i], stream.get(result_peg_14.getPosition()))){
                result_peg_14.nextPosition();
            } else {
                goto out_peg_15;
            }
        }
        result_peg_14.setValue(Value((void*) "enemy"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_14;
        stream.update(result_peg_14.getPosition());
        
        
        return result_peg_14;
        out_peg_15:
        Result result_peg_16(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("playerid"[i], stream.get(result_peg_16.getPosition()))){
                result_peg_16.nextPosition();
            } else {
                goto out_peg_17;
            }
        }
        result_peg_16.setValue(Value((void*) "playerid"));
        
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_17:
    
        if (column_peg_1.chunk13 == 0){
            column_peg_1.chunk13 = new Chunk13();
        }
        column_peg_1.chunk13->chunk_helper__name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk14 != 0 && column_peg_1.chunk14->chunk_helper__identifier.calculated()){
        return column_peg_1.chunk14->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_11(stream, "helper__identifier");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2 = rule_function(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_helper__identifier = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        result_peg_4 = rule_keyword(stream, result_peg_4.getPosition());
        if (result_peg_4.error()){
            goto out_peg_5;
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_helper__identifier = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        {
        
            Result result_peg_9(result_peg_6);
                result_peg_9 = rule_keyword(stream, result_peg_9.getPosition());
                if (result_peg_9.error()){
                    goto not_peg_8;
                }
                goto out_peg_10;
                not_peg_8:
                result_peg_6.setValue(Value((void*)0));
            
            
            
            result_peg_6 = rule_identifier(stream, result_peg_6.getPosition());
                if (result_peg_6.error()){
                    goto out_peg_10;
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_helper__identifier = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_10:
    
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_helper__identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk14 != 0 && column_peg_1.chunk14->chunk_hitflag.calculated()){
        return column_peg_1.chunk14->chunk_hitflag;
    }
    
    RuleTrace trace_peg_26(stream, "hitflag");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("A"[i], stream.get(result_peg_5.getPosition()))){
                                result_peg_5.nextPosition();
                            } else {
                                goto out_peg_8;
                            }
                        }
                        result_peg_5.setValue(Value((void*) "A"));
                            
                    }
                    goto success_peg_6;
                    out_peg_8:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("M"[i], stream.get(result_peg_5.getPosition()))){
                                result_peg_5.nextPosition();
                            } else {
                                goto out_peg_10;
                            }
                        }
                        result_peg_5.setValue(Value((void*) "M"));
                            
                    }
                    goto success_peg_6;
                    out_peg_10:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("F"[i], stream.get(result_peg_5.getPosition()))){
                                result_peg_5.nextPosition();
                            } else {
                                goto out_peg_12;
                            }
                        }
                        result_peg_5.setValue(Value((void*) "F"));
                            
                    }
                    goto success_peg_6;
                    out_peg_12:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("D"[i], stream.get(result_peg_5.getPosition()))){
                                result_peg_5.nextPosition();
                            } else {
                                goto out_peg_14;
                            }
                        }
                        result_peg_5.setValue(Value((void*) "D"));
                            
                    }
                    goto success_peg_6;
                    out_peg_14:
                    goto loop_peg_4;
                    success_peg_6:
                    ;
                    result_peg_2.addResult(result_peg_5);
                } while (true);
                loop_peg_4:
                if (result_peg_2.matches() == 0){
                    goto out_peg_15;
                }
            
            
            
            int save_peg_17 = result_peg_2.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_20;
                        }
                    }
                    result_peg_2.setValue(Value((void*) "+"));
                        
                }
                goto success_peg_18;
                out_peg_20:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_22;
                        }
                    }
                    result_peg_2.setValue(Value((void*) "-"));
                        
                }
                goto success_peg_18;
                out_peg_22:
                
                result_peg_2 = Result(save_peg_17);
                result_peg_2.setValue(Value((void*) 0));
                
                success_peg_18:
                ;
            
            
            
            Result result_peg_25(result_peg_2);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_25.getPosition()))){
                        result_peg_25.nextPosition();
                    } else {
                        goto not_peg_24;
                    }
                }
                result_peg_25.setValue(Value((void*) "."));
                goto out_peg_15;
                not_peg_24:
                result_peg_2.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSimpleIdentifier("A");
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_hitflag = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_15:
    
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_hitflag = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk14 != 0 && column_peg_1.chunk14->chunk_keyword.calculated()){
        return column_peg_1.chunk14->chunk_keyword;
    }
    
    RuleTrace trace_peg_8(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_keyword_real(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            Result result_peg_7(result_peg_2);
                result_peg_7 = rule_alpha_digit(stream, result_peg_7.getPosition());
                if (result_peg_7.error()){
                    goto not_peg_6;
                }
                goto out_peg_4;
                not_peg_6:
                result_peg_2.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_3.getValues();
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk14 != 0 && column_peg_1.chunk14->chunk_keyword_real.calculated()){
        return column_peg_1.chunk14->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_422(stream, "keyword_real");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "vel"));
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_17(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    goto out_peg_20;
                                }
                            }
                            result_peg_17.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_18;
                        out_peg_20:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    goto out_peg_22;
                                }
                            }
                            result_peg_17.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_18;
                        out_peg_22:
                        goto loop_peg_16;
                        success_peg_18:
                        ;
                        result_peg_2.addResult(result_peg_17);
                    } while (true);
                    loop_peg_16:
                    if (result_peg_2.matches() == 0){
                        goto out_peg_15;
                    }
                    
                }
                goto success_peg_6;
                out_peg_15:
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel y");
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_24(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_24.getPosition()))){
                        result_peg_24.nextPosition();
                    } else {
                        goto out_peg_26;
                    }
                }
                result_peg_24.setValue(Value((void*) "vel"));
            
            
            
            {
                    
                    result_peg_24.reset();
                    do{
                        Result result_peg_39(result_peg_24.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    goto out_peg_42;
                                }
                            }
                            result_peg_39.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_40;
                        out_peg_42:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    goto out_peg_44;
                                }
                            }
                            result_peg_39.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_40;
                        out_peg_44:
                        goto loop_peg_38;
                        success_peg_40:
                        ;
                        result_peg_24.addResult(result_peg_39);
                    } while (true);
                    loop_peg_38:
                    if (result_peg_24.matches() == 0){
                        goto out_peg_37;
                    }
                    
                }
                goto success_peg_28;
                out_peg_37:
                goto out_peg_26;
                success_peg_28:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_24.getPosition()))){
                        result_peg_24.nextPosition();
                    } else {
                        goto out_peg_26;
                    }
                }
                result_peg_24.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel x");
                    result_peg_24.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_24;
        stream.update(result_peg_24.getPosition());
        
        
        return result_peg_24;
        out_peg_26:
        Result result_peg_46(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_46.getPosition()))){
                        result_peg_46.nextPosition();
                    } else {
                        goto out_peg_48;
                    }
                }
                result_peg_46.setValue(Value((void*) "pos"));
            
            
            
            {
                    
                    result_peg_46.reset();
                    do{
                        Result result_peg_61(result_peg_46.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    goto out_peg_64;
                                }
                            }
                            result_peg_61.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_62;
                        out_peg_64:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    goto out_peg_66;
                                }
                            }
                            result_peg_61.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_62;
                        out_peg_66:
                        goto loop_peg_60;
                        success_peg_62:
                        ;
                        result_peg_46.addResult(result_peg_61);
                    } while (true);
                    loop_peg_60:
                    if (result_peg_46.matches() == 0){
                        goto out_peg_59;
                    }
                    
                }
                goto success_peg_50;
                out_peg_59:
                goto out_peg_48;
                success_peg_50:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_46.getPosition()))){
                        result_peg_46.nextPosition();
                    } else {
                        goto out_peg_48;
                    }
                }
                result_peg_46.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos y");
                    result_peg_46.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_46;
        stream.update(result_peg_46.getPosition());
        
        
        return result_peg_46;
        out_peg_48:
        Result result_peg_68(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_68.getPosition()))){
                        result_peg_68.nextPosition();
                    } else {
                        goto out_peg_70;
                    }
                }
                result_peg_68.setValue(Value((void*) "pos"));
            
            
            
            {
                    
                    result_peg_68.reset();
                    do{
                        Result result_peg_83(result_peg_68.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_83.getPosition()))){
                                    result_peg_83.nextPosition();
                                } else {
                                    goto out_peg_86;
                                }
                            }
                            result_peg_83.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_84;
                        out_peg_86:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_83.getPosition()))){
                                    result_peg_83.nextPosition();
                                } else {
                                    goto out_peg_88;
                                }
                            }
                            result_peg_83.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_84;
                        out_peg_88:
                        goto loop_peg_82;
                        success_peg_84:
                        ;
                        result_peg_68.addResult(result_peg_83);
                    } while (true);
                    loop_peg_82:
                    if (result_peg_68.matches() == 0){
                        goto out_peg_81;
                    }
                    
                }
                goto success_peg_72;
                out_peg_81:
                goto out_peg_70;
                success_peg_72:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_68.getPosition()))){
                        result_peg_68.nextPosition();
                    } else {
                        goto out_peg_70;
                    }
                }
                result_peg_68.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos x");
                    result_peg_68.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_68;
        stream.update(result_peg_68.getPosition());
        
        
        return result_peg_68;
        out_peg_70:
        Result result_peg_90(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_90.getPosition()))){
                        result_peg_90.nextPosition();
                    } else {
                        goto out_peg_92;
                    }
                }
                result_peg_90.setValue(Value((void*) "p2dist"));
            
            
            
            {
                    
                    result_peg_90.reset();
                    do{
                        Result result_peg_105(result_peg_90.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_105.getPosition()))){
                                    result_peg_105.nextPosition();
                                } else {
                                    goto out_peg_108;
                                }
                            }
                            result_peg_105.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_106;
                        out_peg_108:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_105.getPosition()))){
                                    result_peg_105.nextPosition();
                                } else {
                                    goto out_peg_110;
                                }
                            }
                            result_peg_105.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_106;
                        out_peg_110:
                        goto loop_peg_104;
                        success_peg_106:
                        ;
                        result_peg_90.addResult(result_peg_105);
                    } while (true);
                    loop_peg_104:
                    if (result_peg_90.matches() == 0){
                        goto out_peg_103;
                    }
                    
                }
                goto success_peg_94;
                out_peg_103:
                goto out_peg_92;
                success_peg_94:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_90.getPosition()))){
                        result_peg_90.nextPosition();
                    } else {
                        goto out_peg_92;
                    }
                }
                result_peg_90.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist x");
                    result_peg_90.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_90;
        stream.update(result_peg_90.getPosition());
        
        
        return result_peg_90;
        out_peg_92:
        Result result_peg_112(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_112.getPosition()))){
                        result_peg_112.nextPosition();
                    } else {
                        goto out_peg_114;
                    }
                }
                result_peg_112.setValue(Value((void*) "p2dist"));
            
            
            
            {
                    
                    result_peg_112.reset();
                    do{
                        Result result_peg_127(result_peg_112.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_127.getPosition()))){
                                    result_peg_127.nextPosition();
                                } else {
                                    goto out_peg_130;
                                }
                            }
                            result_peg_127.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_128;
                        out_peg_130:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_127.getPosition()))){
                                    result_peg_127.nextPosition();
                                } else {
                                    goto out_peg_132;
                                }
                            }
                            result_peg_127.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_128;
                        out_peg_132:
                        goto loop_peg_126;
                        success_peg_128:
                        ;
                        result_peg_112.addResult(result_peg_127);
                    } while (true);
                    loop_peg_126:
                    if (result_peg_112.matches() == 0){
                        goto out_peg_125;
                    }
                    
                }
                goto success_peg_116;
                out_peg_125:
                goto out_peg_114;
                success_peg_116:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_112.getPosition()))){
                        result_peg_112.nextPosition();
                    } else {
                        goto out_peg_114;
                    }
                }
                result_peg_112.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist y");
                    result_peg_112.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_112;
        stream.update(result_peg_112.getPosition());
        
        
        return result_peg_112;
        out_peg_114:
        Result result_peg_134(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_134.getPosition()))){
                        result_peg_134.nextPosition();
                    } else {
                        goto out_peg_136;
                    }
                }
                result_peg_134.setValue(Value((void*) "p1dist"));
            
            
            
            {
                    
                    result_peg_134.reset();
                    do{
                        Result result_peg_149(result_peg_134.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_149.getPosition()))){
                                    result_peg_149.nextPosition();
                                } else {
                                    goto out_peg_152;
                                }
                            }
                            result_peg_149.setValue(Value((void*) " "));
                                
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
                            result_peg_149.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_150;
                        out_peg_154:
                        goto loop_peg_148;
                        success_peg_150:
                        ;
                        result_peg_134.addResult(result_peg_149);
                    } while (true);
                    loop_peg_148:
                    if (result_peg_134.matches() == 0){
                        goto out_peg_147;
                    }
                    
                }
                goto success_peg_138;
                out_peg_147:
                goto out_peg_136;
                success_peg_138:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_134.getPosition()))){
                        result_peg_134.nextPosition();
                    } else {
                        goto out_peg_136;
                    }
                }
                result_peg_134.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist x");
                    result_peg_134.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_134;
        stream.update(result_peg_134.getPosition());
        
        
        return result_peg_134;
        out_peg_136:
        Result result_peg_156(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_156.getPosition()))){
                        result_peg_156.nextPosition();
                    } else {
                        goto out_peg_158;
                    }
                }
                result_peg_156.setValue(Value((void*) "p1dist"));
            
            
            
            {
                    
                    result_peg_156.reset();
                    do{
                        Result result_peg_171(result_peg_156.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_171.getPosition()))){
                                    result_peg_171.nextPosition();
                                } else {
                                    goto out_peg_174;
                                }
                            }
                            result_peg_171.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_172;
                        out_peg_174:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_171.getPosition()))){
                                    result_peg_171.nextPosition();
                                } else {
                                    goto out_peg_176;
                                }
                            }
                            result_peg_171.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_172;
                        out_peg_176:
                        goto loop_peg_170;
                        success_peg_172:
                        ;
                        result_peg_156.addResult(result_peg_171);
                    } while (true);
                    loop_peg_170:
                    if (result_peg_156.matches() == 0){
                        goto out_peg_169;
                    }
                    
                }
                goto success_peg_160;
                out_peg_169:
                goto out_peg_158;
                success_peg_160:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_156.getPosition()))){
                        result_peg_156.nextPosition();
                    } else {
                        goto out_peg_158;
                    }
                }
                result_peg_156.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist y");
                    result_peg_156.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_156;
        stream.update(result_peg_156.getPosition());
        
        
        return result_peg_156;
        out_peg_158:
        Result result_peg_178(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_178.getPosition()))){
                        result_peg_178.nextPosition();
                    } else {
                        goto out_peg_180;
                    }
                }
                result_peg_178.setValue(Value((void*) "p2bodydist"));
            
            
            
            {
                    
                    result_peg_178.reset();
                    do{
                        Result result_peg_193(result_peg_178.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_193.getPosition()))){
                                    result_peg_193.nextPosition();
                                } else {
                                    goto out_peg_196;
                                }
                            }
                            result_peg_193.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_194;
                        out_peg_196:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_193.getPosition()))){
                                    result_peg_193.nextPosition();
                                } else {
                                    goto out_peg_198;
                                }
                            }
                            result_peg_193.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_194;
                        out_peg_198:
                        goto loop_peg_192;
                        success_peg_194:
                        ;
                        result_peg_178.addResult(result_peg_193);
                    } while (true);
                    loop_peg_192:
                    if (result_peg_178.matches() == 0){
                        goto out_peg_191;
                    }
                    
                }
                goto success_peg_182;
                out_peg_191:
                goto out_peg_180;
                success_peg_182:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_178.getPosition()))){
                        result_peg_178.nextPosition();
                    } else {
                        goto out_peg_180;
                    }
                }
                result_peg_178.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist x");
                    result_peg_178.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_178;
        stream.update(result_peg_178.getPosition());
        
        
        return result_peg_178;
        out_peg_180:
        Result result_peg_200(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_200.getPosition()))){
                        result_peg_200.nextPosition();
                    } else {
                        goto out_peg_202;
                    }
                }
                result_peg_200.setValue(Value((void*) "p2bodydist"));
            
            
            
            {
                    
                    result_peg_200.reset();
                    do{
                        Result result_peg_215(result_peg_200.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_215.getPosition()))){
                                    result_peg_215.nextPosition();
                                } else {
                                    goto out_peg_218;
                                }
                            }
                            result_peg_215.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_216;
                        out_peg_218:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_215.getPosition()))){
                                    result_peg_215.nextPosition();
                                } else {
                                    goto out_peg_220;
                                }
                            }
                            result_peg_215.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_216;
                        out_peg_220:
                        goto loop_peg_214;
                        success_peg_216:
                        ;
                        result_peg_200.addResult(result_peg_215);
                    } while (true);
                    loop_peg_214:
                    if (result_peg_200.matches() == 0){
                        goto out_peg_213;
                    }
                    
                }
                goto success_peg_204;
                out_peg_213:
                goto out_peg_202;
                success_peg_204:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_200.getPosition()))){
                        result_peg_200.nextPosition();
                    } else {
                        goto out_peg_202;
                    }
                }
                result_peg_200.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist y");
                    result_peg_200.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_200;
        stream.update(result_peg_200.getPosition());
        
        
        return result_peg_200;
        out_peg_202:
        Result result_peg_222(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_222.getPosition()))){
                        result_peg_222.nextPosition();
                    } else {
                        goto out_peg_224;
                    }
                }
                result_peg_222.setValue(Value((void*) "p1bodydist"));
            
            
            
            {
                    
                    result_peg_222.reset();
                    do{
                        Result result_peg_237(result_peg_222.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_237.getPosition()))){
                                    result_peg_237.nextPosition();
                                } else {
                                    goto out_peg_240;
                                }
                            }
                            result_peg_237.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_238;
                        out_peg_240:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_237.getPosition()))){
                                    result_peg_237.nextPosition();
                                } else {
                                    goto out_peg_242;
                                }
                            }
                            result_peg_237.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_238;
                        out_peg_242:
                        goto loop_peg_236;
                        success_peg_238:
                        ;
                        result_peg_222.addResult(result_peg_237);
                    } while (true);
                    loop_peg_236:
                    if (result_peg_222.matches() == 0){
                        goto out_peg_235;
                    }
                    
                }
                goto success_peg_226;
                out_peg_235:
                goto out_peg_224;
                success_peg_226:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_222.getPosition()))){
                        result_peg_222.nextPosition();
                    } else {
                        goto out_peg_224;
                    }
                }
                result_peg_222.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist x");
                    result_peg_222.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_222;
        stream.update(result_peg_222.getPosition());
        
        
        return result_peg_222;
        out_peg_224:
        Result result_peg_244(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_244.getPosition()))){
                        result_peg_244.nextPosition();
                    } else {
                        goto out_peg_246;
                    }
                }
                result_peg_244.setValue(Value((void*) "p1bodydist"));
            
            
            
            {
                    
                    result_peg_244.reset();
                    do{
                        Result result_peg_259(result_peg_244.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_259.getPosition()))){
                                    result_peg_259.nextPosition();
                                } else {
                                    goto out_peg_262;
                                }
                            }
                            result_peg_259.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_260;
                        out_peg_262:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_259.getPosition()))){
                                    result_peg_259.nextPosition();
                                } else {
                                    goto out_peg_264;
                                }
                            }
                            result_peg_259.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_260;
                        out_peg_264:
                        goto loop_peg_258;
                        success_peg_260:
                        ;
                        result_peg_244.addResult(result_peg_259);
                    } while (true);
                    loop_peg_258:
                    if (result_peg_244.matches() == 0){
                        goto out_peg_257;
                    }
                    
                }
                goto success_peg_248;
                out_peg_257:
                goto out_peg_246;
                success_peg_248:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_244.getPosition()))){
                        result_peg_244.nextPosition();
                    } else {
                        goto out_peg_246;
                    }
                }
                result_peg_244.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist y");
                    result_peg_244.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_244;
        stream.update(result_peg_244.getPosition());
        
        
        return result_peg_244;
        out_peg_246:
        Result result_peg_266(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_266.getPosition()))){
                        result_peg_266.nextPosition();
                    } else {
                        goto out_peg_268;
                    }
                }
                result_peg_266.setValue(Value((void*) "parentdist"));
            
            
            
            {
                    
                    result_peg_266.reset();
                    do{
                        Result result_peg_281(result_peg_266.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_281.getPosition()))){
                                    result_peg_281.nextPosition();
                                } else {
                                    goto out_peg_284;
                                }
                            }
                            result_peg_281.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_282;
                        out_peg_284:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_281.getPosition()))){
                                    result_peg_281.nextPosition();
                                } else {
                                    goto out_peg_286;
                                }
                            }
                            result_peg_281.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_282;
                        out_peg_286:
                        goto loop_peg_280;
                        success_peg_282:
                        ;
                        result_peg_266.addResult(result_peg_281);
                    } while (true);
                    loop_peg_280:
                    if (result_peg_266.matches() == 0){
                        goto out_peg_279;
                    }
                    
                }
                goto success_peg_270;
                out_peg_279:
                goto out_peg_268;
                success_peg_270:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_266.getPosition()))){
                        result_peg_266.nextPosition();
                    } else {
                        goto out_peg_268;
                    }
                }
                result_peg_266.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_266.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_266;
        stream.update(result_peg_266.getPosition());
        
        
        return result_peg_266;
        out_peg_268:
        Result result_peg_288(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_288.getPosition()))){
                        result_peg_288.nextPosition();
                    } else {
                        goto out_peg_290;
                    }
                }
                result_peg_288.setValue(Value((void*) "screenpos"));
            
            
            
            {
                    
                    result_peg_288.reset();
                    do{
                        Result result_peg_303(result_peg_288.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_303.getPosition()))){
                                    result_peg_303.nextPosition();
                                } else {
                                    goto out_peg_306;
                                }
                            }
                            result_peg_303.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_304;
                        out_peg_306:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_303.getPosition()))){
                                    result_peg_303.nextPosition();
                                } else {
                                    goto out_peg_308;
                                }
                            }
                            result_peg_303.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_304;
                        out_peg_308:
                        goto loop_peg_302;
                        success_peg_304:
                        ;
                        result_peg_288.addResult(result_peg_303);
                    } while (true);
                    loop_peg_302:
                    if (result_peg_288.matches() == 0){
                        goto out_peg_301;
                    }
                    
                }
                goto success_peg_292;
                out_peg_301:
                goto out_peg_290;
                success_peg_292:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_288.getPosition()))){
                        result_peg_288.nextPosition();
                    } else {
                        goto out_peg_290;
                    }
                }
                result_peg_288.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos x");
                    result_peg_288.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_288;
        stream.update(result_peg_288.getPosition());
        
        
        return result_peg_288;
        out_peg_290:
        Result result_peg_310(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_310.getPosition()))){
                        result_peg_310.nextPosition();
                    } else {
                        goto out_peg_312;
                    }
                }
                result_peg_310.setValue(Value((void*) "screenpos"));
            
            
            
            {
                    
                    result_peg_310.reset();
                    do{
                        Result result_peg_325(result_peg_310.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_325.getPosition()))){
                                    result_peg_325.nextPosition();
                                } else {
                                    goto out_peg_328;
                                }
                            }
                            result_peg_325.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_326;
                        out_peg_328:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_325.getPosition()))){
                                    result_peg_325.nextPosition();
                                } else {
                                    goto out_peg_330;
                                }
                            }
                            result_peg_325.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_326;
                        out_peg_330:
                        goto loop_peg_324;
                        success_peg_326:
                        ;
                        result_peg_310.addResult(result_peg_325);
                    } while (true);
                    loop_peg_324:
                    if (result_peg_310.matches() == 0){
                        goto out_peg_323;
                    }
                    
                }
                goto success_peg_314;
                out_peg_323:
                goto out_peg_312;
                success_peg_314:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_310.getPosition()))){
                        result_peg_310.nextPosition();
                    } else {
                        goto out_peg_312;
                    }
                }
                result_peg_310.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos y");
                    result_peg_310.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_310;
        stream.update(result_peg_310.getPosition());
        
        
        return result_peg_310;
        out_peg_312:
        Result result_peg_332(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_332.getPosition()))){
                        result_peg_332.nextPosition();
                    } else {
                        goto out_peg_334;
                    }
                }
                result_peg_332.setValue(Value((void*) "parentdist"));
            
            
            
            {
                    
                    result_peg_332.reset();
                    do{
                        Result result_peg_347(result_peg_332.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_347.getPosition()))){
                                    result_peg_347.nextPosition();
                                } else {
                                    goto out_peg_350;
                                }
                            }
                            result_peg_347.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_348;
                        out_peg_350:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_347.getPosition()))){
                                    result_peg_347.nextPosition();
                                } else {
                                    goto out_peg_352;
                                }
                            }
                            result_peg_347.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_348;
                        out_peg_352:
                        goto loop_peg_346;
                        success_peg_348:
                        ;
                        result_peg_332.addResult(result_peg_347);
                    } while (true);
                    loop_peg_346:
                    if (result_peg_332.matches() == 0){
                        goto out_peg_345;
                    }
                    
                }
                goto success_peg_336;
                out_peg_345:
                goto out_peg_334;
                success_peg_336:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_332.getPosition()))){
                        result_peg_332.nextPosition();
                    } else {
                        goto out_peg_334;
                    }
                }
                result_peg_332.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_332.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_332;
        stream.update(result_peg_332.getPosition());
        
        
        return result_peg_332;
        out_peg_334:
        Result result_peg_354(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_354.getPosition()))){
                        result_peg_354.nextPosition();
                    } else {
                        goto out_peg_356;
                    }
                }
                result_peg_354.setValue(Value((void*) "parentdist"));
            
            
            
            {
                    
                    result_peg_354.reset();
                    do{
                        Result result_peg_369(result_peg_354.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_369.getPosition()))){
                                    result_peg_369.nextPosition();
                                } else {
                                    goto out_peg_372;
                                }
                            }
                            result_peg_369.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_370;
                        out_peg_372:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_369.getPosition()))){
                                    result_peg_369.nextPosition();
                                } else {
                                    goto out_peg_374;
                                }
                            }
                            result_peg_369.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_370;
                        out_peg_374:
                        goto loop_peg_368;
                        success_peg_370:
                        ;
                        result_peg_354.addResult(result_peg_369);
                    } while (true);
                    loop_peg_368:
                    if (result_peg_354.matches() == 0){
                        goto out_peg_367;
                    }
                    
                }
                goto success_peg_358;
                out_peg_367:
                goto out_peg_356;
                success_peg_358:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_354.getPosition()))){
                        result_peg_354.nextPosition();
                    } else {
                        goto out_peg_356;
                    }
                }
                result_peg_354.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist y");
                    result_peg_354.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_354;
        stream.update(result_peg_354.getPosition());
        
        
        return result_peg_354;
        out_peg_356:
        Result result_peg_376(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_376.getPosition()))){
                        result_peg_376.nextPosition();
                    } else {
                        goto out_peg_378;
                    }
                }
                result_peg_376.setValue(Value((void*) "rootdist"));
            
            
            
            {
                    
                    result_peg_376.reset();
                    do{
                        Result result_peg_391(result_peg_376.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_391.getPosition()))){
                                    result_peg_391.nextPosition();
                                } else {
                                    goto out_peg_394;
                                }
                            }
                            result_peg_391.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_392;
                        out_peg_394:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_391.getPosition()))){
                                    result_peg_391.nextPosition();
                                } else {
                                    goto out_peg_396;
                                }
                            }
                            result_peg_391.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_392;
                        out_peg_396:
                        goto loop_peg_390;
                        success_peg_392:
                        ;
                        result_peg_376.addResult(result_peg_391);
                    } while (true);
                    loop_peg_390:
                    if (result_peg_376.matches() == 0){
                        goto out_peg_389;
                    }
                    
                }
                goto success_peg_380;
                out_peg_389:
                goto out_peg_378;
                success_peg_380:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_376.getPosition()))){
                        result_peg_376.nextPosition();
                    } else {
                        goto out_peg_378;
                    }
                }
                result_peg_376.setValue(Value((void*) "x"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist x");
                    result_peg_376.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_376;
        stream.update(result_peg_376.getPosition());
        
        
        return result_peg_376;
        out_peg_378:
        Result result_peg_398(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_398.getPosition()))){
                        result_peg_398.nextPosition();
                    } else {
                        goto out_peg_400;
                    }
                }
                result_peg_398.setValue(Value((void*) "rootdist"));
            
            
            
            {
                    
                    result_peg_398.reset();
                    do{
                        Result result_peg_413(result_peg_398.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_413.getPosition()))){
                                    result_peg_413.nextPosition();
                                } else {
                                    goto out_peg_416;
                                }
                            }
                            result_peg_413.setValue(Value((void*) " "));
                                
                        }
                        goto success_peg_414;
                        out_peg_416:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_413.getPosition()))){
                                    result_peg_413.nextPosition();
                                } else {
                                    goto out_peg_418;
                                }
                            }
                            result_peg_413.setValue(Value((void*) "\t"));
                                
                        }
                        goto success_peg_414;
                        out_peg_418:
                        goto loop_peg_412;
                        success_peg_414:
                        ;
                        result_peg_398.addResult(result_peg_413);
                    } while (true);
                    loop_peg_412:
                    if (result_peg_398.matches() == 0){
                        goto out_peg_411;
                    }
                    
                }
                goto success_peg_402;
                out_peg_411:
                goto out_peg_400;
                success_peg_402:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_398.getPosition()))){
                        result_peg_398.nextPosition();
                    } else {
                        goto out_peg_400;
                    }
                }
                result_peg_398.setValue(Value((void*) "y"));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist y");
                    result_peg_398.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_398;
        stream.update(result_peg_398.getPosition());
        
        
        return result_peg_398;
        out_peg_400:
        Result result_peg_420(myposition);
        
        result_peg_420 = rule_hitflag(stream, result_peg_420.getPosition());
        if (result_peg_420.error()){
            goto out_peg_421;
        }
        
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = result_peg_420;
        stream.update(result_peg_420.getPosition());
        
        
        return result_peg_420;
        out_peg_421:
    
        if (column_peg_1.chunk14 == 0){
            column_peg_1.chunk14 = new Chunk14();
        }
        column_peg_1.chunk14->chunk_keyword_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

static const void * doParse(Stream & stream, bool stats, const std::string & context){
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::ostringstream out;
        out << "Error while parsing " << context << " " << stream.reportError();
        throw ParseException(out.str());
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}

const void * parse(const std::string & filename, bool stats = false){
    Stream stream(filename);
    return doParse(stream, stats, filename);
}

const void * parse(const char * in, bool stats = false){
    Stream stream(in);
    return doParse(stream, stats, "memory");
}

const void * parse(const char * in, int length, bool stats = false){
    Stream stream(in, length);
    return doParse(stream, stats, "memory");
}



    
    } /* Mugen */
    
} /* Cmd */

        
