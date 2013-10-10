

#include "mugen/ast/all.h"
#include <map>
#include "gc.h"
// #include "cmd-functions.h"
typedef std::list<Ast::Section*> SectionList;


#include <stdint.h>
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

    void setPosition(int position){
        this->position = position;
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
    Result chunk_range;
    Result chunk_name;
};

struct Chunk3{
Result chunk_valuelist;
    Result chunk_expr;
    Result chunk_expr_c;
    Result chunk_expr2;
    Result chunk_expr3;
};

struct Chunk4{
Result chunk_expr4;
    Result chunk_expr5;
    Result chunk_expr6;
    Result chunk_expr7;
    Result chunk_expr8;
};

struct Chunk5{
Result chunk_expr9;
    Result chunk_expr10;
    Result chunk_expr11;
    Result chunk_expr12;
    Result chunk_expr13;
};

struct Chunk6{
Result chunk_expr_rest;
    Result chunk_expr2_rest;
    Result chunk_expr3_rest;
    Result chunk_expr4_rest;
    Result chunk_expr5_rest;
};

struct Chunk7{
Result chunk_expr6_rest;
    Result chunk_expr7_rest;
    Result chunk_expr8_rest;
    Result chunk_expr9_rest;
    Result chunk_all_compare;
};

struct Chunk8{
Result chunk_expr10_rest;
    Result chunk_expr11_rest;
    Result chunk_expr12_rest;
    Result chunk_expr13_real;
    Result chunk_function;
};

struct Chunk9{
Result chunk_paren_integer;
    Result chunk_hitdef__attack__attribute;
    Result chunk_hitdef__attack__attribute__item;
    Result chunk_hitdef__attribute;
    Result chunk_args;
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
    Result chunk_value;
    Result chunk_resource;
    Result chunk_resource__s;
};

struct Chunk12{
Result chunk_resource__f;
    Result chunk_helper;
    Result chunk_helper__expression;
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
        return 0;
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
    line(-1), column(-1),
    message(reason){
    }

    ParseException(const std::string & reason, int line, int column):
    std::exception(),
    line(line), column(column),
    message(reason){
    }

    std::string getReason() const;
    int getLine() const;
    int getColumn() const;

    virtual ~ParseException() throw(){
    }

protected:
    int line, column;
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

        setup();
    }

    /* for null-terminated strings */
    Stream(const char * in):
    temp(0),
    buffer(in),
    farthest(0),
    last_line_info(-1){
        max = strlen(buffer);
        line_info[-1] = LineInfo(1, 1);
        setup();
    }

    /* user-defined length */
    Stream(const char * in, int length):
    temp(0),
    buffer(in),
    farthest(0),
    last_line_info(-1){
        max = length;
        line_info[-1] = LineInfo(1, 1);
        setup();
    }

    void setup(){
        
        createMemo();
    }

    void createMemo(){
        memo_size = 1024 * 2;
        memo = new Column*[memo_size];
        /* dont create column objects before they are needed because transient
         * productions will never call for them so we can save some space by
         * not allocating columns at all.
         */
        memset(memo, 0, sizeof(Column*) * memo_size);
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
        /* Copy old memo table */
        memcpy(newMemo, memo, sizeof(Column*) * memo_size);

        /* Zero out new entries */
        memset(&newMemo[memo_size], 0, sizeof(Column*) * (newSize - memo_size));

        /* Delete old memo table */
        delete[] memo;

        /* Set up new memo table */
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

    /* throws a ParseException */
    void reportError(const std::string & parsingContext){
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
        out << "Error while parsing " << parsingContext << ". Read up till line " << line << " column " << column << std::endl;
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
        out << makeBacktrace(last_trace) << std::endl;
        throw ParseException(out.str(), line, column);
    }

    std::string makeBacktrace(){
        return makeBacktrace(rule_backtrace);
    }

    std::string makeBacktrace(const std::vector<std::string> & trace){
        std::ostringstream out;

        bool first = true;
        for (std::vector<std::string>::const_iterator it = trace.begin(); it != trace.end(); it++){
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
        /* create columns lazily because not every position will have a column. */
        if (memo[position] == NULL){
            memo[position] = new Column();
        }
        return *(memo[position]);
    }

    void update(const int position){
        if (position > farthest){
            farthest = position;
            last_trace = rule_backtrace;
        }
    }

    void push_rule(const std::string & name){
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
    /* an array is faster and uses less memory than std::map */
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

    void setName(const std::string & name){
        stream.pop_rule();
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

int ParseException::getLine() const {
    return line;
}

int ParseException::getColumn() const {
    return column;
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
Result rule_range(Stream &, const int);
Result rule_name(Stream &, const int);
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
Result rule_all_compare(Stream &, const int);
Result rule_expr10_rest(Stream &, const int, Value left);
Result rule_expr11_rest(Stream &, const int, Value left);
Result rule_expr12_rest(Stream &, const int, Value left);
Result rule_expr13_real(Stream &, const int);
Result rule_function(Stream &, const int);
Result rule_paren_integer(Stream &, const int);
Result rule_hitdef__attack__attribute(Stream &, const int);
Result rule_hitdef__attack__attribute__item(Stream &, const int, Value attribute);
Result rule_hitdef__attribute(Stream &, const int);
Result rule_args(Stream &, const int);
Result rule_function_rest(Stream &, const int);
Result rule_keys(Stream &, const int);
Result rule_key_value_list(Stream &, const int);
Result rule_key(Stream &, const int);
Result rule_key_rest(Stream &, const int, Value left);
Result rule_key_real(Stream &, const int);
Result rule_key_modifier(Stream &, const int);
Result rule_value(Stream &, const int);
Result rule_resource(Stream &, const int);
Result rule_resource__s(Stream &, const int);
Result rule_resource__f(Stream &, const int);
Result rule_helper(Stream &, const int);
Result rule_helper__expression(Stream &, const int);
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

Ast::Value * makeInteger(int line, int column, const Value & sign, const Value & digits){
    std::istringstream in(*toString(digits));
    double value = 0;
    in >> value;

    if (sign.getValue() != 0){
        value = -value;
    }
    
    Ast::Number * object = new Ast::Number(line, column, value);
    GC::save(object);
    return object;
}

std::string * toString(char front, const Value & input){
  std::string * str = toString(input);
  str->insert(str->begin(), front);
  return str;
}

Ast::String * makeString(const Value & value){
    /* FIXME: fix line numbers here */
    Ast::String * object = new Ast::String(-1, -1, toString(value));
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

Ast::Value * makeHelper(const Value & name, const Value & expression, const Value & id){
    /* FIXME: fix line numbers here */
    Ast::Value * helper = new Ast::Helper(-1, -1, std::string(as<const char*>(name)), as<Ast::Value*>(expression), as<Ast::Value*>(id));
    GC::save(helper);
    return helper;
}

Ast::Value * makeKeyword(const char * name){
    /* FIXME: fix line numbers here */
    Ast::Value * keyword = new Ast::Keyword(-1, -1, name);
    GC::save(keyword);
    return keyword;
}

Ast::Value * makeExpressionInfix(Ast::ExpressionInfix::InfixType type, const Value & left, const Value & right){
    /* FIXME: fix line numbers here */
    Ast::Value * object = new Ast::ExpressionInfix(-1, -1, type, as<Ast::Value*>(left), as<Ast::Value*>(right));
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
    /* FIXME: fix line numbers here */
    Ast::Value * negation = new Ast::ExpressionUnary(-1, -1, Ast::ExpressionUnary::Negation, expression);
    GC::save(negation);
    return negation;
}

Ast::Value * makeUnaryExpression(const Value & unaries, const Value & exp){
    Ast::Value * expression = as<Ast::Value*>(exp);
    for (Value::iterator it = unaries.getValues().begin(); it != unaries.getValues().end(); it++){
        Ast::ExpressionUnary::UnaryType unary = (Ast::ExpressionUnary::UnaryType) (long) (*it).getValue();
        /* FIXME: fix line numbers here */
        expression = new Ast::ExpressionUnary(-1, -1, unary, expression);
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

Ast::Value * makeValueList2(const Value & first, const Value & second){
    std::list<Ast::Value*> values;
    values.push_back(as<Ast::Value*>(first));
    values.push_back(as<Ast::Value*>(second));
    Ast::ValueList * object = new Ast::ValueList(values);
    GC::save(object);
    return object;
}

Ast::Value * makeValueList3(const Value & first, const Value & second, const Value & third){
    std::list<Ast::Value*> values;
    values.push_back(as<Ast::Value*>(first));
    values.push_back(as<Ast::Value*>(second));
    values.push_back(as<Ast::Value*>(third));
    Ast::ValueList * object = new Ast::ValueList(values);
    GC::save(object);
    return object;
}

Ast::Value * makeResource(const Value & value, bool fight, bool own){
    /* FIXME: line numbers */
    Ast::Resource * resource = new Ast::Resource(-1, -1, as<Ast::Value*>(value), fight, own);
    GC::save(resource);
    return resource;
}

Ast::Value * makeOwnResource(const Value & number){
    return makeResource(number, false, true);
}
    
Ast::Value * makeFightResource(const Value & number){
    return makeResource(number, true, false);
}

Ast::Value * makeFunction(const Value & name, const Value & arg1){
    /* FIXME! fix line numbers here */
    Ast::Value * function = new Ast::Function(-1, -1, std::string(as<const char*>(name)), as<Ast::ValueList*>(arg1));
    GC::save(function);
    return function;
}

Ast::Value * makeFunction(int line, int column, const Value & name, const Value & arg1){
    Ast::Value * function = new Ast::Function(line, column, std::string(as<const char*>(name)), as<Ast::ValueList*>(arg1));
    GC::save(function);
    return function;
}

Ast::Value * makeFunction(const std::string & name, const Value & arg1){
    /* FIXME: fix line numbers here */
    Ast::Value * function = new Ast::Function(-1, -1, name, as<Ast::ValueList*>(arg1));
    GC::save(function);
    return function;
}

Ast::Value * makeFunction1(const Value & name, const Value & arg1){
    return makeFunction(name, Value(makeValueList(arg1, Value())));
}

Ast::Value * makeFunction1(int line, int column, const Value & name, const Value & arg1){
    return makeFunction(line, column, name, Value(makeValueList(arg1, Value())));
}

Ast::Value * makeFunction2(int line, int column, const Value & name, const Value & arg1, const Value & arg2){
    return makeFunction(line, column, name, Value(makeValueList2(arg1, arg2)));
}

Ast::Value * makeFunction3(int line, int column, const Value & name, const Value & arg1, const Value & arg2, const Value & arg3){
    return makeFunction(line, column, name, Value(makeValueList3(arg1, arg2, arg3)));
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
    /* FIXME: fix line numbers here */
    Ast::Value * range = new Ast::Range(-1, -1, type, as<Ast::Value*>(low), as<Ast::Value*>(high));
    GC::save(range);
    return range;
}

Ast::Identifier * makeIdentifier(int line, int column, const char * name){
    Ast::Identifier * object = new Ast::SimpleIdentifier(line, column, std::string(name));
    GC::save(object);
    return object;
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
    /* FIXME: fix line numbers here */
    Ast::Attribute * object = new Ast::AttributeArray(-1, -1, as<Ast::Identifier*>(id), as<Ast::Value*>(index), as<Ast::Value*>(data));
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

Ast::Identifier * makeSimpleIdentifier(const std::string & name){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(name);
    GC::save(identifier);
    return identifier;
}
    
Ast::Identifier * makeHitFlags(const Value & letters, const Value & modifiers){
    std::ostringstream out;
    out << *toString(letters);
    if (modifiers.getValue() != NULL){
        out << *toString(modifiers);
    }
    return makeSimpleIdentifier(out.str());
}

double * parseDouble(const Value & value){
    std::string * str = toString(value);
    std::istringstream get(*str);
    double * number = new double;
    *number = 0;
    get >> *number;
    GC::save(number);
    return number;
}

double * parseDouble(const Value & left, const Value & right){
    std::string * str1 = toString(left);
    std::string * str2 = toString(right);
    std::istringstream get(*str1 + "." + *str2);
    double * number = new double;
    *number = 0;
    get >> *number;
    GC::save(number);
    return number;
}

Ast::Identifier * makeSimpleIdentifier(int line, int column, const std::string & str){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(line, column, str);
    GC::save(identifier);
    return identifier;
}

Ast::Attribute * makeAttribute(const char * name, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(makeSimpleIdentifier(-1, -1, std::string(name)), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttribute(int line, int column, const char * name, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(makeSimpleIdentifier(line, column, std::string(name)), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Value * makeNumber(double value){
    /* FIXME: fix line numbers here */
    Ast::Number * object = new Ast::Number(-1, -1, value);
    GC::save(object);
    return object;
}

Ast::Value * makeNumber(const Value & sign, const Value & number){
    double value = *(as<double*>(number));
    if (sign.getValue() != 0 && strcmp(as<const char *>(sign), "-") == 0){
        value = -value;
    }

    /* FIXME: fix line numbers here */
    Ast::Number * object = new Ast::Number(-1, -1, value);
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

Ast::Section * makeSection(const Value & str, int line, int column){
    Ast::Section * object = new Ast::Section(as<std::string*>(str), line, column);
    GC::save(object);
    return object;
}

Ast::Key * makeKeyModifier(Ast::Key * in, Ast::KeyModifier::ModifierType type, int ticks){
    /* FIXME: fix line numbers here */
    Ast::Key * modded = new Ast::KeyModifier(-1, -1, type, in, ticks);
    GC::save(modded);
    return modded;
}

Ast::Key * makeKeyCombined(const Value & left, const Value & right){
    /* FIXME: fix line numbers here */
    Ast::Key * key = new Ast::KeyCombined(-1, -1, as<Ast::Key*>(left), as<Ast::Key*>(right));
    GC::save(key);
    return key;
}

/* for commands with no keys listed */
Ast::Key * makeEmptyKeyList(){
    std::vector<Ast::Key*> all;
    /* FIXME: fix line numbers here */
    Ast::Key * object = new Ast::KeyList(-1, -1, all);
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

    /* FIXME: fix line numbers here */
    Ast::Key * object = new Ast::KeyList(-1, -1, all);
    GC::save(object);
    return object;
}

Ast::Key * makeKey(const Value & value){
    /* FIXME: fix line numbers here */
    Ast::Key * key = new Ast::KeySingle(-1, -1, as<const char *>(value));
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
    /* FIXME: fix line numbers here */
    Ast::HitDefAttackAttribute * object = new Ast::HitDefAttackAttribute(-1, -1);
    GC::save(object);
    return object;
}

Ast::HitDefAttribute * makeHitDefAttribute(const Value & input){
    std::ostringstream out;
    for (Value::iterator it = input.getValues().begin(); it != input.getValues().end(); it++){
        out << (char*) (*it).getValue();
    }
    /* FIXME: fix line numbers here */
    Ast::HitDefAttribute * object = new Ast::HitDefAttribute(-1, -1, out.str());
    GC::save(object);
    return object;
}

Ast::Value * makeAnimElem(const Value & line, const Value & arg1, const Value & comparison, const Value & arg2){
    /* animelemno == arg1 && animelemtime(arg1) compare arg2 */
#if 0
    Ast::Value * element;
    Ast::Value * time;
    element = makeExpressionEquals(Value(makeSimpleIdentifier(getCurrentLine(line), getCurrentColumn(line), "animelemno")), arg1);
    /* make a copy of arg1 because values can only have one parent */
    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
    time = as<compare_func>(comparison)(Value(makeFunction1(std::string("animelemtime"), Value(copyValue(arg1)))), arg2);
    return makeExpressionAnd(Value(element), Value(time));
#endif
    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
    return as<compare_func>(comparison)(Value(makeFunction1(std::string("animelemtime"), Value(copyValue(arg1)))), arg2);
}

Ast::Value * makeAnimElem(const Value & line, const Value & arg1){
    Value comparison = Value((void*) makeExpressionEquals);
    Value arg2 = Value(makeNumber(0));
    return makeAnimElem(line, arg1, comparison, arg2);
}

Ast::Value * makeArgument(int line, int column, int index){
  Ast::Value * argument = new Ast::Argument(line, column, index);
  GC::save(argument);
  return argument;
}

Ast::Value * makeProjectileExpression(const Value & line, const Value & name, const Value & id, const Value & arg1, const Value & compare, const Value & ticks){
    Value finalId;
    if (id.getValue() != NULL){
        finalId = id;
    } else {
        finalId = makeNumber(0);
    }

    Ast::Value * comparison = NULL;

    /* comparison will be an infix expression where the left node is an Argument ast class.
     * The runtime evaluator will fill in the Environment with a value for arg0 so that
     * when the Argument is evaluated it will pull out arg0.
     *
     * For example, for projhit arg0 will be projectile->lastHit - stage->ticks
     */

    /* check if the second form is used */
    if (compare.getValue() != NULL){
        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
        Ast::Value * argument = makeArgument(getCurrentLine(line), getCurrentColumn(line), 0);
        comparison = as<compare_func>(compare)(Value(argument), ticks);
    } else {
        Ast::Value * one = makeNumber(1);
        Ast::Value * argument = makeArgument(getCurrentLine(line), getCurrentColumn(line), 0);
        comparison = makeExpressionLessThanEquals(Value(argument), Value(one));
    }
    
    Ast::Value * get = makeFunction3(getCurrentLine(line), getCurrentColumn(line), name, finalId, arg1, Value(comparison));
    return get;
}




Result rule_start(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk0 != 0 && column_peg_2.chunk0->chunk_start.calculated()){
        return column_peg_2.chunk0->chunk_start;
    }
    
    
    RuleTrace trace_peg_1(stream, "start");
    int myposition = position;
    
    try{
        
    
    Value current;
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    Value value((void*) 0);
                    value = makeSectionList();
                    result_peg_3.setValue(value);
                }
                current = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_whitespace(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_6;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_9(result_peg_3.getPosition());
                    {
                        int position_peg_11 = result_peg_9.getPosition();
                        
                        result_peg_9.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_9.getPosition()))){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_11);
                                goto out_peg_12;
                            }
                        }
                            
                    }
                    goto success_peg_10;
                    out_peg_12:
                    {
                        int position_peg_13 = result_peg_9.getPosition();
                        
                        result_peg_9.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_9.getPosition()))){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_13);
                                goto out_peg_14;
                            }
                        }
                            
                    }
                    goto success_peg_10;
                    out_peg_14:
                    goto loop_peg_8;
                    success_peg_10:
                    ;
                    result_peg_3.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                ;
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_17(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_20 = result_peg_17.getPosition();
                                
                                result_peg_17.reset();
                                do{
                                    Result result_peg_22(result_peg_17.getPosition());
                                    {
                                        int position_peg_24 = result_peg_22.getPosition();
                                        
                                        {
                                            int position_peg_26 = result_peg_22.getPosition();
                                            
                                            result_peg_22.setValue(Value((void*) " "));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar(" "[i], stream.get(result_peg_22.getPosition()))){
                                                    result_peg_22.nextPosition();
                                                } else {
                                                    result_peg_22.setPosition(position_peg_26);
                                                    goto out_peg_27;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_25;
                                        out_peg_27:
                                        {
                                            int position_peg_28 = result_peg_22.getPosition();
                                            
                                            result_peg_22.setValue(Value((void*) "\t"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\t"[i], stream.get(result_peg_22.getPosition()))){
                                                    result_peg_22.nextPosition();
                                                } else {
                                                    result_peg_22.setPosition(position_peg_28);
                                                    goto out_peg_29;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_25;
                                        out_peg_29:
                                        result_peg_22.setPosition(position_peg_24);
                                        goto out_peg_30;
                                        success_peg_25:
                                        ;
                                        
                                    }
                                    goto success_peg_23;
                                    out_peg_30:
                                    {
                                        int position_peg_31 = result_peg_22.getPosition();
                                        
                                        {
                                            int position_peg_33 = result_peg_22.getPosition();
                                            
                                            result_peg_22.setValue(Value((void*) "\n"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_22.getPosition()))){
                                                    result_peg_22.nextPosition();
                                                } else {
                                                    result_peg_22.setPosition(position_peg_33);
                                                    goto out_peg_34;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_32;
                                        out_peg_34:
                                        {
                                            int position_peg_35 = result_peg_22.getPosition();
                                            
                                            result_peg_22.setValue(Value((void*) "\r"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_22.getPosition()))){
                                                    result_peg_22.nextPosition();
                                                } else {
                                                    result_peg_22.setPosition(position_peg_35);
                                                    goto out_peg_36;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_32;
                                        out_peg_36:
                                        result_peg_22.setPosition(position_peg_31);
                                        goto out_peg_37;
                                        success_peg_32:
                                        ;
                                        
                                    }
                                    goto success_peg_23;
                                    out_peg_37:
                                    goto loop_peg_21;
                                    success_peg_23:
                                    ;
                                    result_peg_17.addResult(result_peg_22);
                                } while (true);
                                loop_peg_21:
                                ;
                                        
                            }
                            goto success_peg_19;
                            goto loop_peg_16;
                            success_peg_19:
                            ;
                        
                        
                        
                        result_peg_17 = rule_line(stream, result_peg_17.getPosition(), current);
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                        
                        result_peg_17 = rule_whitespace(stream, result_peg_17.getPosition());
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                        
                        int save_peg_40 = result_peg_17.getPosition();
                            
                            result_peg_17 = rule_line_end(stream, result_peg_17.getPosition());
                            if (result_peg_17.error()){
                                
                                result_peg_17 = Result(save_peg_40);
                                result_peg_17.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_17);
                } while (true);
                loop_peg_16:
                ;
            
            
            
            {
                    int position_peg_43 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_45(result_peg_3.getPosition());
                        {
                            int position_peg_47 = result_peg_45.getPosition();
                            
                            {
                                int position_peg_49 = result_peg_45.getPosition();
                                
                                result_peg_45.setValue(Value((void*) " "));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(" "[i], stream.get(result_peg_45.getPosition()))){
                                        result_peg_45.nextPosition();
                                    } else {
                                        result_peg_45.setPosition(position_peg_49);
                                        goto out_peg_50;
                                    }
                                }
                                    
                            }
                            goto success_peg_48;
                            out_peg_50:
                            {
                                int position_peg_51 = result_peg_45.getPosition();
                                
                                result_peg_45.setValue(Value((void*) "\t"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\t"[i], stream.get(result_peg_45.getPosition()))){
                                        result_peg_45.nextPosition();
                                    } else {
                                        result_peg_45.setPosition(position_peg_51);
                                        goto out_peg_52;
                                    }
                                }
                                    
                            }
                            goto success_peg_48;
                            out_peg_52:
                            result_peg_45.setPosition(position_peg_47);
                            goto out_peg_53;
                            success_peg_48:
                            ;
                            
                        }
                        goto success_peg_46;
                        out_peg_53:
                        {
                            int position_peg_54 = result_peg_45.getPosition();
                            
                            {
                                int position_peg_56 = result_peg_45.getPosition();
                                
                                result_peg_45.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_45.getPosition()))){
                                        result_peg_45.nextPosition();
                                    } else {
                                        result_peg_45.setPosition(position_peg_56);
                                        goto out_peg_57;
                                    }
                                }
                                    
                            }
                            goto success_peg_55;
                            out_peg_57:
                            {
                                int position_peg_58 = result_peg_45.getPosition();
                                
                                result_peg_45.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_45.getPosition()))){
                                        result_peg_45.nextPosition();
                                    } else {
                                        result_peg_45.setPosition(position_peg_58);
                                        goto out_peg_59;
                                    }
                                }
                                    
                            }
                            goto success_peg_55;
                            out_peg_59:
                            result_peg_45.setPosition(position_peg_54);
                            goto out_peg_60;
                            success_peg_55:
                            ;
                            
                        }
                        goto success_peg_46;
                        out_peg_60:
                        goto loop_peg_44;
                        success_peg_46:
                        ;
                        result_peg_3.addResult(result_peg_45);
                    } while (true);
                    loop_peg_44:
                    ;
                            
                }
                goto success_peg_42;
                goto out_peg_6;
                success_peg_42:
                ;
            
            
            
            if ('\0' == stream.get(result_peg_3.getPosition())){
                    result_peg_3.nextPosition();
                    result_peg_3.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_6;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = current; GC::cleanup(as<SectionList*>(value));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_start = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_6:
    
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_start = errorResult;
        stream.update(errorResult.getPosition());
        
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    
    RuleTrace trace_peg_1(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_8(result_peg_3.getPosition());
                        {
                            int position_peg_10 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_10);
                                    goto out_peg_11;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_11:
                        {
                            int position_peg_12 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_13:
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_3.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_14;
                success_peg_5:
                ;
            
            
            
            result_peg_3 = rule_section(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_14;
                }
            
            Result result_peg_15 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_15.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_14:
        Result result_peg_16(myposition);
        
        
        {
        
            result_peg_16.reset();
                do{
                    Result result_peg_19(result_peg_16.getPosition());
                    {
                    
                        Result result_peg_22(result_peg_19);
                            {
                                int position_peg_24 = result_peg_22.getPosition();
                                
                                result_peg_22.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_22.getPosition()))){
                                        result_peg_22.nextPosition();
                                    } else {
                                        result_peg_22.setPosition(position_peg_24);
                                        goto out_peg_25;
                                    }
                                }
                                    
                            }
                            goto success_peg_23;
                            out_peg_25:
                            {
                                int position_peg_26 = result_peg_22.getPosition();
                                
                                result_peg_22.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_22.getPosition()))){
                                        result_peg_22.nextPosition();
                                    } else {
                                        result_peg_22.setPosition(position_peg_26);
                                        goto out_peg_27;
                                    }
                                }
                                    
                            }
                            goto success_peg_23;
                            out_peg_27:
                            goto not_peg_21;
                            success_peg_23:
                            ;
                            goto loop_peg_18;
                            not_peg_21:
                            result_peg_19.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_28 = stream.get(result_peg_19.getPosition());
                            if (temp_peg_28 != '\0'){
                                result_peg_19.setValue(Value((void*) (intptr_t) temp_peg_28));
                                result_peg_19.nextPosition();
                            } else {
                                goto loop_peg_18;
                            }
                        
                        
                    }
                    result_peg_16.addResult(result_peg_19);
                } while (true);
                loop_peg_18:
                ;
            
            
            
            {
                    int position_peg_30 = result_peg_16.getPosition();
                    
                    result_peg_16.setValue(Value((void*) "\n"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_16.getPosition()))){
                            result_peg_16.nextPosition();
                        } else {
                            result_peg_16.setPosition(position_peg_30);
                            goto out_peg_31;
                        }
                    }
                        
                }
                goto success_peg_29;
                out_peg_31:
                {
                    int position_peg_32 = result_peg_16.getPosition();
                    
                    result_peg_16.setValue(Value((void*) "\r"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_16.getPosition()))){
                            result_peg_16.nextPosition();
                        } else {
                            result_peg_16.setPosition(position_peg_32);
                            goto out_peg_33;
                        }
                    }
                        
                }
                goto success_peg_29;
                out_peg_33:
                goto out_peg_34;
                success_peg_29:
                ;
            
            
        }
        
        stream.update(result_peg_16.getPosition());
        
        return result_peg_16;
        out_peg_34:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk0 != 0 && column_peg_2.chunk0->chunk_line_end.calculated()){
        return column_peg_2.chunk0->chunk_line_end;
    }
    
    
    RuleTrace trace_peg_1(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3.reset();
        do{
            Result result_peg_5(result_peg_3.getPosition());
            {
                int position_peg_7 = result_peg_5.getPosition();
                
                result_peg_5.setValue(Value((void*) "\n"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        result_peg_5.setPosition(position_peg_7);
                        goto out_peg_8;
                    }
                }
                    
            }
            goto success_peg_6;
            out_peg_8:
            {
                int position_peg_9 = result_peg_5.getPosition();
                
                result_peg_5.setValue(Value((void*) "\r"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        result_peg_5.setPosition(position_peg_9);
                        goto out_peg_10;
                    }
                }
                    
            }
            goto success_peg_6;
            out_peg_10:
            goto loop_peg_4;
            success_peg_6:
            ;
            result_peg_3.addResult(result_peg_5);
        } while (true);
        loop_peg_4:
        if (result_peg_3.matches() == 0){
            goto out_peg_11;
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_line_end = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_11:
        Result result_peg_12(myposition);
        
        
        {
        
            Result result_peg_14(result_peg_12.getPosition());
                if ('\0' == stream.get(result_peg_14.getPosition())){
                    result_peg_14.nextPosition();
                    result_peg_14.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_15;
                }
            
            
            
            
            
            
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_line_end = result_peg_12;
        stream.update(result_peg_12.getPosition());
        
        
        return result_peg_12;
        out_peg_15:
    
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk0 != 0 && column_peg_2.chunk0->chunk_whitespace.calculated()){
        return column_peg_2.chunk0->chunk_whitespace;
    }
    
    
    RuleTrace trace_peg_1(stream, "whitespace");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3.reset();
        do{
            Result result_peg_5(result_peg_3.getPosition());
            result_peg_5 = rule_sw(stream, result_peg_5.getPosition());
            if (result_peg_5.error()){
                goto loop_peg_4;
            }
            result_peg_3.addResult(result_peg_5);
        } while (true);
        loop_peg_4:
        ;
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_whitespace = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
    
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk0 != 0 && column_peg_2.chunk0->chunk_sw.calculated()){
        return column_peg_2.chunk0->chunk_sw;
    }
    
    
    RuleTrace trace_peg_1(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
            int position_peg_5 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_3.getPosition()))){
                    result_peg_3.nextPosition();
                } else {
                    result_peg_3.setPosition(position_peg_5);
                    goto out_peg_6;
                }
            }
                
        }
        goto success_peg_4;
        out_peg_6:
        {
            int position_peg_7 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_3.getPosition()))){
                    result_peg_3.nextPosition();
                } else {
                    result_peg_3.setPosition(position_peg_7);
                    goto out_peg_8;
                }
            }
                
        }
        goto success_peg_4;
        out_peg_8:
        goto out_peg_9;
        success_peg_4:
        ;
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_sw = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_9:
        Result result_peg_10(myposition);
        
        
        result_peg_10 = rule_comment(stream, result_peg_10.getPosition());
        if (result_peg_10.error()){
            goto out_peg_11;
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_sw = result_peg_10;
        stream.update(result_peg_10.getPosition());
        
        
        return result_peg_10;
        out_peg_11:
    
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_comment.calculated()){
        return column_peg_2.chunk1->chunk_comment;
    }
    
    
    RuleTrace trace_peg_1(stream, "comment");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) ";"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_7(result_peg_3.getPosition());
                    {
                    
                        Result result_peg_10(result_peg_7);
                            {
                                int position_peg_12 = result_peg_10.getPosition();
                                
                                result_peg_10.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_10.getPosition()))){
                                        result_peg_10.nextPosition();
                                    } else {
                                        result_peg_10.setPosition(position_peg_12);
                                        goto out_peg_13;
                                    }
                                }
                                    
                            }
                            goto success_peg_11;
                            out_peg_13:
                            {
                                int position_peg_14 = result_peg_10.getPosition();
                                
                                result_peg_10.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_10.getPosition()))){
                                        result_peg_10.nextPosition();
                                    } else {
                                        result_peg_10.setPosition(position_peg_14);
                                        goto out_peg_15;
                                    }
                                }
                                    
                            }
                            goto success_peg_11;
                            out_peg_15:
                            goto not_peg_9;
                            success_peg_11:
                            ;
                            goto loop_peg_6;
                            not_peg_9:
                            result_peg_7.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_16 = stream.get(result_peg_7.getPosition());
                            if (temp_peg_16 != '\0'){
                                result_peg_7.setValue(Value((void*) (intptr_t) temp_peg_16));
                                result_peg_7.nextPosition();
                            } else {
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_17(myposition);
        
        
        {
        
            result_peg_17.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_17.getPosition()))){
                        result_peg_17.nextPosition();
                    } else {
                        goto out_peg_19;
                    }
                }
            
            
            
            result_peg_17.reset();
                do{
                    Result result_peg_21(result_peg_17.getPosition());
                    {
                    
                        Result result_peg_24(result_peg_21);
                            {
                                int position_peg_26 = result_peg_24.getPosition();
                                
                                result_peg_24.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_24.getPosition()))){
                                        result_peg_24.nextPosition();
                                    } else {
                                        result_peg_24.setPosition(position_peg_26);
                                        goto out_peg_27;
                                    }
                                }
                                    
                            }
                            goto success_peg_25;
                            out_peg_27:
                            {
                                int position_peg_28 = result_peg_24.getPosition();
                                
                                result_peg_24.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_24.getPosition()))){
                                        result_peg_24.nextPosition();
                                    } else {
                                        result_peg_24.setPosition(position_peg_28);
                                        goto out_peg_29;
                                    }
                                }
                                    
                            }
                            goto success_peg_25;
                            out_peg_29:
                            goto not_peg_23;
                            success_peg_25:
                            ;
                            goto loop_peg_20;
                            not_peg_23:
                            result_peg_21.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_30 = stream.get(result_peg_21.getPosition());
                            if (temp_peg_30 != '\0'){
                                result_peg_21.setValue(Value((void*) (intptr_t) temp_peg_30));
                                result_peg_21.nextPosition();
                            } else {
                                goto loop_peg_20;
                            }
                        
                        
                    }
                    result_peg_17.addResult(result_peg_21);
                } while (true);
                loop_peg_20:
                ;
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = result_peg_17;
        stream.update(result_peg_17.getPosition());
        
        
        return result_peg_17;
        out_peg_19:
        Result result_peg_31(myposition);
        
        
        {
        
            result_peg_31.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_31.getPosition()))){
                        result_peg_31.nextPosition();
                    } else {
                        goto out_peg_33;
                    }
                }
            
            
            
            result_peg_31.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_31.getPosition()))){
                        result_peg_31.nextPosition();
                    } else {
                        goto out_peg_33;
                    }
                }
            
            
            
            result_peg_31.reset();
                do{
                    Result result_peg_36(result_peg_31.getPosition());
                    {
                    
                        Result result_peg_39(result_peg_36);
                            {
                                int position_peg_41 = result_peg_39.getPosition();
                                
                                result_peg_39.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_39.getPosition()))){
                                        result_peg_39.nextPosition();
                                    } else {
                                        result_peg_39.setPosition(position_peg_41);
                                        goto out_peg_42;
                                    }
                                }
                                    
                            }
                            goto success_peg_40;
                            out_peg_42:
                            {
                                int position_peg_43 = result_peg_39.getPosition();
                                
                                result_peg_39.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_39.getPosition()))){
                                        result_peg_39.nextPosition();
                                    } else {
                                        result_peg_39.setPosition(position_peg_43);
                                        goto out_peg_44;
                                    }
                                }
                                    
                            }
                            goto success_peg_40;
                            out_peg_44:
                            goto not_peg_38;
                            success_peg_40:
                            ;
                            goto loop_peg_35;
                            not_peg_38:
                            result_peg_36.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_45 = stream.get(result_peg_36.getPosition());
                            if (temp_peg_45 != '\0'){
                                result_peg_36.setValue(Value((void*) (intptr_t) temp_peg_45));
                                result_peg_36.nextPosition();
                            } else {
                                goto loop_peg_35;
                            }
                        
                        
                    }
                    result_peg_31.addResult(result_peg_36);
                } while (true);
                loop_peg_35:
                ;
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = result_peg_31;
        stream.update(result_peg_31.getPosition());
        
        
        return result_peg_31;
        out_peg_33:
        Result result_peg_46(myposition);
        
        
        {
        
            result_peg_46.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_46.getPosition()))){
                        result_peg_46.nextPosition();
                    } else {
                        goto out_peg_48;
                    }
                }
            
            
            
            result_peg_46.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_46.getPosition()))){
                        result_peg_46.nextPosition();
                    } else {
                        goto out_peg_48;
                    }
                }
            
            
            
            result_peg_46.reset();
                do{
                    Result result_peg_51(result_peg_46.getPosition());
                    {
                    
                        Result result_peg_54(result_peg_51);
                            {
                                int position_peg_56 = result_peg_54.getPosition();
                                
                                result_peg_54.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_54.getPosition()))){
                                        result_peg_54.nextPosition();
                                    } else {
                                        result_peg_54.setPosition(position_peg_56);
                                        goto out_peg_57;
                                    }
                                }
                                    
                            }
                            goto success_peg_55;
                            out_peg_57:
                            {
                                int position_peg_58 = result_peg_54.getPosition();
                                
                                result_peg_54.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_54.getPosition()))){
                                        result_peg_54.nextPosition();
                                    } else {
                                        result_peg_54.setPosition(position_peg_58);
                                        goto out_peg_59;
                                    }
                                }
                                    
                            }
                            goto success_peg_55;
                            out_peg_59:
                            goto not_peg_53;
                            success_peg_55:
                            ;
                            goto loop_peg_50;
                            not_peg_53:
                            result_peg_51.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_60 = stream.get(result_peg_51.getPosition());
                            if (temp_peg_60 != '\0'){
                                result_peg_51.setValue(Value((void*) (intptr_t) temp_peg_60));
                                result_peg_51.nextPosition();
                            } else {
                                goto loop_peg_50;
                            }
                        
                        
                    }
                    result_peg_46.addResult(result_peg_51);
                } while (true);
                loop_peg_50:
                ;
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = result_peg_46;
        stream.update(result_peg_46.getPosition());
        
        
        return result_peg_46;
        out_peg_48:
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_section.calculated()){
        return column_peg_2.chunk1->chunk_section;
    }
    
    
    RuleTrace trace_peg_1(stream, "section");
    int myposition = position;
    
    
    Value line;
        Value name;
        Value ast;
    Result result_peg_3(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_5 = stream.getLineInfo(result_peg_3.getPosition());
                line = &line_info_peg_5;
            
            
            
            result_peg_3 = rule_section_title(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_7;
                }
                name = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSection(name, getCurrentLine(line), getCurrentColumn(line));
                    result_peg_3.setValue(value);
                }
                ast = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_whitespace(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_7;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_12(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_15 = result_peg_12.getPosition();
                                
                                result_peg_12.reset();
                                do{
                                    Result result_peg_17(result_peg_12.getPosition());
                                    {
                                        int position_peg_19 = result_peg_17.getPosition();
                                        
                                        {
                                            int position_peg_21 = result_peg_17.getPosition();
                                            
                                            result_peg_17.setValue(Value((void*) " "));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar(" "[i], stream.get(result_peg_17.getPosition()))){
                                                    result_peg_17.nextPosition();
                                                } else {
                                                    result_peg_17.setPosition(position_peg_21);
                                                    goto out_peg_22;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_20;
                                        out_peg_22:
                                        {
                                            int position_peg_23 = result_peg_17.getPosition();
                                            
                                            result_peg_17.setValue(Value((void*) "\t"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\t"[i], stream.get(result_peg_17.getPosition()))){
                                                    result_peg_17.nextPosition();
                                                } else {
                                                    result_peg_17.setPosition(position_peg_23);
                                                    goto out_peg_24;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_20;
                                        out_peg_24:
                                        result_peg_17.setPosition(position_peg_19);
                                        goto out_peg_25;
                                        success_peg_20:
                                        ;
                                        
                                    }
                                    goto success_peg_18;
                                    out_peg_25:
                                    {
                                        int position_peg_26 = result_peg_17.getPosition();
                                        
                                        {
                                            int position_peg_28 = result_peg_17.getPosition();
                                            
                                            result_peg_17.setValue(Value((void*) "\n"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_17.getPosition()))){
                                                    result_peg_17.nextPosition();
                                                } else {
                                                    result_peg_17.setPosition(position_peg_28);
                                                    goto out_peg_29;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_27;
                                        out_peg_29:
                                        {
                                            int position_peg_30 = result_peg_17.getPosition();
                                            
                                            result_peg_17.setValue(Value((void*) "\r"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_17.getPosition()))){
                                                    result_peg_17.nextPosition();
                                                } else {
                                                    result_peg_17.setPosition(position_peg_30);
                                                    goto out_peg_31;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_27;
                                        out_peg_31:
                                        result_peg_17.setPosition(position_peg_26);
                                        goto out_peg_32;
                                        success_peg_27:
                                        ;
                                        
                                    }
                                    goto success_peg_18;
                                    out_peg_32:
                                    goto loop_peg_16;
                                    success_peg_18:
                                    ;
                                    result_peg_12.addResult(result_peg_17);
                                } while (true);
                                loop_peg_16:
                                ;
                                        
                            }
                            goto success_peg_14;
                            goto loop_peg_11;
                            success_peg_14:
                            ;
                        
                        
                        
                        result_peg_12 = rule_section_item(stream, result_peg_12.getPosition(), ast);
                            if (result_peg_12.error()){
                                goto loop_peg_11;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_section = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_7:
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_section = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    
    RuleTrace trace_peg_1(stream, "section_item");
    int myposition = position;
    
    
    Value data;
        Value line;
        Value name;
        Value fail;
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_8(result_peg_3.getPosition());
                        {
                            int position_peg_10 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_10);
                                    goto out_peg_11;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_11:
                        {
                            int position_peg_12 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_13:
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_3.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_14;
                success_peg_5:
                ;
            
            
            
            result_peg_3 = rule_assignment(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_14;
                }
                data = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_whitespace(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_14;
                }
            
            
            
            result_peg_3 = rule_line_end(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_14;
                }
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_14:
        Result result_peg_18(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_20 = stream.getLineInfo(result_peg_18.getPosition());
                line = &line_info_peg_20;
            
            
            
            result_peg_18 = rule_identifier(stream, result_peg_18.getPosition());
                if (result_peg_18.error()){
                    goto out_peg_22;
                }
                name = result_peg_18.getValues();
            
            
            
            {
                    int position_peg_25 = result_peg_18.getPosition();
                    
                    result_peg_18.reset();
                    do{
                        Result result_peg_27(result_peg_18.getPosition());
                        {
                            int position_peg_29 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_29);
                                    goto out_peg_30;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_30:
                        {
                            int position_peg_31 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_31);
                                    goto out_peg_32;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_32:
                        goto loop_peg_26;
                        success_peg_28:
                        ;
                        result_peg_18.addResult(result_peg_27);
                    } while (true);
                    loop_peg_26:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_22;
                success_peg_24:
                ;
            
            
            
            result_peg_18.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_18.getPosition()))){
                        result_peg_18.nextPosition();
                    } else {
                        goto out_peg_22;
                    }
                }
            
            
            
            result_peg_18.reset();
                do{
                    Result result_peg_36(result_peg_18.getPosition());
                    {
                    
                        Result result_peg_39(result_peg_36);
                            {
                                int position_peg_41 = result_peg_39.getPosition();
                                
                                result_peg_39.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_39.getPosition()))){
                                        result_peg_39.nextPosition();
                                    } else {
                                        result_peg_39.setPosition(position_peg_41);
                                        goto out_peg_42;
                                    }
                                }
                                    
                            }
                            goto success_peg_40;
                            out_peg_42:
                            {
                                int position_peg_43 = result_peg_39.getPosition();
                                
                                result_peg_39.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_39.getPosition()))){
                                        result_peg_39.nextPosition();
                                    } else {
                                        result_peg_39.setPosition(position_peg_43);
                                        goto out_peg_44;
                                    }
                                }
                                    
                            }
                            goto success_peg_40;
                            out_peg_44:
                            goto not_peg_38;
                            success_peg_40:
                            ;
                            goto loop_peg_35;
                            not_peg_38:
                            result_peg_36.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_45 = stream.get(result_peg_36.getPosition());
                            if (temp_peg_45 != '\0'){
                                result_peg_36.setValue(Value((void*) (intptr_t) temp_peg_45));
                                result_peg_36.nextPosition();
                            } else {
                                goto loop_peg_35;
                            }
                        
                        
                    }
                    result_peg_18.addResult(result_peg_36);
                } while (true);
                loop_peg_35:
                ;
                fail = result_peg_18.getValues();
            
            
            
            result_peg_18 = rule_line_end(stream, result_peg_18.getPosition());
                if (result_peg_18.error()){
                    goto out_peg_22;
                }
            
            
            
            {
                    Value value((void*) 0);
                    std::cout << "Warning: invalid line at " << getCurrentLine(line) << " '" << as<Ast::Identifier*>(name)->toString() << " =" << *toString(fail) << "'" << std::endl;
                    result_peg_18.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_18.getPosition());
        
        return result_peg_18;
        out_peg_22:
        Result result_peg_47(myposition);
        
        
        {
        
            Result result_peg_50(result_peg_47);
                {
                
                    {
                            int position_peg_53 = result_peg_50.getPosition();
                            
                            result_peg_50.reset();
                            do{
                                Result result_peg_55(result_peg_50.getPosition());
                                {
                                    int position_peg_57 = result_peg_55.getPosition();
                                    
                                    result_peg_55.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_55.getPosition()))){
                                            result_peg_55.nextPosition();
                                        } else {
                                            result_peg_55.setPosition(position_peg_57);
                                            goto out_peg_58;
                                        }
                                    }
                                        
                                }
                                goto success_peg_56;
                                out_peg_58:
                                {
                                    int position_peg_59 = result_peg_55.getPosition();
                                    
                                    result_peg_55.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_55.getPosition()))){
                                            result_peg_55.nextPosition();
                                        } else {
                                            result_peg_55.setPosition(position_peg_59);
                                            goto out_peg_60;
                                        }
                                    }
                                        
                                }
                                goto success_peg_56;
                                out_peg_60:
                                goto loop_peg_54;
                                success_peg_56:
                                ;
                                result_peg_50.addResult(result_peg_55);
                            } while (true);
                            loop_peg_54:
                            ;
                                    
                        }
                        goto success_peg_52;
                        goto not_peg_49;
                        success_peg_52:
                        ;
                    
                    
                    
                    result_peg_50 = rule_section_title(stream, result_peg_50.getPosition());
                        if (result_peg_50.error()){
                            goto not_peg_49;
                        }
                    
                    
                }
                goto out_peg_61;
                not_peg_49:
                result_peg_47.setValue(Value((void*)0));
            
            
            
            result_peg_47.reset();
                do{
                    Result result_peg_64(result_peg_47.getPosition());
                    {
                    
                        Result result_peg_67(result_peg_64);
                            {
                                int position_peg_69 = result_peg_67.getPosition();
                                
                                result_peg_67.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_67.getPosition()))){
                                        result_peg_67.nextPosition();
                                    } else {
                                        result_peg_67.setPosition(position_peg_69);
                                        goto out_peg_70;
                                    }
                                }
                                    
                            }
                            goto success_peg_68;
                            out_peg_70:
                            {
                                int position_peg_71 = result_peg_67.getPosition();
                                
                                result_peg_67.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_67.getPosition()))){
                                        result_peg_67.nextPosition();
                                    } else {
                                        result_peg_67.setPosition(position_peg_71);
                                        goto out_peg_72;
                                    }
                                }
                                    
                            }
                            goto success_peg_68;
                            out_peg_72:
                            goto not_peg_66;
                            success_peg_68:
                            ;
                            goto loop_peg_63;
                            not_peg_66:
                            result_peg_64.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_73 = stream.get(result_peg_64.getPosition());
                            if (temp_peg_73 != '\0'){
                                result_peg_64.setValue(Value((void*) (intptr_t) temp_peg_73));
                                result_peg_64.nextPosition();
                            } else {
                                goto loop_peg_63;
                            }
                        
                        
                    }
                    result_peg_47.addResult(result_peg_64);
                } while (true);
                loop_peg_63:
                if (result_peg_47.matches() == 0){
                    goto out_peg_61;
                }
            
            
            
            result_peg_47 = rule_line_end(stream, result_peg_47.getPosition());
                if (result_peg_47.error()){
                    goto out_peg_61;
                }
            
            
        }
        
        stream.update(result_peg_47.getPosition());
        
        return result_peg_47;
        out_peg_61:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_section_title.calculated()){
        return column_peg_2.chunk1->chunk_section_title;
    }
    
    
    RuleTrace trace_peg_1(stream, "section_title");
    int myposition = position;
    
    
    Value name;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_18(result_peg_3.getPosition());
                    {
                    
                        Result result_peg_21(result_peg_18);
                            result_peg_21.setValue(Value((void*) "]"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    goto not_peg_20;
                                }
                            }
                            goto loop_peg_17;
                            not_peg_20:
                            result_peg_18.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_22 = stream.get(result_peg_18.getPosition());
                            if (temp_peg_22 != '\0'){
                                result_peg_18.setValue(Value((void*) (intptr_t) temp_peg_22));
                                result_peg_18.nextPosition();
                            } else {
                                goto loop_peg_17;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_18);
                } while (true);
                loop_peg_17:
                if (result_peg_3.matches() == 0){
                    goto out_peg_5;
                }
                name = result_peg_3.getValues();
            
            
            
            result_peg_3.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = toString(name);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_section_title = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_section_title = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_assignment.calculated()){
        return column_peg_2.chunk1->chunk_assignment;
    }
    
    
    RuleTrace trace_peg_1(stream, "assignment");
    int myposition = position;
    
    
    Value all;
        Value line;
        Value name;
        Value exp;
        Value index;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "command"));
                for (int i = 0; i < 7; i++){
                    if (compareCharCase("command"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_5;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_keys(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                all = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("command", all);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_assignment = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_28(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_30 = stream.getLineInfo(result_peg_28.getPosition());
                line = &line_info_peg_30;
            
            
            
            result_peg_28 = rule_identifier(stream, result_peg_28.getPosition());
                if (result_peg_28.error()){
                    goto out_peg_32;
                }
                name = result_peg_28.getValues();
            
            
            
            {
                    int position_peg_35 = result_peg_28.getPosition();
                    
                    result_peg_28.reset();
                    do{
                        Result result_peg_37(result_peg_28.getPosition());
                        {
                            int position_peg_39 = result_peg_37.getPosition();
                            
                            result_peg_37.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_37.getPosition()))){
                                    result_peg_37.nextPosition();
                                } else {
                                    result_peg_37.setPosition(position_peg_39);
                                    goto out_peg_40;
                                }
                            }
                                
                        }
                        goto success_peg_38;
                        out_peg_40:
                        {
                            int position_peg_41 = result_peg_37.getPosition();
                            
                            result_peg_37.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_37.getPosition()))){
                                    result_peg_37.nextPosition();
                                } else {
                                    result_peg_37.setPosition(position_peg_41);
                                    goto out_peg_42;
                                }
                            }
                                
                        }
                        goto success_peg_38;
                        out_peg_42:
                        goto loop_peg_36;
                        success_peg_38:
                        ;
                        result_peg_28.addResult(result_peg_37);
                    } while (true);
                    loop_peg_36:
                    ;
                            
                }
                goto success_peg_34;
                goto out_peg_32;
                success_peg_34:
                ;
            
            
            
            result_peg_28.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_32;
                    }
                }
            
            
            
            {
                    int position_peg_46 = result_peg_28.getPosition();
                    
                    result_peg_28.reset();
                    do{
                        Result result_peg_48(result_peg_28.getPosition());
                        {
                            int position_peg_50 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_49;
                        out_peg_51:
                        {
                            int position_peg_52 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_52);
                                    goto out_peg_53;
                                }
                            }
                                
                        }
                        goto success_peg_49;
                        out_peg_53:
                        goto loop_peg_47;
                        success_peg_49:
                        ;
                        result_peg_28.addResult(result_peg_48);
                    } while (true);
                    loop_peg_47:
                    ;
                            
                }
                goto success_peg_45;
                goto out_peg_32;
                success_peg_45:
                ;
            
            
            
            result_peg_28 = rule_expr(stream, result_peg_28.getPosition());
                if (result_peg_28.error()){
                    goto out_peg_32;
                }
                exp = result_peg_28.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
                    result_peg_28.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_assignment = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_32:
        Result result_peg_55(myposition);
        
        
        {
        
            result_peg_55 = rule_identifier(stream, result_peg_55.getPosition());
                if (result_peg_55.error()){
                    goto out_peg_57;
                }
                name = result_peg_55.getValues();
            
            
            
            {
                    int position_peg_60 = result_peg_55.getPosition();
                    
                    result_peg_55.reset();
                    do{
                        Result result_peg_62(result_peg_55.getPosition());
                        {
                            int position_peg_64 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_64);
                                    goto out_peg_65;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_65:
                        {
                            int position_peg_66 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_66);
                                    goto out_peg_67;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_67:
                        goto loop_peg_61;
                        success_peg_63:
                        ;
                        result_peg_55.addResult(result_peg_62);
                    } while (true);
                    loop_peg_61:
                    ;
                            
                }
                goto success_peg_59;
                goto out_peg_57;
                success_peg_59:
                ;
            
            
            
            result_peg_55.setValue(Value((void*) "!="));
                for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_55.getPosition()))){
                        result_peg_55.nextPosition();
                    } else {
                        goto out_peg_57;
                    }
                }
            
            
            
            {
                    int position_peg_71 = result_peg_55.getPosition();
                    
                    result_peg_55.reset();
                    do{
                        Result result_peg_73(result_peg_55.getPosition());
                        {
                            int position_peg_75 = result_peg_73.getPosition();
                            
                            result_peg_73.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    result_peg_73.setPosition(position_peg_75);
                                    goto out_peg_76;
                                }
                            }
                                
                        }
                        goto success_peg_74;
                        out_peg_76:
                        {
                            int position_peg_77 = result_peg_73.getPosition();
                            
                            result_peg_73.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    result_peg_73.setPosition(position_peg_77);
                                    goto out_peg_78;
                                }
                            }
                                
                        }
                        goto success_peg_74;
                        out_peg_78:
                        goto loop_peg_72;
                        success_peg_74:
                        ;
                        result_peg_55.addResult(result_peg_73);
                    } while (true);
                    loop_peg_72:
                    ;
                            
                }
                goto success_peg_70;
                goto out_peg_57;
                success_peg_70:
                ;
            
            
            
            result_peg_55 = rule_expr(stream, result_peg_55.getPosition());
                if (result_peg_55.error()){
                    goto out_peg_57;
                }
                exp = result_peg_55.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name, Value(negateExpression(exp)));
                    result_peg_55.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_assignment = result_peg_55;
        stream.update(result_peg_55.getPosition());
        
        
        return result_peg_55;
        out_peg_57:
        Result result_peg_80(myposition);
        
        
        {
        
            result_peg_80 = rule_identifier(stream, result_peg_80.getPosition());
                if (result_peg_80.error()){
                    goto out_peg_82;
                }
                name = result_peg_80.getValues();
            
            
            
            {
                    int position_peg_85 = result_peg_80.getPosition();
                    
                    result_peg_80.reset();
                    do{
                        Result result_peg_87(result_peg_80.getPosition());
                        {
                            int position_peg_89 = result_peg_87.getPosition();
                            
                            result_peg_87.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_87.getPosition()))){
                                    result_peg_87.nextPosition();
                                } else {
                                    result_peg_87.setPosition(position_peg_89);
                                    goto out_peg_90;
                                }
                            }
                                
                        }
                        goto success_peg_88;
                        out_peg_90:
                        {
                            int position_peg_91 = result_peg_87.getPosition();
                            
                            result_peg_87.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_87.getPosition()))){
                                    result_peg_87.nextPosition();
                                } else {
                                    result_peg_87.setPosition(position_peg_91);
                                    goto out_peg_92;
                                }
                            }
                                
                        }
                        goto success_peg_88;
                        out_peg_92:
                        goto loop_peg_86;
                        success_peg_88:
                        ;
                        result_peg_80.addResult(result_peg_87);
                    } while (true);
                    loop_peg_86:
                    ;
                            
                }
                goto success_peg_84;
                goto out_peg_82;
                success_peg_84:
                ;
            
            
            
            result_peg_80.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_80.getPosition()))){
                        result_peg_80.nextPosition();
                    } else {
                        goto out_peg_82;
                    }
                }
            
            
            
            result_peg_80 = rule_whitespace(stream, result_peg_80.getPosition());
                if (result_peg_80.error()){
                    goto out_peg_82;
                }
            
            
            
            Result result_peg_96(result_peg_80.getPosition());
                result_peg_96 = rule_line_end(stream, result_peg_96.getPosition());
                if (result_peg_96.error()){
                    goto out_peg_82;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name);
                    result_peg_80.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_assignment = result_peg_80;
        stream.update(result_peg_80.getPosition());
        
        
        return result_peg_80;
        out_peg_82:
        Result result_peg_97(myposition);
        
        
        {
        
            result_peg_97 = rule_identifier(stream, result_peg_97.getPosition());
                if (result_peg_97.error()){
                    goto out_peg_99;
                }
                name = result_peg_97.getValues();
            
            
            
            {
                    int position_peg_102 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_104(result_peg_97.getPosition());
                        {
                            int position_peg_106 = result_peg_104.getPosition();
                            
                            result_peg_104.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_104.getPosition()))){
                                    result_peg_104.nextPosition();
                                } else {
                                    result_peg_104.setPosition(position_peg_106);
                                    goto out_peg_107;
                                }
                            }
                                
                        }
                        goto success_peg_105;
                        out_peg_107:
                        {
                            int position_peg_108 = result_peg_104.getPosition();
                            
                            result_peg_104.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_104.getPosition()))){
                                    result_peg_104.nextPosition();
                                } else {
                                    result_peg_104.setPosition(position_peg_108);
                                    goto out_peg_109;
                                }
                            }
                                
                        }
                        goto success_peg_105;
                        out_peg_109:
                        goto loop_peg_103;
                        success_peg_105:
                        ;
                        result_peg_97.addResult(result_peg_104);
                    } while (true);
                    loop_peg_103:
                    ;
                            
                }
                goto success_peg_101;
                goto out_peg_99;
                success_peg_101:
                ;
            
            
            
            result_peg_97.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        goto out_peg_99;
                    }
                }
            
            
            
            {
                    int position_peg_113 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_115(result_peg_97.getPosition());
                        {
                            int position_peg_117 = result_peg_115.getPosition();
                            
                            result_peg_115.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_115.getPosition()))){
                                    result_peg_115.nextPosition();
                                } else {
                                    result_peg_115.setPosition(position_peg_117);
                                    goto out_peg_118;
                                }
                            }
                                
                        }
                        goto success_peg_116;
                        out_peg_118:
                        {
                            int position_peg_119 = result_peg_115.getPosition();
                            
                            result_peg_115.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_115.getPosition()))){
                                    result_peg_115.nextPosition();
                                } else {
                                    result_peg_115.setPosition(position_peg_119);
                                    goto out_peg_120;
                                }
                            }
                                
                        }
                        goto success_peg_116;
                        out_peg_120:
                        goto loop_peg_114;
                        success_peg_116:
                        ;
                        result_peg_97.addResult(result_peg_115);
                    } while (true);
                    loop_peg_114:
                    ;
                            
                }
                goto success_peg_112;
                goto out_peg_99;
                success_peg_112:
                ;
            
            
            
            result_peg_97 = rule_integer(stream, result_peg_97.getPosition());
                if (result_peg_97.error()){
                    goto out_peg_99;
                }
                index = result_peg_97.getValues();
            
            
            
            {
                    int position_peg_124 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_126(result_peg_97.getPosition());
                        {
                            int position_peg_128 = result_peg_126.getPosition();
                            
                            result_peg_126.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_126.getPosition()))){
                                    result_peg_126.nextPosition();
                                } else {
                                    result_peg_126.setPosition(position_peg_128);
                                    goto out_peg_129;
                                }
                            }
                                
                        }
                        goto success_peg_127;
                        out_peg_129:
                        {
                            int position_peg_130 = result_peg_126.getPosition();
                            
                            result_peg_126.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_126.getPosition()))){
                                    result_peg_126.nextPosition();
                                } else {
                                    result_peg_126.setPosition(position_peg_130);
                                    goto out_peg_131;
                                }
                            }
                                
                        }
                        goto success_peg_127;
                        out_peg_131:
                        goto loop_peg_125;
                        success_peg_127:
                        ;
                        result_peg_97.addResult(result_peg_126);
                    } while (true);
                    loop_peg_125:
                    ;
                            
                }
                goto success_peg_123;
                goto out_peg_99;
                success_peg_123:
                ;
            
            
            
            result_peg_97.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        goto out_peg_99;
                    }
                }
            
            
            
            {
                    int position_peg_135 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_137(result_peg_97.getPosition());
                        {
                            int position_peg_139 = result_peg_137.getPosition();
                            
                            result_peg_137.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_137.getPosition()))){
                                    result_peg_137.nextPosition();
                                } else {
                                    result_peg_137.setPosition(position_peg_139);
                                    goto out_peg_140;
                                }
                            }
                                
                        }
                        goto success_peg_138;
                        out_peg_140:
                        {
                            int position_peg_141 = result_peg_137.getPosition();
                            
                            result_peg_137.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_137.getPosition()))){
                                    result_peg_137.nextPosition();
                                } else {
                                    result_peg_137.setPosition(position_peg_141);
                                    goto out_peg_142;
                                }
                            }
                                
                        }
                        goto success_peg_138;
                        out_peg_142:
                        goto loop_peg_136;
                        success_peg_138:
                        ;
                        result_peg_97.addResult(result_peg_137);
                    } while (true);
                    loop_peg_136:
                    ;
                            
                }
                goto success_peg_134;
                goto out_peg_99;
                success_peg_134:
                ;
            
            
            
            result_peg_97.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        goto out_peg_99;
                    }
                }
            
            
            
            {
                    int position_peg_146 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_148(result_peg_97.getPosition());
                        {
                            int position_peg_150 = result_peg_148.getPosition();
                            
                            result_peg_148.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_148.getPosition()))){
                                    result_peg_148.nextPosition();
                                } else {
                                    result_peg_148.setPosition(position_peg_150);
                                    goto out_peg_151;
                                }
                            }
                                
                        }
                        goto success_peg_149;
                        out_peg_151:
                        {
                            int position_peg_152 = result_peg_148.getPosition();
                            
                            result_peg_148.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_148.getPosition()))){
                                    result_peg_148.nextPosition();
                                } else {
                                    result_peg_148.setPosition(position_peg_152);
                                    goto out_peg_153;
                                }
                            }
                                
                        }
                        goto success_peg_149;
                        out_peg_153:
                        goto loop_peg_147;
                        success_peg_149:
                        ;
                        result_peg_97.addResult(result_peg_148);
                    } while (true);
                    loop_peg_147:
                    ;
                            
                }
                goto success_peg_145;
                goto out_peg_99;
                success_peg_145:
                ;
            
            
            
            result_peg_97 = rule_expr(stream, result_peg_97.getPosition());
                if (result_peg_97.error()){
                    goto out_peg_99;
                }
                exp = result_peg_97.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(name, index, exp);
                    result_peg_97.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_assignment = result_peg_97;
        stream.update(result_peg_97.getPosition());
        
        
        return result_peg_97;
        out_peg_99:
        Result result_peg_155(myposition);
        
        
        {
        
            result_peg_155.setValue(Value((void*) "ctrl"));
                for (int i = 0; i < 4; i++){
                    if (compareCharCase("ctrl"[i], stream.get(result_peg_155.getPosition()))){
                        result_peg_155.nextPosition();
                    } else {
                        goto out_peg_157;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(Value(makeSimpleIdentifier("ctrl")), Value(makeNumber(1)));
                    result_peg_155.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_assignment = result_peg_155;
        stream.update(result_peg_155.getPosition());
        
        
        return result_peg_155;
        out_peg_157:
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_assignment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_identifier.calculated()){
        return column_peg_2.chunk2->chunk_identifier;
    }
    
    
    RuleTrace trace_peg_1(stream, "identifier");
    int myposition = position;
    
    
    Value line;
        Value first;
        Value rest;
    Result result_peg_3(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_5 = stream.getLineInfo(result_peg_3.getPosition());
                line = &line_info_peg_5;
            
            
            
            result_peg_3 = rule_name(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_7;
                }
                first = result_peg_3.getValues();
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_10(result_peg_3.getPosition());
                    {
                    
                        result_peg_10.setValue(Value((void*) "."));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    goto loop_peg_9;
                                }
                            }
                        
                        
                        
                        result_peg_10 = rule_name(stream, result_peg_10.getPosition());
                            if (result_peg_10.error()){
                                goto loop_peg_9;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_10);
                } while (true);
                loop_peg_9:
                ;
                rest = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_identifier = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_7:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_integer.calculated()){
        return column_peg_2.chunk2->chunk_integer;
    }
    
    
    RuleTrace trace_peg_1(stream, "integer");
    int myposition = position;
    
    
    Value line;
        Value n1;
        Value n2;
    Result result_peg_3(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_5 = stream.getLineInfo(result_peg_3.getPosition());
                line = &line_info_peg_5;
            
            
            
            int save_peg_7 = result_peg_3.getPosition();
                
                {
                    int position_peg_9 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_9);
                            goto out_peg_10;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_10:
                {
                    int position_peg_11 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_11);
                            goto out_peg_12;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_12:
                
                result_peg_3 = Result(save_peg_7);
                result_peg_3.setValue(Value((void*) 0));
                
                success_peg_8:
                ;
                n1 = result_peg_3.getValues();
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_15(result_peg_3.getPosition());
                    {
                        int position_peg_17 = result_peg_15.getPosition();
                        
                        char letter_peg_18 = stream.get(result_peg_15.getPosition());
                        if (letter_peg_18 != '\0' && strchr("0123456789", letter_peg_18) != NULL){
                            result_peg_15.nextPosition();
                            result_peg_15.setValue(Value((void*) (intptr_t) letter_peg_18));
                        } else {
                            result_peg_15.setPosition(position_peg_17);
                            goto out_peg_19;
                        }
                        
                    }
                    goto success_peg_16;
                    out_peg_19:
                    goto loop_peg_14;
                    success_peg_16:
                    ;
                    result_peg_3.addResult(result_peg_15);
                } while (true);
                loop_peg_14:
                if (result_peg_3.matches() == 0){
                    goto out_peg_20;
                }
                n2 = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeInteger(getCurrentLine(line), getCurrentColumn(line), n1, n2);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_integer = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_20:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_float.calculated()){
        return column_peg_2.chunk2->chunk_float;
    }
    
    
    RuleTrace trace_peg_1(stream, "float");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_3(myposition);
        
        
        {
        
            int save_peg_5 = result_peg_3.getPosition();
                
                {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_7);
                            goto out_peg_8;
                        }
                    }
                        
                }
                goto success_peg_6;
                out_peg_8:
                {
                    int position_peg_9 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_9);
                            goto out_peg_10;
                        }
                    }
                        
                }
                goto success_peg_6;
                out_peg_10:
                
                result_peg_3 = Result(save_peg_5);
                result_peg_3.setValue(Value((void*) 0));
                
                success_peg_6:
                ;
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.reset();
                do{
                    Result result_peg_13(result_peg_3.getPosition());
                    {
                        int position_peg_15 = result_peg_13.getPosition();
                        
                        char letter_peg_16 = stream.get(result_peg_13.getPosition());
                        if (letter_peg_16 != '\0' && strchr("0123456789", letter_peg_16) != NULL){
                            result_peg_13.nextPosition();
                            result_peg_13.setValue(Value((void*) (intptr_t) letter_peg_16));
                        } else {
                            result_peg_13.setPosition(position_peg_15);
                            goto out_peg_17;
                        }
                        
                    }
                    goto success_peg_14;
                    out_peg_17:
                    goto loop_peg_12;
                    success_peg_14:
                    ;
                    result_peg_3.addResult(result_peg_13);
                } while (true);
                loop_peg_12:
                ;
                left = result_peg_3.getValues();
            
            
            
            result_peg_3.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_19;
                    }
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_22(result_peg_3.getPosition());
                    {
                        int position_peg_24 = result_peg_22.getPosition();
                        
                        char letter_peg_25 = stream.get(result_peg_22.getPosition());
                        if (letter_peg_25 != '\0' && strchr("0123456789", letter_peg_25) != NULL){
                            result_peg_22.nextPosition();
                            result_peg_22.setValue(Value((void*) (intptr_t) letter_peg_25));
                        } else {
                            result_peg_22.setPosition(position_peg_24);
                            goto out_peg_26;
                        }
                        
                    }
                    goto success_peg_23;
                    out_peg_26:
                    goto loop_peg_21;
                    success_peg_23:
                    ;
                    result_peg_3.addResult(result_peg_22);
                } while (true);
                loop_peg_21:
                if (result_peg_3.matches() == 0){
                    goto out_peg_19;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_4.getValues(), Value(parseDouble(left,right)));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_float = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_19:
        Result result_peg_27(myposition);
        
        
        {
        
            int save_peg_29 = result_peg_27.getPosition();
                
                {
                    int position_peg_31 = result_peg_27.getPosition();
                    
                    result_peg_27.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_27.getPosition()))){
                            result_peg_27.nextPosition();
                        } else {
                            result_peg_27.setPosition(position_peg_31);
                            goto out_peg_32;
                        }
                    }
                        
                }
                goto success_peg_30;
                out_peg_32:
                {
                    int position_peg_33 = result_peg_27.getPosition();
                    
                    result_peg_27.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_27.getPosition()))){
                            result_peg_27.nextPosition();
                        } else {
                            result_peg_27.setPosition(position_peg_33);
                            goto out_peg_34;
                        }
                    }
                        
                }
                goto success_peg_30;
                out_peg_34:
                
                result_peg_27 = Result(save_peg_29);
                result_peg_27.setValue(Value((void*) 0));
                
                success_peg_30:
                ;
            
            Result result_peg_28 = result_peg_27;
            
            result_peg_27.reset();
                do{
                    Result result_peg_37(result_peg_27.getPosition());
                    {
                        int position_peg_39 = result_peg_37.getPosition();
                        
                        char letter_peg_40 = stream.get(result_peg_37.getPosition());
                        if (letter_peg_40 != '\0' && strchr("0123456789", letter_peg_40) != NULL){
                            result_peg_37.nextPosition();
                            result_peg_37.setValue(Value((void*) (intptr_t) letter_peg_40));
                        } else {
                            result_peg_37.setPosition(position_peg_39);
                            goto out_peg_41;
                        }
                        
                    }
                    goto success_peg_38;
                    out_peg_41:
                    goto loop_peg_36;
                    success_peg_38:
                    ;
                    result_peg_27.addResult(result_peg_37);
                } while (true);
                loop_peg_36:
                if (result_peg_27.matches() == 0){
                    goto out_peg_42;
                }
                left = result_peg_27.getValues();
            
            
            
            result_peg_27.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_27.getPosition()))){
                        result_peg_27.nextPosition();
                    } else {
                        goto out_peg_42;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_28.getValues(), Value(parseDouble(left)));
                    result_peg_27.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_float = result_peg_27;
        stream.update(result_peg_27.getPosition());
        
        
        return result_peg_27;
        out_peg_42:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_float = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_range.calculated()){
        return column_peg_2.chunk2->chunk_range;
    }
    
    
    RuleTrace trace_peg_1(stream, "range");
    int myposition = position;
    
    
    Value low;
        Value high;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3 = rule_expr_c(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                low = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_5;
                success_peg_18:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_30 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_32(result_peg_3.getPosition());
                        {
                            int position_peg_34 = result_peg_32.getPosition();
                            
                            result_peg_32.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_32.getPosition()))){
                                    result_peg_32.nextPosition();
                                } else {
                                    result_peg_32.setPosition(position_peg_34);
                                    goto out_peg_35;
                                }
                            }
                                
                        }
                        goto success_peg_33;
                        out_peg_35:
                        {
                            int position_peg_36 = result_peg_32.getPosition();
                            
                            result_peg_32.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_32.getPosition()))){
                                    result_peg_32.nextPosition();
                                } else {
                                    result_peg_32.setPosition(position_peg_36);
                                    goto out_peg_37;
                                }
                            }
                                
                        }
                        goto success_peg_33;
                        out_peg_37:
                        goto loop_peg_31;
                        success_peg_33:
                        ;
                        result_peg_3.addResult(result_peg_32);
                    } while (true);
                    loop_peg_31:
                    ;
                            
                }
                goto success_peg_29;
                goto out_peg_5;
                success_peg_29:
                ;
            
            
            
            result_peg_3 = rule_expr_c(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                high = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_41 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_43(result_peg_3.getPosition());
                        {
                            int position_peg_45 = result_peg_43.getPosition();
                            
                            result_peg_43.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_43.getPosition()))){
                                    result_peg_43.nextPosition();
                                } else {
                                    result_peg_43.setPosition(position_peg_45);
                                    goto out_peg_46;
                                }
                            }
                                
                        }
                        goto success_peg_44;
                        out_peg_46:
                        {
                            int position_peg_47 = result_peg_43.getPosition();
                            
                            result_peg_43.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_43.getPosition()))){
                                    result_peg_43.nextPosition();
                                } else {
                                    result_peg_43.setPosition(position_peg_47);
                                    goto out_peg_48;
                                }
                            }
                                
                        }
                        goto success_peg_44;
                        out_peg_48:
                        goto loop_peg_42;
                        success_peg_44:
                        ;
                        result_peg_3.addResult(result_peg_43);
                    } while (true);
                    loop_peg_42:
                    ;
                            
                }
                goto success_peg_40;
                goto out_peg_5;
                success_peg_40:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllInclusive, low, high);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_range = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_50(myposition);
        
        
        {
        
            result_peg_50.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_50.getPosition()))){
                        result_peg_50.nextPosition();
                    } else {
                        goto out_peg_52;
                    }
                }
            
            
            
            {
                    int position_peg_55 = result_peg_50.getPosition();
                    
                    result_peg_50.reset();
                    do{
                        Result result_peg_57(result_peg_50.getPosition());
                        {
                            int position_peg_59 = result_peg_57.getPosition();
                            
                            result_peg_57.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_57.getPosition()))){
                                    result_peg_57.nextPosition();
                                } else {
                                    result_peg_57.setPosition(position_peg_59);
                                    goto out_peg_60;
                                }
                            }
                                
                        }
                        goto success_peg_58;
                        out_peg_60:
                        {
                            int position_peg_61 = result_peg_57.getPosition();
                            
                            result_peg_57.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_57.getPosition()))){
                                    result_peg_57.nextPosition();
                                } else {
                                    result_peg_57.setPosition(position_peg_61);
                                    goto out_peg_62;
                                }
                            }
                                
                        }
                        goto success_peg_58;
                        out_peg_62:
                        goto loop_peg_56;
                        success_peg_58:
                        ;
                        result_peg_50.addResult(result_peg_57);
                    } while (true);
                    loop_peg_56:
                    ;
                            
                }
                goto success_peg_54;
                goto out_peg_52;
                success_peg_54:
                ;
            
            
            
            result_peg_50 = rule_expr_c(stream, result_peg_50.getPosition());
                if (result_peg_50.error()){
                    goto out_peg_52;
                }
                low = result_peg_50.getValues();
            
            
            
            {
                    int position_peg_66 = result_peg_50.getPosition();
                    
                    result_peg_50.reset();
                    do{
                        Result result_peg_68(result_peg_50.getPosition());
                        {
                            int position_peg_70 = result_peg_68.getPosition();
                            
                            result_peg_68.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_68.getPosition()))){
                                    result_peg_68.nextPosition();
                                } else {
                                    result_peg_68.setPosition(position_peg_70);
                                    goto out_peg_71;
                                }
                            }
                                
                        }
                        goto success_peg_69;
                        out_peg_71:
                        {
                            int position_peg_72 = result_peg_68.getPosition();
                            
                            result_peg_68.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_68.getPosition()))){
                                    result_peg_68.nextPosition();
                                } else {
                                    result_peg_68.setPosition(position_peg_72);
                                    goto out_peg_73;
                                }
                            }
                                
                        }
                        goto success_peg_69;
                        out_peg_73:
                        goto loop_peg_67;
                        success_peg_69:
                        ;
                        result_peg_50.addResult(result_peg_68);
                    } while (true);
                    loop_peg_67:
                    ;
                            
                }
                goto success_peg_65;
                goto out_peg_52;
                success_peg_65:
                ;
            
            
            
            result_peg_50.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_50.getPosition()))){
                        result_peg_50.nextPosition();
                    } else {
                        goto out_peg_52;
                    }
                }
            
            
            
            {
                    int position_peg_77 = result_peg_50.getPosition();
                    
                    result_peg_50.reset();
                    do{
                        Result result_peg_79(result_peg_50.getPosition());
                        {
                            int position_peg_81 = result_peg_79.getPosition();
                            
                            result_peg_79.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_79.getPosition()))){
                                    result_peg_79.nextPosition();
                                } else {
                                    result_peg_79.setPosition(position_peg_81);
                                    goto out_peg_82;
                                }
                            }
                                
                        }
                        goto success_peg_80;
                        out_peg_82:
                        {
                            int position_peg_83 = result_peg_79.getPosition();
                            
                            result_peg_79.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_79.getPosition()))){
                                    result_peg_79.nextPosition();
                                } else {
                                    result_peg_79.setPosition(position_peg_83);
                                    goto out_peg_84;
                                }
                            }
                                
                        }
                        goto success_peg_80;
                        out_peg_84:
                        goto loop_peg_78;
                        success_peg_80:
                        ;
                        result_peg_50.addResult(result_peg_79);
                    } while (true);
                    loop_peg_78:
                    ;
                            
                }
                goto success_peg_76;
                goto out_peg_52;
                success_peg_76:
                ;
            
            
            
            result_peg_50 = rule_expr_c(stream, result_peg_50.getPosition());
                if (result_peg_50.error()){
                    goto out_peg_52;
                }
                high = result_peg_50.getValues();
            
            
            
            {
                    int position_peg_88 = result_peg_50.getPosition();
                    
                    result_peg_50.reset();
                    do{
                        Result result_peg_90(result_peg_50.getPosition());
                        {
                            int position_peg_92 = result_peg_90.getPosition();
                            
                            result_peg_90.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_90.getPosition()))){
                                    result_peg_90.nextPosition();
                                } else {
                                    result_peg_90.setPosition(position_peg_92);
                                    goto out_peg_93;
                                }
                            }
                                
                        }
                        goto success_peg_91;
                        out_peg_93:
                        {
                            int position_peg_94 = result_peg_90.getPosition();
                            
                            result_peg_90.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_90.getPosition()))){
                                    result_peg_90.nextPosition();
                                } else {
                                    result_peg_90.setPosition(position_peg_94);
                                    goto out_peg_95;
                                }
                            }
                                
                        }
                        goto success_peg_91;
                        out_peg_95:
                        goto loop_peg_89;
                        success_peg_91:
                        ;
                        result_peg_50.addResult(result_peg_90);
                    } while (true);
                    loop_peg_89:
                    ;
                            
                }
                goto success_peg_87;
                goto out_peg_52;
                success_peg_87:
                ;
            
            
            
            result_peg_50.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_50.getPosition()))){
                        result_peg_50.nextPosition();
                    } else {
                        goto out_peg_52;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllExclusive, low, high);
                    result_peg_50.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_range = result_peg_50;
        stream.update(result_peg_50.getPosition());
        
        
        return result_peg_50;
        out_peg_52:
        Result result_peg_97(myposition);
        
        
        {
        
            result_peg_97.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        goto out_peg_99;
                    }
                }
            
            
            
            {
                    int position_peg_102 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_104(result_peg_97.getPosition());
                        {
                            int position_peg_106 = result_peg_104.getPosition();
                            
                            result_peg_104.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_104.getPosition()))){
                                    result_peg_104.nextPosition();
                                } else {
                                    result_peg_104.setPosition(position_peg_106);
                                    goto out_peg_107;
                                }
                            }
                                
                        }
                        goto success_peg_105;
                        out_peg_107:
                        {
                            int position_peg_108 = result_peg_104.getPosition();
                            
                            result_peg_104.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_104.getPosition()))){
                                    result_peg_104.nextPosition();
                                } else {
                                    result_peg_104.setPosition(position_peg_108);
                                    goto out_peg_109;
                                }
                            }
                                
                        }
                        goto success_peg_105;
                        out_peg_109:
                        goto loop_peg_103;
                        success_peg_105:
                        ;
                        result_peg_97.addResult(result_peg_104);
                    } while (true);
                    loop_peg_103:
                    ;
                            
                }
                goto success_peg_101;
                goto out_peg_99;
                success_peg_101:
                ;
            
            
            
            result_peg_97 = rule_expr_c(stream, result_peg_97.getPosition());
                if (result_peg_97.error()){
                    goto out_peg_99;
                }
                low = result_peg_97.getValues();
            
            
            
            {
                    int position_peg_113 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_115(result_peg_97.getPosition());
                        {
                            int position_peg_117 = result_peg_115.getPosition();
                            
                            result_peg_115.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_115.getPosition()))){
                                    result_peg_115.nextPosition();
                                } else {
                                    result_peg_115.setPosition(position_peg_117);
                                    goto out_peg_118;
                                }
                            }
                                
                        }
                        goto success_peg_116;
                        out_peg_118:
                        {
                            int position_peg_119 = result_peg_115.getPosition();
                            
                            result_peg_115.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_115.getPosition()))){
                                    result_peg_115.nextPosition();
                                } else {
                                    result_peg_115.setPosition(position_peg_119);
                                    goto out_peg_120;
                                }
                            }
                                
                        }
                        goto success_peg_116;
                        out_peg_120:
                        goto loop_peg_114;
                        success_peg_116:
                        ;
                        result_peg_97.addResult(result_peg_115);
                    } while (true);
                    loop_peg_114:
                    ;
                            
                }
                goto success_peg_112;
                goto out_peg_99;
                success_peg_112:
                ;
            
            
            
            result_peg_97.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        goto out_peg_99;
                    }
                }
            
            
            
            {
                    int position_peg_124 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_126(result_peg_97.getPosition());
                        {
                            int position_peg_128 = result_peg_126.getPosition();
                            
                            result_peg_126.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_126.getPosition()))){
                                    result_peg_126.nextPosition();
                                } else {
                                    result_peg_126.setPosition(position_peg_128);
                                    goto out_peg_129;
                                }
                            }
                                
                        }
                        goto success_peg_127;
                        out_peg_129:
                        {
                            int position_peg_130 = result_peg_126.getPosition();
                            
                            result_peg_126.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_126.getPosition()))){
                                    result_peg_126.nextPosition();
                                } else {
                                    result_peg_126.setPosition(position_peg_130);
                                    goto out_peg_131;
                                }
                            }
                                
                        }
                        goto success_peg_127;
                        out_peg_131:
                        goto loop_peg_125;
                        success_peg_127:
                        ;
                        result_peg_97.addResult(result_peg_126);
                    } while (true);
                    loop_peg_125:
                    ;
                            
                }
                goto success_peg_123;
                goto out_peg_99;
                success_peg_123:
                ;
            
            
            
            result_peg_97 = rule_expr_c(stream, result_peg_97.getPosition());
                if (result_peg_97.error()){
                    goto out_peg_99;
                }
                high = result_peg_97.getValues();
            
            
            
            {
                    int position_peg_135 = result_peg_97.getPosition();
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_137(result_peg_97.getPosition());
                        {
                            int position_peg_139 = result_peg_137.getPosition();
                            
                            result_peg_137.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_137.getPosition()))){
                                    result_peg_137.nextPosition();
                                } else {
                                    result_peg_137.setPosition(position_peg_139);
                                    goto out_peg_140;
                                }
                            }
                                
                        }
                        goto success_peg_138;
                        out_peg_140:
                        {
                            int position_peg_141 = result_peg_137.getPosition();
                            
                            result_peg_137.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_137.getPosition()))){
                                    result_peg_137.nextPosition();
                                } else {
                                    result_peg_137.setPosition(position_peg_141);
                                    goto out_peg_142;
                                }
                            }
                                
                        }
                        goto success_peg_138;
                        out_peg_142:
                        goto loop_peg_136;
                        success_peg_138:
                        ;
                        result_peg_97.addResult(result_peg_137);
                    } while (true);
                    loop_peg_136:
                    ;
                            
                }
                goto success_peg_134;
                goto out_peg_99;
                success_peg_134:
                ;
            
            
            
            result_peg_97.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        goto out_peg_99;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                    result_peg_97.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_range = result_peg_97;
        stream.update(result_peg_97.getPosition());
        
        
        return result_peg_97;
        out_peg_99:
        Result result_peg_144(myposition);
        
        
        {
        
            result_peg_144.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_144.getPosition()))){
                        result_peg_144.nextPosition();
                    } else {
                        goto out_peg_146;
                    }
                }
            
            
            
            {
                    int position_peg_149 = result_peg_144.getPosition();
                    
                    result_peg_144.reset();
                    do{
                        Result result_peg_151(result_peg_144.getPosition());
                        {
                            int position_peg_153 = result_peg_151.getPosition();
                            
                            result_peg_151.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_151.getPosition()))){
                                    result_peg_151.nextPosition();
                                } else {
                                    result_peg_151.setPosition(position_peg_153);
                                    goto out_peg_154;
                                }
                            }
                                
                        }
                        goto success_peg_152;
                        out_peg_154:
                        {
                            int position_peg_155 = result_peg_151.getPosition();
                            
                            result_peg_151.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_151.getPosition()))){
                                    result_peg_151.nextPosition();
                                } else {
                                    result_peg_151.setPosition(position_peg_155);
                                    goto out_peg_156;
                                }
                            }
                                
                        }
                        goto success_peg_152;
                        out_peg_156:
                        goto loop_peg_150;
                        success_peg_152:
                        ;
                        result_peg_144.addResult(result_peg_151);
                    } while (true);
                    loop_peg_150:
                    ;
                            
                }
                goto success_peg_148;
                goto out_peg_146;
                success_peg_148:
                ;
            
            
            
            result_peg_144 = rule_expr_c(stream, result_peg_144.getPosition());
                if (result_peg_144.error()){
                    goto out_peg_146;
                }
                low = result_peg_144.getValues();
            
            
            
            {
                    int position_peg_160 = result_peg_144.getPosition();
                    
                    result_peg_144.reset();
                    do{
                        Result result_peg_162(result_peg_144.getPosition());
                        {
                            int position_peg_164 = result_peg_162.getPosition();
                            
                            result_peg_162.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_162.getPosition()))){
                                    result_peg_162.nextPosition();
                                } else {
                                    result_peg_162.setPosition(position_peg_164);
                                    goto out_peg_165;
                                }
                            }
                                
                        }
                        goto success_peg_163;
                        out_peg_165:
                        {
                            int position_peg_166 = result_peg_162.getPosition();
                            
                            result_peg_162.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_162.getPosition()))){
                                    result_peg_162.nextPosition();
                                } else {
                                    result_peg_162.setPosition(position_peg_166);
                                    goto out_peg_167;
                                }
                            }
                                
                        }
                        goto success_peg_163;
                        out_peg_167:
                        goto loop_peg_161;
                        success_peg_163:
                        ;
                        result_peg_144.addResult(result_peg_162);
                    } while (true);
                    loop_peg_161:
                    ;
                            
                }
                goto success_peg_159;
                goto out_peg_146;
                success_peg_159:
                ;
            
            
            
            result_peg_144.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_144.getPosition()))){
                        result_peg_144.nextPosition();
                    } else {
                        goto out_peg_146;
                    }
                }
            
            
            
            {
                    int position_peg_171 = result_peg_144.getPosition();
                    
                    result_peg_144.reset();
                    do{
                        Result result_peg_173(result_peg_144.getPosition());
                        {
                            int position_peg_175 = result_peg_173.getPosition();
                            
                            result_peg_173.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_173.getPosition()))){
                                    result_peg_173.nextPosition();
                                } else {
                                    result_peg_173.setPosition(position_peg_175);
                                    goto out_peg_176;
                                }
                            }
                                
                        }
                        goto success_peg_174;
                        out_peg_176:
                        {
                            int position_peg_177 = result_peg_173.getPosition();
                            
                            result_peg_173.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_173.getPosition()))){
                                    result_peg_173.nextPosition();
                                } else {
                                    result_peg_173.setPosition(position_peg_177);
                                    goto out_peg_178;
                                }
                            }
                                
                        }
                        goto success_peg_174;
                        out_peg_178:
                        goto loop_peg_172;
                        success_peg_174:
                        ;
                        result_peg_144.addResult(result_peg_173);
                    } while (true);
                    loop_peg_172:
                    ;
                            
                }
                goto success_peg_170;
                goto out_peg_146;
                success_peg_170:
                ;
            
            
            
            result_peg_144 = rule_expr_c(stream, result_peg_144.getPosition());
                if (result_peg_144.error()){
                    goto out_peg_146;
                }
                high = result_peg_144.getValues();
            
            
            
            {
                    int position_peg_182 = result_peg_144.getPosition();
                    
                    result_peg_144.reset();
                    do{
                        Result result_peg_184(result_peg_144.getPosition());
                        {
                            int position_peg_186 = result_peg_184.getPosition();
                            
                            result_peg_184.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_184.getPosition()))){
                                    result_peg_184.nextPosition();
                                } else {
                                    result_peg_184.setPosition(position_peg_186);
                                    goto out_peg_187;
                                }
                            }
                                
                        }
                        goto success_peg_185;
                        out_peg_187:
                        {
                            int position_peg_188 = result_peg_184.getPosition();
                            
                            result_peg_184.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_184.getPosition()))){
                                    result_peg_184.nextPosition();
                                } else {
                                    result_peg_184.setPosition(position_peg_188);
                                    goto out_peg_189;
                                }
                            }
                                
                        }
                        goto success_peg_185;
                        out_peg_189:
                        goto loop_peg_183;
                        success_peg_185:
                        ;
                        result_peg_144.addResult(result_peg_184);
                    } while (true);
                    loop_peg_183:
                    ;
                            
                }
                goto success_peg_181;
                goto out_peg_146;
                success_peg_181:
                ;
            
            
            
            result_peg_144.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_144.getPosition()))){
                        result_peg_144.nextPosition();
                    } else {
                        goto out_peg_146;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                    result_peg_144.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_range = result_peg_144;
        stream.update(result_peg_144.getPosition());
        
        
        return result_peg_144;
        out_peg_146:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_range = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_name.calculated()){
        return column_peg_2.chunk2->chunk_name;
    }
    
    
    RuleTrace trace_peg_1(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    char letter_peg_7 = stream.get(result_peg_3.getPosition());
                    if (letter_peg_7 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_7) != NULL){
                        result_peg_3.nextPosition();
                        result_peg_3.setValue(Value((void*) (intptr_t) letter_peg_7));
                    } else {
                        result_peg_3.setPosition(position_peg_6);
                        goto out_peg_8;
                    }
                    
                }
                goto success_peg_5;
                out_peg_8:
                goto out_peg_9;
                success_peg_5:
                ;
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.reset();
                do{
                    Result result_peg_12(result_peg_3.getPosition());
                    {
                        int position_peg_14 = result_peg_12.getPosition();
                        
                        {
                            int position_peg_16 = result_peg_12.getPosition();
                            
                            char letter_peg_17 = stream.get(result_peg_12.getPosition());
                            if (letter_peg_17 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_17) != NULL){
                                result_peg_12.nextPosition();
                                result_peg_12.setValue(Value((void*) (intptr_t) letter_peg_17));
                            } else {
                                result_peg_12.setPosition(position_peg_16);
                                goto out_peg_18;
                            }
                            
                        }
                        goto success_peg_15;
                        out_peg_18:
                        result_peg_12.setPosition(position_peg_14);
                        goto out_peg_19;
                        success_peg_15:
                        ;
                        
                    }
                    goto success_peg_13;
                    out_peg_19:
                    {
                        int position_peg_20 = result_peg_12.getPosition();
                        
                        {
                            int position_peg_22 = result_peg_12.getPosition();
                            
                            char letter_peg_23 = stream.get(result_peg_12.getPosition());
                            if (letter_peg_23 != '\0' && strchr("0123456789", letter_peg_23) != NULL){
                                result_peg_12.nextPosition();
                                result_peg_12.setValue(Value((void*) (intptr_t) letter_peg_23));
                            } else {
                                result_peg_12.setPosition(position_peg_22);
                                goto out_peg_24;
                            }
                            
                        }
                        goto success_peg_21;
                        out_peg_24:
                        result_peg_12.setPosition(position_peg_20);
                        goto out_peg_25;
                        success_peg_21:
                        ;
                        
                    }
                    goto success_peg_13;
                    out_peg_25:
                    goto loop_peg_11;
                    success_peg_13:
                    ;
                    result_peg_3.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                ;
            
            Result result_peg_10 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_4.getValues().getValue(),result_peg_10.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_name = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_9:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk3 != 0 && column_peg_2.chunk3->chunk_valuelist.calculated()){
        return column_peg_2.chunk3->chunk_valuelist;
    }
    
    
    RuleTrace trace_peg_1(stream, "valuelist");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr_c(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                first = result_peg_3.getValues();
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_8(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_11 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_13(result_peg_8.getPosition());
                                    {
                                        int position_peg_15 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_15);
                                                goto out_peg_16;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_16:
                                    {
                                        int position_peg_17 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_17);
                                                goto out_peg_18;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_18:
                                    goto loop_peg_12;
                                    success_peg_14:
                                    ;
                                    result_peg_8.addResult(result_peg_13);
                                } while (true);
                                loop_peg_12:
                                ;
                                        
                            }
                            goto success_peg_10;
                            goto loop_peg_7;
                            success_peg_10:
                            ;
                        
                        
                        
                        result_peg_8.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    goto loop_peg_7;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_22 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_24(result_peg_8.getPosition());
                                    {
                                        int position_peg_26 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_26);
                                                goto out_peg_27;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_27:
                                    {
                                        int position_peg_28 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_28);
                                                goto out_peg_29;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_29:
                                    goto loop_peg_23;
                                    success_peg_25:
                                    ;
                                    result_peg_8.addResult(result_peg_24);
                                } while (true);
                                loop_peg_23:
                                ;
                                        
                            }
                            goto success_peg_21;
                            goto loop_peg_7;
                            success_peg_21:
                            ;
                        
                        
                        
                        result_peg_8 = rule_expr_c(stream, result_peg_8.getPosition());
                            if (result_peg_8.error()){
                                goto loop_peg_7;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                if (result_peg_3.matches() == 0){
                    goto out_peg_5;
                }
                rest = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_valuelist = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_30(myposition);
        
        
        {
        
            {
                    int position_peg_33 = result_peg_30.getPosition();
                    
                    result_peg_30.reset();
                    do{
                        Result result_peg_35(result_peg_30.getPosition());
                        {
                            int position_peg_37 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_37);
                                    goto out_peg_38;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_38:
                        {
                            int position_peg_39 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_39);
                                    goto out_peg_40;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_40:
                        goto loop_peg_34;
                        success_peg_36:
                        ;
                        result_peg_30.addResult(result_peg_35);
                    } while (true);
                    loop_peg_34:
                    ;
                            
                }
                goto success_peg_32;
                goto out_peg_41;
                success_peg_32:
                ;
            
            
            
            result_peg_30.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_41;
                    }
                }
            
            
            
            {
                    int position_peg_45 = result_peg_30.getPosition();
                    
                    result_peg_30.reset();
                    do{
                        Result result_peg_47(result_peg_30.getPosition());
                        {
                            int position_peg_49 = result_peg_47.getPosition();
                            
                            result_peg_47.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_47.getPosition()))){
                                    result_peg_47.nextPosition();
                                } else {
                                    result_peg_47.setPosition(position_peg_49);
                                    goto out_peg_50;
                                }
                            }
                                
                        }
                        goto success_peg_48;
                        out_peg_50:
                        {
                            int position_peg_51 = result_peg_47.getPosition();
                            
                            result_peg_47.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_47.getPosition()))){
                                    result_peg_47.nextPosition();
                                } else {
                                    result_peg_47.setPosition(position_peg_51);
                                    goto out_peg_52;
                                }
                            }
                                
                        }
                        goto success_peg_48;
                        out_peg_52:
                        goto loop_peg_46;
                        success_peg_48:
                        ;
                        result_peg_30.addResult(result_peg_47);
                    } while (true);
                    loop_peg_46:
                    ;
                            
                }
                goto success_peg_44;
                goto out_peg_41;
                success_peg_44:
                ;
            
            
            
            result_peg_30 = rule_expr_c(stream, result_peg_30.getPosition());
                if (result_peg_30.error()){
                    goto out_peg_41;
                }
                first = result_peg_30.getValues();
            
            
            
            result_peg_30.reset();
                do{
                    Result result_peg_56(result_peg_30.getPosition());
                    {
                    
                        {
                                int position_peg_59 = result_peg_56.getPosition();
                                
                                result_peg_56.reset();
                                do{
                                    Result result_peg_61(result_peg_56.getPosition());
                                    {
                                        int position_peg_63 = result_peg_61.getPosition();
                                        
                                        result_peg_61.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_61.getPosition()))){
                                                result_peg_61.nextPosition();
                                            } else {
                                                result_peg_61.setPosition(position_peg_63);
                                                goto out_peg_64;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_62;
                                    out_peg_64:
                                    {
                                        int position_peg_65 = result_peg_61.getPosition();
                                        
                                        result_peg_61.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_61.getPosition()))){
                                                result_peg_61.nextPosition();
                                            } else {
                                                result_peg_61.setPosition(position_peg_65);
                                                goto out_peg_66;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_62;
                                    out_peg_66:
                                    goto loop_peg_60;
                                    success_peg_62:
                                    ;
                                    result_peg_56.addResult(result_peg_61);
                                } while (true);
                                loop_peg_60:
                                ;
                                        
                            }
                            goto success_peg_58;
                            goto loop_peg_55;
                            success_peg_58:
                            ;
                        
                        
                        
                        result_peg_56.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_56.getPosition()))){
                                    result_peg_56.nextPosition();
                                } else {
                                    goto loop_peg_55;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_70 = result_peg_56.getPosition();
                                
                                result_peg_56.reset();
                                do{
                                    Result result_peg_72(result_peg_56.getPosition());
                                    {
                                        int position_peg_74 = result_peg_72.getPosition();
                                        
                                        result_peg_72.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_72.getPosition()))){
                                                result_peg_72.nextPosition();
                                            } else {
                                                result_peg_72.setPosition(position_peg_74);
                                                goto out_peg_75;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_73;
                                    out_peg_75:
                                    {
                                        int position_peg_76 = result_peg_72.getPosition();
                                        
                                        result_peg_72.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_72.getPosition()))){
                                                result_peg_72.nextPosition();
                                            } else {
                                                result_peg_72.setPosition(position_peg_76);
                                                goto out_peg_77;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_73;
                                    out_peg_77:
                                    goto loop_peg_71;
                                    success_peg_73:
                                    ;
                                    result_peg_56.addResult(result_peg_72);
                                } while (true);
                                loop_peg_71:
                                ;
                                        
                            }
                            goto success_peg_69;
                            goto loop_peg_55;
                            success_peg_69:
                            ;
                        
                        
                        
                        result_peg_56 = rule_expr_c(stream, result_peg_56.getPosition());
                            if (result_peg_56.error()){
                                goto loop_peg_55;
                            }
                        
                        
                    }
                    result_peg_30.addResult(result_peg_56);
                } while (true);
                loop_peg_55:
                ;
                rest = result_peg_30.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_30.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_valuelist = result_peg_30;
        stream.update(result_peg_30.getPosition());
        
        
        return result_peg_30;
        out_peg_41:
    
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk3 != 0 && column_peg_2.chunk3->chunk_expr.calculated()){
        return column_peg_2.chunk3->chunk_expr;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr_c(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            Result result_peg_4 = result_peg_3;
            
            Result result_peg_8(result_peg_3);
                {
                
                    {
                            int position_peg_11 = result_peg_8.getPosition();
                            
                            result_peg_8.reset();
                            do{
                                Result result_peg_13(result_peg_8.getPosition());
                                {
                                    int position_peg_15 = result_peg_13.getPosition();
                                    
                                    result_peg_13.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_13.getPosition()))){
                                            result_peg_13.nextPosition();
                                        } else {
                                            result_peg_13.setPosition(position_peg_15);
                                            goto out_peg_16;
                                        }
                                    }
                                        
                                }
                                goto success_peg_14;
                                out_peg_16:
                                {
                                    int position_peg_17 = result_peg_13.getPosition();
                                    
                                    result_peg_13.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_13.getPosition()))){
                                            result_peg_13.nextPosition();
                                        } else {
                                            result_peg_13.setPosition(position_peg_17);
                                            goto out_peg_18;
                                        }
                                    }
                                        
                                }
                                goto success_peg_14;
                                out_peg_18:
                                goto loop_peg_12;
                                success_peg_14:
                                ;
                                result_peg_8.addResult(result_peg_13);
                            } while (true);
                            loop_peg_12:
                            ;
                                    
                        }
                        goto success_peg_10;
                        goto not_peg_7;
                        success_peg_10:
                        ;
                    
                    
                    
                    result_peg_8.setValue(Value((void*) ","));
                        for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_8.getPosition()))){
                                result_peg_8.nextPosition();
                            } else {
                                goto not_peg_7;
                            }
                        }
                    
                    
                }
                goto out_peg_5;
                not_peg_7:
                result_peg_3.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_4.getValues();
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_19(myposition);
        
        
        {
        
            result_peg_19 = rule_valuelist(stream, result_peg_19.getPosition());
                if (result_peg_19.error()){
                    goto out_peg_21;
                }
            
            Result result_peg_20 = result_peg_19;
            
            result_peg_19 = rule_expr2_rest(stream, result_peg_19.getPosition(), result_peg_20.getValues());
                if (result_peg_19.error()){
                    goto out_peg_21;
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr = result_peg_19;
        stream.update(result_peg_19.getPosition());
        
        
        return result_peg_19;
        out_peg_21:
    
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk3 != 0 && column_peg_2.chunk3->chunk_expr_c.calculated()){
        return column_peg_2.chunk3->chunk_expr_c;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr_c");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr2(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr_c = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr_c = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk3 != 0 && column_peg_2.chunk3->chunk_expr2.calculated()){
        return column_peg_2.chunk3->chunk_expr2;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr2");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr3(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr2_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr2 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr2 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk3 != 0 && column_peg_2.chunk3->chunk_expr3.calculated()){
        return column_peg_2.chunk3->chunk_expr3;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr3");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr4(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr3_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr3 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_expr3 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk4 != 0 && column_peg_2.chunk4->chunk_expr4.calculated()){
        return column_peg_2.chunk4->chunk_expr4;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr4");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr5(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr4_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr4 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr4 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk4 != 0 && column_peg_2.chunk4->chunk_expr5.calculated()){
        return column_peg_2.chunk4->chunk_expr5;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr5");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr6(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr5_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr5 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr5 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk4 != 0 && column_peg_2.chunk4->chunk_expr6.calculated()){
        return column_peg_2.chunk4->chunk_expr6;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr6");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr7(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr6_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr6 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr6 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk4 != 0 && column_peg_2.chunk4->chunk_expr7.calculated()){
        return column_peg_2.chunk4->chunk_expr7;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr7");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr8(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr7_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr7 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr7 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk4 != 0 && column_peg_2.chunk4->chunk_expr8.calculated()){
        return column_peg_2.chunk4->chunk_expr8;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr8");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr9(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr8_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr8 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk4 == 0){
            column_peg_2.chunk4 = new Chunk4();
        }
        column_peg_2.chunk4->chunk_expr8 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk5 != 0 && column_peg_2.chunk5->chunk_expr9.calculated()){
        return column_peg_2.chunk5->chunk_expr9;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr9");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr10(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr9_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr9 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr9 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk5 != 0 && column_peg_2.chunk5->chunk_expr10.calculated()){
        return column_peg_2.chunk5->chunk_expr10;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr10");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr11(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr10_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr10 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr10 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk5 != 0 && column_peg_2.chunk5->chunk_expr11.calculated()){
        return column_peg_2.chunk5->chunk_expr11;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr11");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr12(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr11_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr11 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr11 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk5 != 0 && column_peg_2.chunk5->chunk_expr12.calculated()){
        return column_peg_2.chunk5->chunk_expr12;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr12");
    int myposition = position;
    
    
    Value left;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr13(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                left = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_expr12_rest(stream, result_peg_3.getPosition(), left);
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
        }
        
        
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr12 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr12 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk5 != 0 && column_peg_2.chunk5->chunk_expr13.calculated()){
        return column_peg_2.chunk5->chunk_expr13;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr13");
    int myposition = position;
    
    
    Value exp;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.reset();
                do{
                    Result result_peg_6(result_peg_3.getPosition());
                    {
                        int position_peg_8 = result_peg_6.getPosition();
                        {
                        
                            result_peg_6.setValue(Value((void*) "!"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("!"[i], stream.get(result_peg_6.getPosition()))){
                                        result_peg_6.nextPosition();
                                    } else {
                                        result_peg_6.setPosition(position_peg_8);
                                        goto out_peg_10;
                                    }
                                }
                            
                            
                            
                            {
                                    Value value((void*) 0);
                                    value = (void*) Ast::ExpressionUnary::Not;
                                    result_peg_6.setValue(value);
                                }
                            
                            
                        }
                    }
                    goto success_peg_7;
                    out_peg_10:
                    {
                        int position_peg_11 = result_peg_6.getPosition();
                        {
                        
                            result_peg_6.setValue(Value((void*) "-"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("-"[i], stream.get(result_peg_6.getPosition()))){
                                        result_peg_6.nextPosition();
                                    } else {
                                        result_peg_6.setPosition(position_peg_11);
                                        goto out_peg_13;
                                    }
                                }
                            
                            
                            
                            Result result_peg_16(result_peg_6);
                                {
                                    int position_peg_18 = result_peg_16.getPosition();
                                    
                                    char letter_peg_19 = stream.get(result_peg_16.getPosition());
                                    if (letter_peg_19 != '\0' && strchr("0123456789", letter_peg_19) != NULL){
                                        result_peg_16.nextPosition();
                                        result_peg_16.setValue(Value((void*) (intptr_t) letter_peg_19));
                                    } else {
                                        result_peg_16.setPosition(position_peg_18);
                                        goto out_peg_20;
                                    }
                                    
                                }
                                goto success_peg_17;
                                out_peg_20:
                                goto not_peg_15;
                                success_peg_17:
                                ;
                                result_peg_6.setPosition(position_peg_11);
                                goto out_peg_13;
                                not_peg_15:
                                result_peg_6.setValue(Value((void*)0));
                            
                            
                            
                            {
                                    Value value((void*) 0);
                                    value = (void*) Ast::ExpressionUnary::Minus;
                                    result_peg_6.setValue(value);
                                }
                            
                            
                        }
                    }
                    goto success_peg_7;
                    out_peg_13:
                    {
                        int position_peg_21 = result_peg_6.getPosition();
                        {
                        
                            result_peg_6.setValue(Value((void*) "~"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("~"[i], stream.get(result_peg_6.getPosition()))){
                                        result_peg_6.nextPosition();
                                    } else {
                                        result_peg_6.setPosition(position_peg_21);
                                        goto out_peg_23;
                                    }
                                }
                            
                            
                            
                            {
                                    Value value((void*) 0);
                                    value = (void*) Ast::ExpressionUnary::Negation;
                                    result_peg_6.setValue(value);
                                }
                            
                            
                        }
                    }
                    goto success_peg_7;
                    out_peg_23:
                    goto loop_peg_5;
                    success_peg_7:
                    ;
                    result_peg_3.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                ;
            
            Result result_peg_4 = result_peg_3;
            
            {
                    int position_peg_26 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_28(result_peg_3.getPosition());
                        {
                            int position_peg_30 = result_peg_28.getPosition();
                            
                            result_peg_28.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_28.getPosition()))){
                                    result_peg_28.nextPosition();
                                } else {
                                    result_peg_28.setPosition(position_peg_30);
                                    goto out_peg_31;
                                }
                            }
                                
                        }
                        goto success_peg_29;
                        out_peg_31:
                        {
                            int position_peg_32 = result_peg_28.getPosition();
                            
                            result_peg_28.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_28.getPosition()))){
                                    result_peg_28.nextPosition();
                                } else {
                                    result_peg_28.setPosition(position_peg_32);
                                    goto out_peg_33;
                                }
                            }
                                
                        }
                        goto success_peg_29;
                        out_peg_33:
                        goto loop_peg_27;
                        success_peg_29:
                        ;
                        result_peg_3.addResult(result_peg_28);
                    } while (true);
                    loop_peg_27:
                    ;
                            
                }
                goto success_peg_25;
                goto out_peg_34;
                success_peg_25:
                ;
            
            
            
            result_peg_3 = rule_expr13_real(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_34;
                }
                exp = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeUnaryExpression(result_peg_4.getValues(), exp);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr13 = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_34:
    
        if (column_peg_2.chunk5 == 0){
            column_peg_2.chunk5 = new Chunk5();
        }
        column_peg_2.chunk5->chunk_expr13 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "||"));
                for (int i = 0; i < 2; i++){
                    if (compareChar("||"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr2(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionOr(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr2_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "^^"));
                for (int i = 0; i < 2; i++){
                    if (compareChar("^^"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr3(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionXOr(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr3_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "&&"));
                for (int i = 0; i < 2; i++){
                    if (compareChar("&&"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr4(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAnd(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr4_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "|"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("|"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr5(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseOr(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr5_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "^"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("^"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr6(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseXOr(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr6_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "&"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("&"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr7(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseAnd(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr7_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ":="));
                for (int i = 0; i < 2; i++){
                    if (compareChar(":="[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr8(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAssignment(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr8_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr9(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionEquals(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        {
        
            {
                    int position_peg_32 = result_peg_29.getPosition();
                    
                    result_peg_29.reset();
                    do{
                        Result result_peg_34(result_peg_29.getPosition());
                        {
                            int position_peg_36 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_36);
                                    goto out_peg_37;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            int position_peg_38 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_38);
                                    goto out_peg_39;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_29.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_31;
                goto out_peg_40;
                success_peg_31:
                ;
            
            
            
            result_peg_29.setValue(Value((void*) "!="));
                for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_29.getPosition()))){
                        result_peg_29.nextPosition();
                    } else {
                        goto out_peg_40;
                    }
                }
            
            
            
            {
                    int position_peg_44 = result_peg_29.getPosition();
                    
                    result_peg_29.reset();
                    do{
                        Result result_peg_46(result_peg_29.getPosition());
                        {
                            int position_peg_48 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_48);
                                    goto out_peg_49;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_49:
                        {
                            int position_peg_50 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_51:
                        goto loop_peg_45;
                        success_peg_47:
                        ;
                        result_peg_29.addResult(result_peg_46);
                    } while (true);
                    loop_peg_45:
                    ;
                            
                }
                goto success_peg_43;
                goto out_peg_40;
                success_peg_43:
                ;
            
            
            
            result_peg_29 = rule_expr9(stream, result_peg_29.getPosition());
                if (result_peg_29.error()){
                    goto out_peg_40;
                }
                right = result_peg_29.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionUnequals(left, right);
                    result_peg_29.setValue(value);
                }
                new_left = result_peg_29.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_29.getPosition();
        goto tail_peg_4;
        out_peg_40:
        Result result_peg_54(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_54.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_54.getPosition());
        
        return result_peg_54;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr9_rest");
    int myposition = position;
    
    tail_peg_4:
    Value maker;
        Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            {
                    int position_peg_18 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) "<="));
                            for (int i = 0; i < 2; i++){
                                if (compareChar("<="[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_18);
                                    goto out_peg_20;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = (void *) makeExpressionLessThanEquals;
                                result_peg_3.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_17;
                out_peg_20:
                {
                    int position_peg_21 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) ">="));
                            for (int i = 0; i < 2; i++){
                                if (compareChar(">="[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_21);
                                    goto out_peg_23;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = (void *) makeExpressionGreaterThanEquals;
                                result_peg_3.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_17;
                out_peg_23:
                {
                    int position_peg_24 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) "<"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("<"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_24);
                                    goto out_peg_26;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = (void *) makeExpressionLessThan;
                                result_peg_3.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_17;
                out_peg_26:
                {
                    int position_peg_27 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) ">"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(">"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_27);
                                    goto out_peg_29;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = (void *) makeExpressionGreaterThan;
                                result_peg_3.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_17;
                out_peg_29:
                goto out_peg_15;
                success_peg_17:
                ;
                maker = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_32 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_34(result_peg_3.getPosition());
                        {
                            int position_peg_36 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_36);
                                    goto out_peg_37;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            int position_peg_38 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_38);
                                    goto out_peg_39;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_3.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_31;
                goto out_peg_15;
                success_peg_31:
                ;
            
            
            
            result_peg_3 = rule_expr10(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    {
                            typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                            value = as<compare_func>(maker)(left, right);
                            }
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
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
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk7 != 0 && column_peg_2.chunk7->chunk_all_compare.calculated()){
        return column_peg_2.chunk7->chunk_all_compare;
    }
    
    
    RuleTrace trace_peg_1(stream, "all_compare");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
            int position_peg_5 = result_peg_3.getPosition();
            {
            
                result_peg_3.setValue(Value((void*) "<="));
                    for (int i = 0; i < 2; i++){
                        if (compareChar("<="[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_5);
                            goto out_peg_7;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = (void *) makeExpressionLessThanEquals;
                        result_peg_3.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_4;
        out_peg_7:
        {
            int position_peg_8 = result_peg_3.getPosition();
            {
            
                result_peg_3.setValue(Value((void*) ">="));
                    for (int i = 0; i < 2; i++){
                        if (compareChar(">="[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_8);
                            goto out_peg_10;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = (void *) makeExpressionGreaterThanEquals;
                        result_peg_3.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_4;
        out_peg_10:
        {
            int position_peg_11 = result_peg_3.getPosition();
            {
            
                result_peg_3.setValue(Value((void*) "<"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("<"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_11);
                            goto out_peg_13;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = (void *) makeExpressionLessThan;
                        result_peg_3.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_4;
        out_peg_13:
        {
            int position_peg_14 = result_peg_3.getPosition();
            {
            
                result_peg_3.setValue(Value((void*) ">"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar(">"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_14);
                            goto out_peg_16;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = (void *) makeExpressionGreaterThan;
                        result_peg_3.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_4;
        out_peg_16:
        goto out_peg_17;
        success_peg_4:
        ;
        
        
        if (column_peg_2.chunk7 == 0){
            column_peg_2.chunk7 = new Chunk7();
        }
        column_peg_2.chunk7->chunk_all_compare = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_17:
        Result result_peg_18(myposition);
        
        
        {
            int position_peg_20 = result_peg_18.getPosition();
            {
            
                result_peg_18.setValue(Value((void*) "="));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("="[i], stream.get(result_peg_18.getPosition()))){
                            result_peg_18.nextPosition();
                        } else {
                            result_peg_18.setPosition(position_peg_20);
                            goto out_peg_22;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = (void*) makeExpressionEquals;
                        result_peg_18.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_19;
        out_peg_22:
        {
            int position_peg_23 = result_peg_18.getPosition();
            {
            
                result_peg_18.setValue(Value((void*) "!="));
                    for (int i = 0; i < 2; i++){
                        if (compareChar("!="[i], stream.get(result_peg_18.getPosition()))){
                            result_peg_18.nextPosition();
                        } else {
                            result_peg_18.setPosition(position_peg_23);
                            goto out_peg_25;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = (void*) makeExpressionUnequals;
                        result_peg_18.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_19;
        out_peg_25:
        goto out_peg_26;
        success_peg_19:
        ;
        
        
        if (column_peg_2.chunk7 == 0){
            column_peg_2.chunk7 = new Chunk7();
        }
        column_peg_2.chunk7->chunk_all_compare = result_peg_18;
        stream.update(result_peg_18.getPosition());
        
        
        return result_peg_18;
        out_peg_26:
    
        if (column_peg_2.chunk7 == 0){
            column_peg_2.chunk7 = new Chunk7();
        }
        column_peg_2.chunk7->chunk_all_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr10_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "+"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr11(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAdd(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        {
        
            {
                    int position_peg_32 = result_peg_29.getPosition();
                    
                    result_peg_29.reset();
                    do{
                        Result result_peg_34(result_peg_29.getPosition());
                        {
                            int position_peg_36 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_36);
                                    goto out_peg_37;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            int position_peg_38 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_38);
                                    goto out_peg_39;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_29.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_31;
                goto out_peg_40;
                success_peg_31:
                ;
            
            
            
            result_peg_29.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_29.getPosition()))){
                        result_peg_29.nextPosition();
                    } else {
                        goto out_peg_40;
                    }
                }
            
            
            
            {
                    int position_peg_44 = result_peg_29.getPosition();
                    
                    result_peg_29.reset();
                    do{
                        Result result_peg_46(result_peg_29.getPosition());
                        {
                            int position_peg_48 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_48);
                                    goto out_peg_49;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_49:
                        {
                            int position_peg_50 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_51:
                        goto loop_peg_45;
                        success_peg_47:
                        ;
                        result_peg_29.addResult(result_peg_46);
                    } while (true);
                    loop_peg_45:
                    ;
                            
                }
                goto success_peg_43;
                goto out_peg_40;
                success_peg_43:
                ;
            
            
            
            result_peg_29 = rule_expr11(stream, result_peg_29.getPosition());
                if (result_peg_29.error()){
                    goto out_peg_40;
                }
                right = result_peg_29.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionSubtract(left, right);
                    result_peg_29.setValue(value);
                }
                new_left = result_peg_29.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_29.getPosition();
        goto tail_peg_4;
        out_peg_40:
        Result result_peg_54(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_54.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_54.getPosition());
        
        return result_peg_54;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr11_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "*"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("*"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr12(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionMultiply(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        {
        
            {
                    int position_peg_32 = result_peg_29.getPosition();
                    
                    result_peg_29.reset();
                    do{
                        Result result_peg_34(result_peg_29.getPosition());
                        {
                            int position_peg_36 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_36);
                                    goto out_peg_37;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            int position_peg_38 = result_peg_34.getPosition();
                            
                            result_peg_34.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    result_peg_34.setPosition(position_peg_38);
                                    goto out_peg_39;
                                }
                            }
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_29.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_31;
                goto out_peg_40;
                success_peg_31:
                ;
            
            
            
            result_peg_29.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_29.getPosition()))){
                        result_peg_29.nextPosition();
                    } else {
                        goto out_peg_40;
                    }
                }
            
            
            
            {
                    int position_peg_44 = result_peg_29.getPosition();
                    
                    result_peg_29.reset();
                    do{
                        Result result_peg_46(result_peg_29.getPosition());
                        {
                            int position_peg_48 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_48);
                                    goto out_peg_49;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_49:
                        {
                            int position_peg_50 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_51:
                        goto loop_peg_45;
                        success_peg_47:
                        ;
                        result_peg_29.addResult(result_peg_46);
                    } while (true);
                    loop_peg_45:
                    ;
                            
                }
                goto success_peg_43;
                goto out_peg_40;
                success_peg_43:
                ;
            
            
            
            result_peg_29 = rule_expr12(stream, result_peg_29.getPosition());
                if (result_peg_29.error()){
                    goto out_peg_40;
                }
                right = result_peg_29.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionDivide(left, right);
                    result_peg_29.setValue(value);
                }
                new_left = result_peg_29.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_29.getPosition();
        goto tail_peg_4;
        out_peg_40:
        Result result_peg_54(myposition);
        {
        
            {
                    int position_peg_57 = result_peg_54.getPosition();
                    
                    result_peg_54.reset();
                    do{
                        Result result_peg_59(result_peg_54.getPosition());
                        {
                            int position_peg_61 = result_peg_59.getPosition();
                            
                            result_peg_59.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_59.getPosition()))){
                                    result_peg_59.nextPosition();
                                } else {
                                    result_peg_59.setPosition(position_peg_61);
                                    goto out_peg_62;
                                }
                            }
                                
                        }
                        goto success_peg_60;
                        out_peg_62:
                        {
                            int position_peg_63 = result_peg_59.getPosition();
                            
                            result_peg_59.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_59.getPosition()))){
                                    result_peg_59.nextPosition();
                                } else {
                                    result_peg_59.setPosition(position_peg_63);
                                    goto out_peg_64;
                                }
                            }
                                
                        }
                        goto success_peg_60;
                        out_peg_64:
                        goto loop_peg_58;
                        success_peg_60:
                        ;
                        result_peg_54.addResult(result_peg_59);
                    } while (true);
                    loop_peg_58:
                    ;
                            
                }
                goto success_peg_56;
                goto out_peg_65;
                success_peg_56:
                ;
            
            
            
            result_peg_54.setValue(Value((void*) "%"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("%"[i], stream.get(result_peg_54.getPosition()))){
                        result_peg_54.nextPosition();
                    } else {
                        goto out_peg_65;
                    }
                }
            
            
            
            {
                    int position_peg_69 = result_peg_54.getPosition();
                    
                    result_peg_54.reset();
                    do{
                        Result result_peg_71(result_peg_54.getPosition());
                        {
                            int position_peg_73 = result_peg_71.getPosition();
                            
                            result_peg_71.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_71.getPosition()))){
                                    result_peg_71.nextPosition();
                                } else {
                                    result_peg_71.setPosition(position_peg_73);
                                    goto out_peg_74;
                                }
                            }
                                
                        }
                        goto success_peg_72;
                        out_peg_74:
                        {
                            int position_peg_75 = result_peg_71.getPosition();
                            
                            result_peg_71.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_71.getPosition()))){
                                    result_peg_71.nextPosition();
                                } else {
                                    result_peg_71.setPosition(position_peg_75);
                                    goto out_peg_76;
                                }
                            }
                                
                        }
                        goto success_peg_72;
                        out_peg_76:
                        goto loop_peg_70;
                        success_peg_72:
                        ;
                        result_peg_54.addResult(result_peg_71);
                    } while (true);
                    loop_peg_70:
                    ;
                            
                }
                goto success_peg_68;
                goto out_peg_65;
                success_peg_68:
                ;
            
            
            
            result_peg_54 = rule_expr12(stream, result_peg_54.getPosition());
                if (result_peg_54.error()){
                    goto out_peg_65;
                }
                right = result_peg_54.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionModulo(left, right);
                    result_peg_54.setValue(value);
                }
                new_left = result_peg_54.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_54.getPosition();
        goto tail_peg_4;
        out_peg_65:
        Result result_peg_79(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_79.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_79.getPosition());
        
        return result_peg_79;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "expr12_rest");
    int myposition = position;
    
    tail_peg_4:
    Value right;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "**"));
                for (int i = 0; i < 2; i++){
                    if (compareChar("**"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_expr13(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionPower(left, right);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk8 != 0 && column_peg_2.chunk8->chunk_expr13_real.calculated()){
        return column_peg_2.chunk8->chunk_expr13_real;
    }
    
    
    RuleTrace trace_peg_1(stream, "expr13_real");
    int myposition = position;
    
    
    Value e;
    Result result_peg_3(myposition);
        
        
        result_peg_3 = rule_helper(stream, result_peg_3.getPosition());
        if (result_peg_3.error()){
            goto out_peg_4;
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_expr13_real = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_4:
        Result result_peg_5(myposition);
        
        
        result_peg_5 = rule_function(stream, result_peg_5.getPosition());
        if (result_peg_5.error()){
            goto out_peg_6;
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_expr13_real = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_6:
        Result result_peg_7(myposition);
        
        
        result_peg_7 = rule_value(stream, result_peg_7.getPosition());
        if (result_peg_7.error()){
            goto out_peg_8;
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_expr13_real = result_peg_7;
        stream.update(result_peg_7.getPosition());
        
        
        return result_peg_7;
        out_peg_8:
        Result result_peg_9(myposition);
        
        
        {
        
            result_peg_9.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_9.getPosition()))){
                        result_peg_9.nextPosition();
                    } else {
                        goto out_peg_11;
                    }
                }
            
            
            
            {
                    int position_peg_14 = result_peg_9.getPosition();
                    
                    result_peg_9.reset();
                    do{
                        Result result_peg_16(result_peg_9.getPosition());
                        {
                            int position_peg_18 = result_peg_16.getPosition();
                            
                            result_peg_16.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    result_peg_16.setPosition(position_peg_18);
                                    goto out_peg_19;
                                }
                            }
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            int position_peg_20 = result_peg_16.getPosition();
                            
                            result_peg_16.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    result_peg_16.setPosition(position_peg_20);
                                    goto out_peg_21;
                                }
                            }
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_9.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_13;
                goto out_peg_11;
                success_peg_13:
                ;
            
            
            
            result_peg_9 = rule_expr(stream, result_peg_9.getPosition());
                if (result_peg_9.error()){
                    goto out_peg_11;
                }
                e = result_peg_9.getValues();
            
            
            
            {
                    int position_peg_25 = result_peg_9.getPosition();
                    
                    result_peg_9.reset();
                    do{
                        Result result_peg_27(result_peg_9.getPosition());
                        {
                            int position_peg_29 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_29);
                                    goto out_peg_30;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_30:
                        {
                            int position_peg_31 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_31);
                                    goto out_peg_32;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_32:
                        goto loop_peg_26;
                        success_peg_28:
                        ;
                        result_peg_9.addResult(result_peg_27);
                    } while (true);
                    loop_peg_26:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_11;
                success_peg_24:
                ;
            
            
            
            result_peg_9.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_9.getPosition()))){
                        result_peg_9.nextPosition();
                    } else {
                        goto out_peg_11;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = e;
                    result_peg_9.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_expr13_real = result_peg_9;
        stream.update(result_peg_9.getPosition());
        
        
        return result_peg_9;
        out_peg_11:
    
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_expr13_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk8 != 0 && column_peg_2.chunk8->chunk_function.calculated()){
        return column_peg_2.chunk8->chunk_function;
    }
    
    
    RuleTrace trace_peg_1(stream, "function");
    int myposition = position;
    
    
    Value name;
        Value arg1;
        Value line;
        Value comparison;
        Value arg2;
        Value compare;
        Value mod;
        Value to;
        Value id;
        Value ticks;
        Value state;
        Value attributes;
        Value args;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "teammode"));
                for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
                name = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_5;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_identifier(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                arg1 = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(name, arg1);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_28(myposition);
        
        
        {
        
            result_peg_28.setValue(Value((void*) "teammode"));
                for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_30;
                    }
                }
                name = result_peg_28.getValues();
            
            
            
            {
                    int position_peg_33 = result_peg_28.getPosition();
                    
                    result_peg_28.reset();
                    do{
                        Result result_peg_35(result_peg_28.getPosition());
                        {
                            int position_peg_37 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_37);
                                    goto out_peg_38;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_38:
                        {
                            int position_peg_39 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_39);
                                    goto out_peg_40;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_40:
                        goto loop_peg_34;
                        success_peg_36:
                        ;
                        result_peg_28.addResult(result_peg_35);
                    } while (true);
                    loop_peg_34:
                    ;
                            
                }
                goto success_peg_32;
                goto out_peg_30;
                success_peg_32:
                ;
            
            
            
            result_peg_28.setValue(Value((void*) "!="));
                for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_30;
                    }
                }
            
            
            
            {
                    int position_peg_44 = result_peg_28.getPosition();
                    
                    result_peg_28.reset();
                    do{
                        Result result_peg_46(result_peg_28.getPosition());
                        {
                            int position_peg_48 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_48);
                                    goto out_peg_49;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_49:
                        {
                            int position_peg_50 = result_peg_46.getPosition();
                            
                            result_peg_46.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_46.getPosition()))){
                                    result_peg_46.nextPosition();
                                } else {
                                    result_peg_46.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_47;
                        out_peg_51:
                        goto loop_peg_45;
                        success_peg_47:
                        ;
                        result_peg_28.addResult(result_peg_46);
                    } while (true);
                    loop_peg_45:
                    ;
                            
                }
                goto success_peg_43;
                goto out_peg_30;
                success_peg_43:
                ;
            
            
            
            result_peg_28 = rule_identifier(stream, result_peg_28.getPosition());
                if (result_peg_28.error()){
                    goto out_peg_30;
                }
                arg1 = result_peg_28.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(std::string("teammode!="), arg1);
                    result_peg_28.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_30:
        Result result_peg_53(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_55 = stream.getLineInfo(result_peg_53.getPosition());
                line = &line_info_peg_55;
            
            
            
            result_peg_53.setValue(Value((void*) "animelem"));
                for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_53.getPosition()))){
                        result_peg_53.nextPosition();
                    } else {
                        goto out_peg_57;
                    }
                }
            
            
            
            {
                    int position_peg_60 = result_peg_53.getPosition();
                    
                    result_peg_53.reset();
                    do{
                        Result result_peg_62(result_peg_53.getPosition());
                        {
                            int position_peg_64 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_64);
                                    goto out_peg_65;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_65:
                        {
                            int position_peg_66 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_66);
                                    goto out_peg_67;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_67:
                        goto loop_peg_61;
                        success_peg_63:
                        ;
                        result_peg_53.addResult(result_peg_62);
                    } while (true);
                    loop_peg_61:
                    ;
                            
                }
                goto success_peg_59;
                goto out_peg_57;
                success_peg_59:
                ;
            
            
            
            result_peg_53 = rule_all_compare(stream, result_peg_53.getPosition());
                if (result_peg_53.error()){
                    goto out_peg_57;
                }
            
            
            
            {
                    int position_peg_71 = result_peg_53.getPosition();
                    
                    result_peg_53.reset();
                    do{
                        Result result_peg_73(result_peg_53.getPosition());
                        {
                            int position_peg_75 = result_peg_73.getPosition();
                            
                            result_peg_73.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    result_peg_73.setPosition(position_peg_75);
                                    goto out_peg_76;
                                }
                            }
                                
                        }
                        goto success_peg_74;
                        out_peg_76:
                        {
                            int position_peg_77 = result_peg_73.getPosition();
                            
                            result_peg_73.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    result_peg_73.setPosition(position_peg_77);
                                    goto out_peg_78;
                                }
                            }
                                
                        }
                        goto success_peg_74;
                        out_peg_78:
                        goto loop_peg_72;
                        success_peg_74:
                        ;
                        result_peg_53.addResult(result_peg_73);
                    } while (true);
                    loop_peg_72:
                    ;
                            
                }
                goto success_peg_70;
                goto out_peg_57;
                success_peg_70:
                ;
            
            
            
            result_peg_53 = rule_integer(stream, result_peg_53.getPosition());
                if (result_peg_53.error()){
                    goto out_peg_57;
                }
                arg1 = result_peg_53.getValues();
            
            
            
            {
                    int position_peg_82 = result_peg_53.getPosition();
                    
                    result_peg_53.reset();
                    do{
                        Result result_peg_84(result_peg_53.getPosition());
                        {
                            int position_peg_86 = result_peg_84.getPosition();
                            
                            result_peg_84.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    result_peg_84.setPosition(position_peg_86);
                                    goto out_peg_87;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_87:
                        {
                            int position_peg_88 = result_peg_84.getPosition();
                            
                            result_peg_84.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    result_peg_84.setPosition(position_peg_88);
                                    goto out_peg_89;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_89:
                        goto loop_peg_83;
                        success_peg_85:
                        ;
                        result_peg_53.addResult(result_peg_84);
                    } while (true);
                    loop_peg_83:
                    ;
                            
                }
                goto success_peg_81;
                goto out_peg_57;
                success_peg_81:
                ;
            
            
            
            result_peg_53.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_53.getPosition()))){
                        result_peg_53.nextPosition();
                    } else {
                        goto out_peg_57;
                    }
                }
            
            
            
            {
                    int position_peg_93 = result_peg_53.getPosition();
                    
                    result_peg_53.reset();
                    do{
                        Result result_peg_95(result_peg_53.getPosition());
                        {
                            int position_peg_97 = result_peg_95.getPosition();
                            
                            result_peg_95.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    result_peg_95.setPosition(position_peg_97);
                                    goto out_peg_98;
                                }
                            }
                                
                        }
                        goto success_peg_96;
                        out_peg_98:
                        {
                            int position_peg_99 = result_peg_95.getPosition();
                            
                            result_peg_95.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    result_peg_95.setPosition(position_peg_99);
                                    goto out_peg_100;
                                }
                            }
                                
                        }
                        goto success_peg_96;
                        out_peg_100:
                        goto loop_peg_94;
                        success_peg_96:
                        ;
                        result_peg_53.addResult(result_peg_95);
                    } while (true);
                    loop_peg_94:
                    ;
                            
                }
                goto success_peg_92;
                goto out_peg_57;
                success_peg_92:
                ;
            
            
            
            int save_peg_102 = result_peg_53.getPosition();
                
                result_peg_53 = rule_all_compare(stream, result_peg_53.getPosition());
                if (result_peg_53.error()){
                    
                    result_peg_53 = Result(save_peg_102);
                    result_peg_53.setValue(Value((void*) 0));
                    
                }
                comparison = result_peg_53.getValues();
            
            
            
            {
                    int position_peg_105 = result_peg_53.getPosition();
                    
                    result_peg_53.reset();
                    do{
                        Result result_peg_107(result_peg_53.getPosition());
                        {
                            int position_peg_109 = result_peg_107.getPosition();
                            
                            result_peg_107.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_107.getPosition()))){
                                    result_peg_107.nextPosition();
                                } else {
                                    result_peg_107.setPosition(position_peg_109);
                                    goto out_peg_110;
                                }
                            }
                                
                        }
                        goto success_peg_108;
                        out_peg_110:
                        {
                            int position_peg_111 = result_peg_107.getPosition();
                            
                            result_peg_107.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_107.getPosition()))){
                                    result_peg_107.nextPosition();
                                } else {
                                    result_peg_107.setPosition(position_peg_111);
                                    goto out_peg_112;
                                }
                            }
                                
                        }
                        goto success_peg_108;
                        out_peg_112:
                        goto loop_peg_106;
                        success_peg_108:
                        ;
                        result_peg_53.addResult(result_peg_107);
                    } while (true);
                    loop_peg_106:
                    ;
                            
                }
                goto success_peg_104;
                goto out_peg_57;
                success_peg_104:
                ;
            
            
            
            result_peg_53 = rule_integer(stream, result_peg_53.getPosition());
                if (result_peg_53.error()){
                    goto out_peg_57;
                }
                arg2 = result_peg_53.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    if (comparison.getValue() == NULL){
                                        comparison = (void*) makeExpressionEquals;
                                    }
                                    value = makeAnimElem(line, arg1, comparison, arg2);
                    result_peg_53.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_53;
        stream.update(result_peg_53.getPosition());
        
        
        return result_peg_53;
        out_peg_57:
        Result result_peg_114(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_116 = stream.getLineInfo(result_peg_114.getPosition());
                line = &line_info_peg_116;
            
            
            
            result_peg_114.setValue(Value((void*) "animelem"));
                for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_114.getPosition()))){
                        result_peg_114.nextPosition();
                    } else {
                        goto out_peg_118;
                    }
                }
            
            
            
            {
                    int position_peg_121 = result_peg_114.getPosition();
                    
                    result_peg_114.reset();
                    do{
                        Result result_peg_123(result_peg_114.getPosition());
                        {
                            int position_peg_125 = result_peg_123.getPosition();
                            
                            result_peg_123.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_123.getPosition()))){
                                    result_peg_123.nextPosition();
                                } else {
                                    result_peg_123.setPosition(position_peg_125);
                                    goto out_peg_126;
                                }
                            }
                                
                        }
                        goto success_peg_124;
                        out_peg_126:
                        {
                            int position_peg_127 = result_peg_123.getPosition();
                            
                            result_peg_123.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_123.getPosition()))){
                                    result_peg_123.nextPosition();
                                } else {
                                    result_peg_123.setPosition(position_peg_127);
                                    goto out_peg_128;
                                }
                            }
                                
                        }
                        goto success_peg_124;
                        out_peg_128:
                        goto loop_peg_122;
                        success_peg_124:
                        ;
                        result_peg_114.addResult(result_peg_123);
                    } while (true);
                    loop_peg_122:
                    ;
                            
                }
                goto success_peg_120;
                goto out_peg_118;
                success_peg_120:
                ;
            
            
            
            result_peg_114 = rule_all_compare(stream, result_peg_114.getPosition());
                if (result_peg_114.error()){
                    goto out_peg_118;
                }
            
            
            
            {
                    int position_peg_132 = result_peg_114.getPosition();
                    
                    result_peg_114.reset();
                    do{
                        Result result_peg_134(result_peg_114.getPosition());
                        {
                            int position_peg_136 = result_peg_134.getPosition();
                            
                            result_peg_134.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_134.getPosition()))){
                                    result_peg_134.nextPosition();
                                } else {
                                    result_peg_134.setPosition(position_peg_136);
                                    goto out_peg_137;
                                }
                            }
                                
                        }
                        goto success_peg_135;
                        out_peg_137:
                        {
                            int position_peg_138 = result_peg_134.getPosition();
                            
                            result_peg_134.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_134.getPosition()))){
                                    result_peg_134.nextPosition();
                                } else {
                                    result_peg_134.setPosition(position_peg_138);
                                    goto out_peg_139;
                                }
                            }
                                
                        }
                        goto success_peg_135;
                        out_peg_139:
                        goto loop_peg_133;
                        success_peg_135:
                        ;
                        result_peg_114.addResult(result_peg_134);
                    } while (true);
                    loop_peg_133:
                    ;
                            
                }
                goto success_peg_131;
                goto out_peg_118;
                success_peg_131:
                ;
            
            
            
            result_peg_114 = rule_integer(stream, result_peg_114.getPosition());
                if (result_peg_114.error()){
                    goto out_peg_118;
                }
                arg1 = result_peg_114.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAnimElem(line, arg1);
                    result_peg_114.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_114;
        stream.update(result_peg_114.getPosition());
        
        
        return result_peg_114;
        out_peg_118:
        Result result_peg_141(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_143 = stream.getLineInfo(result_peg_141.getPosition());
                line = &line_info_peg_143;
            
            
            
            result_peg_141.setValue(Value((void*) "timemod"));
                for (int i = 0; i < 7; i++){
                    if (compareCharCase("timemod"[i], stream.get(result_peg_141.getPosition()))){
                        result_peg_141.nextPosition();
                    } else {
                        goto out_peg_145;
                    }
                }
            
            
            
            {
                    int position_peg_148 = result_peg_141.getPosition();
                    
                    result_peg_141.reset();
                    do{
                        Result result_peg_150(result_peg_141.getPosition());
                        {
                            int position_peg_152 = result_peg_150.getPosition();
                            
                            result_peg_150.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_150.getPosition()))){
                                    result_peg_150.nextPosition();
                                } else {
                                    result_peg_150.setPosition(position_peg_152);
                                    goto out_peg_153;
                                }
                            }
                                
                        }
                        goto success_peg_151;
                        out_peg_153:
                        {
                            int position_peg_154 = result_peg_150.getPosition();
                            
                            result_peg_150.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_150.getPosition()))){
                                    result_peg_150.nextPosition();
                                } else {
                                    result_peg_150.setPosition(position_peg_154);
                                    goto out_peg_155;
                                }
                            }
                                
                        }
                        goto success_peg_151;
                        out_peg_155:
                        goto loop_peg_149;
                        success_peg_151:
                        ;
                        result_peg_141.addResult(result_peg_150);
                    } while (true);
                    loop_peg_149:
                    ;
                            
                }
                goto success_peg_147;
                goto out_peg_145;
                success_peg_147:
                ;
            
            
            
            result_peg_141 = rule_all_compare(stream, result_peg_141.getPosition());
                if (result_peg_141.error()){
                    goto out_peg_145;
                }
                compare = result_peg_141.getValues();
            
            
            
            {
                    int position_peg_159 = result_peg_141.getPosition();
                    
                    result_peg_141.reset();
                    do{
                        Result result_peg_161(result_peg_141.getPosition());
                        {
                            int position_peg_163 = result_peg_161.getPosition();
                            
                            result_peg_161.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_161.getPosition()))){
                                    result_peg_161.nextPosition();
                                } else {
                                    result_peg_161.setPosition(position_peg_163);
                                    goto out_peg_164;
                                }
                            }
                                
                        }
                        goto success_peg_162;
                        out_peg_164:
                        {
                            int position_peg_165 = result_peg_161.getPosition();
                            
                            result_peg_161.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_161.getPosition()))){
                                    result_peg_161.nextPosition();
                                } else {
                                    result_peg_161.setPosition(position_peg_165);
                                    goto out_peg_166;
                                }
                            }
                                
                        }
                        goto success_peg_162;
                        out_peg_166:
                        goto loop_peg_160;
                        success_peg_162:
                        ;
                        result_peg_141.addResult(result_peg_161);
                    } while (true);
                    loop_peg_160:
                    ;
                            
                }
                goto success_peg_158;
                goto out_peg_145;
                success_peg_158:
                ;
            
            
            
            result_peg_141 = rule_integer(stream, result_peg_141.getPosition());
                if (result_peg_141.error()){
                    goto out_peg_145;
                }
                mod = result_peg_141.getValues();
            
            
            
            {
                    int position_peg_170 = result_peg_141.getPosition();
                    
                    result_peg_141.reset();
                    do{
                        Result result_peg_172(result_peg_141.getPosition());
                        {
                            int position_peg_174 = result_peg_172.getPosition();
                            
                            result_peg_172.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_172.getPosition()))){
                                    result_peg_172.nextPosition();
                                } else {
                                    result_peg_172.setPosition(position_peg_174);
                                    goto out_peg_175;
                                }
                            }
                                
                        }
                        goto success_peg_173;
                        out_peg_175:
                        {
                            int position_peg_176 = result_peg_172.getPosition();
                            
                            result_peg_172.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_172.getPosition()))){
                                    result_peg_172.nextPosition();
                                } else {
                                    result_peg_172.setPosition(position_peg_176);
                                    goto out_peg_177;
                                }
                            }
                                
                        }
                        goto success_peg_173;
                        out_peg_177:
                        goto loop_peg_171;
                        success_peg_173:
                        ;
                        result_peg_141.addResult(result_peg_172);
                    } while (true);
                    loop_peg_171:
                    ;
                            
                }
                goto success_peg_169;
                goto out_peg_145;
                success_peg_169:
                ;
            
            
            
            result_peg_141.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_141.getPosition()))){
                        result_peg_141.nextPosition();
                    } else {
                        goto out_peg_145;
                    }
                }
            
            
            
            {
                    int position_peg_181 = result_peg_141.getPosition();
                    
                    result_peg_141.reset();
                    do{
                        Result result_peg_183(result_peg_141.getPosition());
                        {
                            int position_peg_185 = result_peg_183.getPosition();
                            
                            result_peg_183.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_183.getPosition()))){
                                    result_peg_183.nextPosition();
                                } else {
                                    result_peg_183.setPosition(position_peg_185);
                                    goto out_peg_186;
                                }
                            }
                                
                        }
                        goto success_peg_184;
                        out_peg_186:
                        {
                            int position_peg_187 = result_peg_183.getPosition();
                            
                            result_peg_183.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_183.getPosition()))){
                                    result_peg_183.nextPosition();
                                } else {
                                    result_peg_183.setPosition(position_peg_187);
                                    goto out_peg_188;
                                }
                            }
                                
                        }
                        goto success_peg_184;
                        out_peg_188:
                        goto loop_peg_182;
                        success_peg_184:
                        ;
                        result_peg_141.addResult(result_peg_183);
                    } while (true);
                    loop_peg_182:
                    ;
                            
                }
                goto success_peg_180;
                goto out_peg_145;
                success_peg_180:
                ;
            
            
            
            int save_peg_190 = result_peg_141.getPosition();
                
                result_peg_141 = rule_all_compare(stream, result_peg_141.getPosition());
                if (result_peg_141.error()){
                    
                    result_peg_141 = Result(save_peg_190);
                    result_peg_141.setValue(Value((void*) 0));
                    
                }
            
            
            
            {
                    int position_peg_193 = result_peg_141.getPosition();
                    
                    result_peg_141.reset();
                    do{
                        Result result_peg_195(result_peg_141.getPosition());
                        {
                            int position_peg_197 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_195.getPosition()))){
                                    result_peg_195.nextPosition();
                                } else {
                                    result_peg_195.setPosition(position_peg_197);
                                    goto out_peg_198;
                                }
                            }
                                
                        }
                        goto success_peg_196;
                        out_peg_198:
                        {
                            int position_peg_199 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_195.getPosition()))){
                                    result_peg_195.nextPosition();
                                } else {
                                    result_peg_195.setPosition(position_peg_199);
                                    goto out_peg_200;
                                }
                            }
                                
                        }
                        goto success_peg_196;
                        out_peg_200:
                        goto loop_peg_194;
                        success_peg_196:
                        ;
                        result_peg_141.addResult(result_peg_195);
                    } while (true);
                    loop_peg_194:
                    ;
                            
                }
                goto success_peg_192;
                goto out_peg_145;
                success_peg_192:
                ;
            
            
            
            result_peg_141 = rule_integer(stream, result_peg_141.getPosition());
                if (result_peg_141.error()){
                    goto out_peg_145;
                }
                to = result_peg_141.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionEquals(Value(makeExpressionModulo(Value(makeIdentifier(getCurrentLine(line), getCurrentColumn(line), "time")), mod)), to);
                    result_peg_141.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_141;
        stream.update(result_peg_141.getPosition());
        
        
        return result_peg_141;
        out_peg_145:
        Result result_peg_202(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_204 = stream.getLineInfo(result_peg_202.getPosition());
                line = &line_info_peg_204;
            
            
            
            {
                    int position_peg_207 = result_peg_202.getPosition();
                    
                    result_peg_202.setValue(Value((void*) "projhit"));
                    for (int i = 0; i < 7; i++){
                        if (compareCharCase("projhit"[i], stream.get(result_peg_202.getPosition()))){
                            result_peg_202.nextPosition();
                        } else {
                            result_peg_202.setPosition(position_peg_207);
                            goto out_peg_208;
                        }
                    }
                        
                }
                goto success_peg_206;
                out_peg_208:
                {
                    int position_peg_209 = result_peg_202.getPosition();
                    
                    result_peg_202.setValue(Value((void*) "projguarded"));
                    for (int i = 0; i < 11; i++){
                        if (compareCharCase("projguarded"[i], stream.get(result_peg_202.getPosition()))){
                            result_peg_202.nextPosition();
                        } else {
                            result_peg_202.setPosition(position_peg_209);
                            goto out_peg_210;
                        }
                    }
                        
                }
                goto success_peg_206;
                out_peg_210:
                {
                    int position_peg_211 = result_peg_202.getPosition();
                    
                    result_peg_202.setValue(Value((void*) "projcontact"));
                    for (int i = 0; i < 11; i++){
                        if (compareCharCase("projcontact"[i], stream.get(result_peg_202.getPosition()))){
                            result_peg_202.nextPosition();
                        } else {
                            result_peg_202.setPosition(position_peg_211);
                            goto out_peg_212;
                        }
                    }
                        
                }
                goto success_peg_206;
                out_peg_212:
                goto out_peg_213;
                success_peg_206:
                ;
                name = result_peg_202.getValues();
            
            
            
            int save_peg_215 = result_peg_202.getPosition();
                
                result_peg_202 = rule_paren_integer(stream, result_peg_202.getPosition());
                if (result_peg_202.error()){
                    
                    result_peg_202 = Result(save_peg_215);
                    result_peg_202.setValue(Value((void*) 0));
                    
                }
                id = result_peg_202.getValues();
            
            
            
            {
                    int position_peg_218 = result_peg_202.getPosition();
                    
                    result_peg_202.reset();
                    do{
                        Result result_peg_220(result_peg_202.getPosition());
                        {
                            int position_peg_222 = result_peg_220.getPosition();
                            
                            result_peg_220.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    result_peg_220.setPosition(position_peg_222);
                                    goto out_peg_223;
                                }
                            }
                                
                        }
                        goto success_peg_221;
                        out_peg_223:
                        {
                            int position_peg_224 = result_peg_220.getPosition();
                            
                            result_peg_220.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    result_peg_220.setPosition(position_peg_224);
                                    goto out_peg_225;
                                }
                            }
                                
                        }
                        goto success_peg_221;
                        out_peg_225:
                        goto loop_peg_219;
                        success_peg_221:
                        ;
                        result_peg_202.addResult(result_peg_220);
                    } while (true);
                    loop_peg_219:
                    ;
                            
                }
                goto success_peg_217;
                goto out_peg_213;
                success_peg_217:
                ;
            
            
            
            result_peg_202.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_202.getPosition()))){
                        result_peg_202.nextPosition();
                    } else {
                        goto out_peg_213;
                    }
                }
            
            
            
            {
                    int position_peg_229 = result_peg_202.getPosition();
                    
                    result_peg_202.reset();
                    do{
                        Result result_peg_231(result_peg_202.getPosition());
                        {
                            int position_peg_233 = result_peg_231.getPosition();
                            
                            result_peg_231.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_231.getPosition()))){
                                    result_peg_231.nextPosition();
                                } else {
                                    result_peg_231.setPosition(position_peg_233);
                                    goto out_peg_234;
                                }
                            }
                                
                        }
                        goto success_peg_232;
                        out_peg_234:
                        {
                            int position_peg_235 = result_peg_231.getPosition();
                            
                            result_peg_231.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_231.getPosition()))){
                                    result_peg_231.nextPosition();
                                } else {
                                    result_peg_231.setPosition(position_peg_235);
                                    goto out_peg_236;
                                }
                            }
                                
                        }
                        goto success_peg_232;
                        out_peg_236:
                        goto loop_peg_230;
                        success_peg_232:
                        ;
                        result_peg_202.addResult(result_peg_231);
                    } while (true);
                    loop_peg_230:
                    ;
                            
                }
                goto success_peg_228;
                goto out_peg_213;
                success_peg_228:
                ;
            
            
            
            result_peg_202 = rule_integer(stream, result_peg_202.getPosition());
                if (result_peg_202.error()){
                    goto out_peg_213;
                }
                arg1 = result_peg_202.getValues();
            
            
            
            int save_peg_239 = result_peg_202.getPosition();
                {
                
                    {
                            int position_peg_242 = result_peg_202.getPosition();
                            
                            result_peg_202.reset();
                            do{
                                Result result_peg_244(result_peg_202.getPosition());
                                {
                                    int position_peg_246 = result_peg_244.getPosition();
                                    
                                    result_peg_244.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_244.getPosition()))){
                                            result_peg_244.nextPosition();
                                        } else {
                                            result_peg_244.setPosition(position_peg_246);
                                            goto out_peg_247;
                                        }
                                    }
                                        
                                }
                                goto success_peg_245;
                                out_peg_247:
                                {
                                    int position_peg_248 = result_peg_244.getPosition();
                                    
                                    result_peg_244.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_244.getPosition()))){
                                            result_peg_244.nextPosition();
                                        } else {
                                            result_peg_244.setPosition(position_peg_248);
                                            goto out_peg_249;
                                        }
                                    }
                                        
                                }
                                goto success_peg_245;
                                out_peg_249:
                                goto loop_peg_243;
                                success_peg_245:
                                ;
                                result_peg_202.addResult(result_peg_244);
                            } while (true);
                            loop_peg_243:
                            ;
                                    
                        }
                        goto success_peg_241;
                        
                        result_peg_202 = Result(save_peg_239);
                        result_peg_202.setValue(Value((void*) 0));
                        
                        success_peg_241:
                        ;
                    
                    
                    
                    result_peg_202.setValue(Value((void*) ","));
                        for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_202.getPosition()))){
                                result_peg_202.nextPosition();
                            } else {
                                
                                result_peg_202 = Result(save_peg_239);
                                result_peg_202.setValue(Value((void*) 0));
                                
                            }
                        }
                    
                    
                    
                    {
                            int position_peg_253 = result_peg_202.getPosition();
                            
                            result_peg_202.reset();
                            do{
                                Result result_peg_255(result_peg_202.getPosition());
                                {
                                    int position_peg_257 = result_peg_255.getPosition();
                                    
                                    result_peg_255.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_255.getPosition()))){
                                            result_peg_255.nextPosition();
                                        } else {
                                            result_peg_255.setPosition(position_peg_257);
                                            goto out_peg_258;
                                        }
                                    }
                                        
                                }
                                goto success_peg_256;
                                out_peg_258:
                                {
                                    int position_peg_259 = result_peg_255.getPosition();
                                    
                                    result_peg_255.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_255.getPosition()))){
                                            result_peg_255.nextPosition();
                                        } else {
                                            result_peg_255.setPosition(position_peg_259);
                                            goto out_peg_260;
                                        }
                                    }
                                        
                                }
                                goto success_peg_256;
                                out_peg_260:
                                goto loop_peg_254;
                                success_peg_256:
                                ;
                                result_peg_202.addResult(result_peg_255);
                            } while (true);
                            loop_peg_254:
                            ;
                                    
                        }
                        goto success_peg_252;
                        
                        result_peg_202 = Result(save_peg_239);
                        result_peg_202.setValue(Value((void*) 0));
                        
                        success_peg_252:
                        ;
                    
                    
                    
                    result_peg_202 = rule_all_compare(stream, result_peg_202.getPosition());
                        if (result_peg_202.error()){
                            
                            result_peg_202 = Result(save_peg_239);
                            result_peg_202.setValue(Value((void*) 0));
                            
                        }
                        compare = result_peg_202.getValues();
                    
                    
                    
                    {
                            int position_peg_264 = result_peg_202.getPosition();
                            
                            result_peg_202.reset();
                            do{
                                Result result_peg_266(result_peg_202.getPosition());
                                {
                                    int position_peg_268 = result_peg_266.getPosition();
                                    
                                    result_peg_266.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_266.getPosition()))){
                                            result_peg_266.nextPosition();
                                        } else {
                                            result_peg_266.setPosition(position_peg_268);
                                            goto out_peg_269;
                                        }
                                    }
                                        
                                }
                                goto success_peg_267;
                                out_peg_269:
                                {
                                    int position_peg_270 = result_peg_266.getPosition();
                                    
                                    result_peg_266.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_266.getPosition()))){
                                            result_peg_266.nextPosition();
                                        } else {
                                            result_peg_266.setPosition(position_peg_270);
                                            goto out_peg_271;
                                        }
                                    }
                                        
                                }
                                goto success_peg_267;
                                out_peg_271:
                                goto loop_peg_265;
                                success_peg_267:
                                ;
                                result_peg_202.addResult(result_peg_266);
                            } while (true);
                            loop_peg_265:
                            ;
                                    
                        }
                        goto success_peg_263;
                        
                        result_peg_202 = Result(save_peg_239);
                        result_peg_202.setValue(Value((void*) 0));
                        
                        success_peg_263:
                        ;
                    
                    
                    
                    result_peg_202 = rule_value(stream, result_peg_202.getPosition());
                        if (result_peg_202.error()){
                            
                            result_peg_202 = Result(save_peg_239);
                            result_peg_202.setValue(Value((void*) 0));
                            
                        }
                        ticks = result_peg_202.getValues();
                    
                    
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = Value(makeProjectileExpression(line, name, id, arg1, compare, ticks));
                    result_peg_202.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_202;
        stream.update(result_peg_202.getPosition());
        
        
        return result_peg_202;
        out_peg_213:
        Result result_peg_272(myposition);
        
        
        {
        
            result_peg_272.setValue(Value((void*) "hitdefattr"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("hitdefattr"[i], stream.get(result_peg_272.getPosition()))){
                        result_peg_272.nextPosition();
                    } else {
                        goto out_peg_274;
                    }
                }
            
            
            
            {
                    int position_peg_277 = result_peg_272.getPosition();
                    
                    result_peg_272.reset();
                    do{
                        Result result_peg_279(result_peg_272.getPosition());
                        {
                            int position_peg_281 = result_peg_279.getPosition();
                            
                            result_peg_279.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_279.getPosition()))){
                                    result_peg_279.nextPosition();
                                } else {
                                    result_peg_279.setPosition(position_peg_281);
                                    goto out_peg_282;
                                }
                            }
                                
                        }
                        goto success_peg_280;
                        out_peg_282:
                        {
                            int position_peg_283 = result_peg_279.getPosition();
                            
                            result_peg_279.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_279.getPosition()))){
                                    result_peg_279.nextPosition();
                                } else {
                                    result_peg_279.setPosition(position_peg_283);
                                    goto out_peg_284;
                                }
                            }
                                
                        }
                        goto success_peg_280;
                        out_peg_284:
                        goto loop_peg_278;
                        success_peg_280:
                        ;
                        result_peg_272.addResult(result_peg_279);
                    } while (true);
                    loop_peg_278:
                    ;
                            
                }
                goto success_peg_276;
                goto out_peg_274;
                success_peg_276:
                ;
            
            
            
            {
                    int position_peg_287 = result_peg_272.getPosition();
                    {
                    
                        result_peg_272.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_272.getPosition()))){
                                    result_peg_272.nextPosition();
                                } else {
                                    result_peg_272.setPosition(position_peg_287);
                                    goto out_peg_289;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = (void*) makeExpressionEquals;
                                result_peg_272.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_286;
                out_peg_289:
                {
                    int position_peg_290 = result_peg_272.getPosition();
                    {
                    
                        result_peg_272.setValue(Value((void*) "!="));
                            for (int i = 0; i < 2; i++){
                                if (compareChar("!="[i], stream.get(result_peg_272.getPosition()))){
                                    result_peg_272.nextPosition();
                                } else {
                                    result_peg_272.setPosition(position_peg_290);
                                    goto out_peg_292;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = (void*) makeExpressionUnequals;
                                result_peg_272.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_286;
                out_peg_292:
                goto out_peg_274;
                success_peg_286:
                ;
                comparison = result_peg_272.getValues();
            
            
            
            {
                    int position_peg_295 = result_peg_272.getPosition();
                    
                    result_peg_272.reset();
                    do{
                        Result result_peg_297(result_peg_272.getPosition());
                        {
                            int position_peg_299 = result_peg_297.getPosition();
                            
                            result_peg_297.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_297.getPosition()))){
                                    result_peg_297.nextPosition();
                                } else {
                                    result_peg_297.setPosition(position_peg_299);
                                    goto out_peg_300;
                                }
                            }
                                
                        }
                        goto success_peg_298;
                        out_peg_300:
                        {
                            int position_peg_301 = result_peg_297.getPosition();
                            
                            result_peg_297.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_297.getPosition()))){
                                    result_peg_297.nextPosition();
                                } else {
                                    result_peg_297.setPosition(position_peg_301);
                                    goto out_peg_302;
                                }
                            }
                                
                        }
                        goto success_peg_298;
                        out_peg_302:
                        goto loop_peg_296;
                        success_peg_298:
                        ;
                        result_peg_272.addResult(result_peg_297);
                    } while (true);
                    loop_peg_296:
                    ;
                            
                }
                goto success_peg_294;
                goto out_peg_274;
                success_peg_294:
                ;
            
            
            
            result_peg_272 = rule_hitdef__attribute(stream, result_peg_272.getPosition());
                if (result_peg_272.error()){
                    goto out_peg_274;
                }
                state = result_peg_272.getValues();
            
            
            
            {
                    int position_peg_306 = result_peg_272.getPosition();
                    
                    result_peg_272.reset();
                    do{
                        Result result_peg_308(result_peg_272.getPosition());
                        {
                            int position_peg_310 = result_peg_308.getPosition();
                            
                            result_peg_308.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_308.getPosition()))){
                                    result_peg_308.nextPosition();
                                } else {
                                    result_peg_308.setPosition(position_peg_310);
                                    goto out_peg_311;
                                }
                            }
                                
                        }
                        goto success_peg_309;
                        out_peg_311:
                        {
                            int position_peg_312 = result_peg_308.getPosition();
                            
                            result_peg_308.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_308.getPosition()))){
                                    result_peg_308.nextPosition();
                                } else {
                                    result_peg_308.setPosition(position_peg_312);
                                    goto out_peg_313;
                                }
                            }
                                
                        }
                        goto success_peg_309;
                        out_peg_313:
                        goto loop_peg_307;
                        success_peg_309:
                        ;
                        result_peg_272.addResult(result_peg_308);
                    } while (true);
                    loop_peg_307:
                    ;
                            
                }
                goto success_peg_305;
                goto out_peg_274;
                success_peg_305:
                ;
            
            
            
            result_peg_272.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_272.getPosition()))){
                        result_peg_272.nextPosition();
                    } else {
                        goto out_peg_274;
                    }
                }
            
            
            
            {
                    int position_peg_317 = result_peg_272.getPosition();
                    
                    result_peg_272.reset();
                    do{
                        Result result_peg_319(result_peg_272.getPosition());
                        {
                            int position_peg_321 = result_peg_319.getPosition();
                            
                            result_peg_319.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_319.getPosition()))){
                                    result_peg_319.nextPosition();
                                } else {
                                    result_peg_319.setPosition(position_peg_321);
                                    goto out_peg_322;
                                }
                            }
                                
                        }
                        goto success_peg_320;
                        out_peg_322:
                        {
                            int position_peg_323 = result_peg_319.getPosition();
                            
                            result_peg_319.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_319.getPosition()))){
                                    result_peg_319.nextPosition();
                                } else {
                                    result_peg_319.setPosition(position_peg_323);
                                    goto out_peg_324;
                                }
                            }
                                
                        }
                        goto success_peg_320;
                        out_peg_324:
                        goto loop_peg_318;
                        success_peg_320:
                        ;
                        result_peg_272.addResult(result_peg_319);
                    } while (true);
                    loop_peg_318:
                    ;
                            
                }
                goto success_peg_316;
                goto out_peg_274;
                success_peg_316:
                ;
            
            
            
            result_peg_272 = rule_hitdef__attack__attribute(stream, result_peg_272.getPosition());
                if (result_peg_272.error()){
                    goto out_peg_274;
                }
                attributes = result_peg_272.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                                     Ast::Value * compare_state = as<compare_func>(comparison)(Value(makeSimpleIdentifier("hitdefattr:state")), state);
                                     Ast::Value * compare_attribute = as<compare_func>(comparison)(Value(makeSimpleIdentifier("hitdefattr:attribute")), attributes);
                                     value = makeExpressionAnd(Value(compare_state), Value(compare_attribute));
                    result_peg_272.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_272;
        stream.update(result_peg_272.getPosition());
        
        
        return result_peg_272;
        out_peg_274:
        Result result_peg_326(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_328 = stream.getLineInfo(result_peg_326.getPosition());
                line = &line_info_peg_328;
            
            
            
            result_peg_326.setValue(Value((void*) "hitdefattr"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("hitdefattr"[i], stream.get(result_peg_326.getPosition()))){
                        result_peg_326.nextPosition();
                    } else {
                        goto out_peg_330;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    std::ostringstream error;
                                       error << "Invalid hitdefattr on line " << getCurrentLine(line) << " column " << getCurrentColumn(line);
                                       throw ParseException(error.str());
                    result_peg_326.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_326;
        stream.update(result_peg_326.getPosition());
        
        
        return result_peg_326;
        out_peg_330:
        Result result_peg_331(myposition);
        
        
        {
        
            {
                    int position_peg_334 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "abs"));
                    for (int i = 0; i < 3; i++){
                        if (compareCharCase("abs"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_334);
                            goto out_peg_335;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_335:
                {
                    int position_peg_336 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "const"));
                    for (int i = 0; i < 5; i++){
                        if (compareCharCase("const"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_336);
                            goto out_peg_337;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_337:
                {
                    int position_peg_338 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "selfanimexist"));
                    for (int i = 0; i < 13; i++){
                        if (compareCharCase("selfanimexist"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_338);
                            goto out_peg_339;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_339:
                {
                    int position_peg_340 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "ifelse"));
                    for (int i = 0; i < 6; i++){
                        if (compareCharCase("ifelse"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_340);
                            goto out_peg_341;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_341:
                {
                    int position_peg_342 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "gethitvar"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("gethitvar"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_342);
                            goto out_peg_343;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_343:
                {
                    int position_peg_344 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "floor"));
                    for (int i = 0; i < 5; i++){
                        if (compareCharCase("floor"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_344);
                            goto out_peg_345;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_345:
                {
                    int position_peg_346 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "ceil"));
                    for (int i = 0; i < 4; i++){
                        if (compareCharCase("ceil"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_346);
                            goto out_peg_347;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_347:
                {
                    int position_peg_348 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "exp"));
                    for (int i = 0; i < 3; i++){
                        if (compareCharCase("exp"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_348);
                            goto out_peg_349;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_349:
                {
                    int position_peg_350 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "acos"));
                    for (int i = 0; i < 4; i++){
                        if (compareCharCase("acos"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_350);
                            goto out_peg_351;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_351:
                {
                    int position_peg_352 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "asin"));
                    for (int i = 0; i < 4; i++){
                        if (compareCharCase("asin"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_352);
                            goto out_peg_353;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_353:
                {
                    int position_peg_354 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "atan"));
                    for (int i = 0; i < 4; i++){
                        if (compareCharCase("atan"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_354);
                            goto out_peg_355;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_355:
                {
                    int position_peg_356 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "tan"));
                    for (int i = 0; i < 3; i++){
                        if (compareCharCase("tan"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_356);
                            goto out_peg_357;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_357:
                {
                    int position_peg_358 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "cos"));
                    for (int i = 0; i < 3; i++){
                        if (compareCharCase("cos"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_358);
                            goto out_peg_359;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_359:
                {
                    int position_peg_360 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "sin"));
                    for (int i = 0; i < 3; i++){
                        if (compareCharCase("sin"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_360);
                            goto out_peg_361;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_361:
                {
                    int position_peg_362 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "log"));
                    for (int i = 0; i < 3; i++){
                        if (compareCharCase("log"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_362);
                            goto out_peg_363;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_363:
                {
                    int position_peg_364 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "ln"));
                    for (int i = 0; i < 2; i++){
                        if (compareCharCase("ln"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_364);
                            goto out_peg_365;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_365:
                {
                    int position_peg_366 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "sysfvar"));
                    for (int i = 0; i < 7; i++){
                        if (compareCharCase("sysfvar"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_366);
                            goto out_peg_367;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_367:
                {
                    int position_peg_368 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "sysvar"));
                    for (int i = 0; i < 6; i++){
                        if (compareCharCase("sysvar"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_368);
                            goto out_peg_369;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_369:
                {
                    int position_peg_370 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "var"));
                    for (int i = 0; i < 3; i++){
                        if (compareCharCase("var"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_370);
                            goto out_peg_371;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_371:
                {
                    int position_peg_372 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "numexplod"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("numexplod"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_372);
                            goto out_peg_373;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_373:
                {
                    int position_peg_374 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "numhelper"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("numhelper"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_374);
                            goto out_peg_375;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_375:
                {
                    int position_peg_376 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "numprojid"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("numprojid"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_376);
                            goto out_peg_377;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_377:
                {
                    int position_peg_378 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "fvar"));
                    for (int i = 0; i < 4; i++){
                        if (compareCharCase("fvar"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_378);
                            goto out_peg_379;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_379:
                {
                    int position_peg_380 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "ishelper"));
                    for (int i = 0; i < 8; i++){
                        if (compareCharCase("ishelper"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_380);
                            goto out_peg_381;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_381:
                {
                    int position_peg_382 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "numtarget"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("numtarget"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_382);
                            goto out_peg_383;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_383:
                {
                    int position_peg_384 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "animelemtime"));
                    for (int i = 0; i < 12; i++){
                        if (compareCharCase("animelemtime"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_384);
                            goto out_peg_385;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_385:
                {
                    int position_peg_386 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "animelemno"));
                    for (int i = 0; i < 10; i++){
                        if (compareCharCase("animelemno"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_386);
                            goto out_peg_387;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_387:
                {
                    int position_peg_388 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "animexist"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("animexist"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_388);
                            goto out_peg_389;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_389:
                {
                    int position_peg_390 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "playeridexist"));
                    for (int i = 0; i < 13; i++){
                        if (compareCharCase("playeridexist"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_390);
                            goto out_peg_391;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_391:
                {
                    int position_peg_392 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "projguarded"));
                    for (int i = 0; i < 11; i++){
                        if (compareCharCase("projguarded"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_392);
                            goto out_peg_393;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_393:
                {
                    int position_peg_394 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "projcanceltime"));
                    for (int i = 0; i < 14; i++){
                        if (compareCharCase("projcanceltime"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_394);
                            goto out_peg_395;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_395:
                {
                    int position_peg_396 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "projhittime"));
                    for (int i = 0; i < 11; i++){
                        if (compareCharCase("projhittime"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_396);
                            goto out_peg_397;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_397:
                {
                    int position_peg_398 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "projhit"));
                    for (int i = 0; i < 7; i++){
                        if (compareCharCase("projhit"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_398);
                            goto out_peg_399;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_399:
                {
                    int position_peg_400 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "projcontacttime"));
                    for (int i = 0; i < 15; i++){
                        if (compareCharCase("projcontacttime"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_400);
                            goto out_peg_401;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_401:
                {
                    int position_peg_402 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "projcontact"));
                    for (int i = 0; i < 11; i++){
                        if (compareCharCase("projcontact"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_402);
                            goto out_peg_403;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_403:
                {
                    int position_peg_404 = result_peg_331.getPosition();
                    
                    result_peg_331.setValue(Value((void*) "numhelper"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("numhelper"[i], stream.get(result_peg_331.getPosition()))){
                            result_peg_331.nextPosition();
                        } else {
                            result_peg_331.setPosition(position_peg_404);
                            goto out_peg_405;
                        }
                    }
                        
                }
                goto success_peg_333;
                out_peg_405:
                goto out_peg_406;
                success_peg_333:
                ;
                name = result_peg_331.getValues();
            
            
            
            {
                    int position_peg_409 = result_peg_331.getPosition();
                    
                    result_peg_331.reset();
                    do{
                        Result result_peg_411(result_peg_331.getPosition());
                        {
                            int position_peg_413 = result_peg_411.getPosition();
                            
                            result_peg_411.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_411.getPosition()))){
                                    result_peg_411.nextPosition();
                                } else {
                                    result_peg_411.setPosition(position_peg_413);
                                    goto out_peg_414;
                                }
                            }
                                
                        }
                        goto success_peg_412;
                        out_peg_414:
                        {
                            int position_peg_415 = result_peg_411.getPosition();
                            
                            result_peg_411.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_411.getPosition()))){
                                    result_peg_411.nextPosition();
                                } else {
                                    result_peg_411.setPosition(position_peg_415);
                                    goto out_peg_416;
                                }
                            }
                                
                        }
                        goto success_peg_412;
                        out_peg_416:
                        goto loop_peg_410;
                        success_peg_412:
                        ;
                        result_peg_331.addResult(result_peg_411);
                    } while (true);
                    loop_peg_410:
                    ;
                            
                }
                goto success_peg_408;
                goto out_peg_406;
                success_peg_408:
                ;
            
            
            
            result_peg_331.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_331.getPosition()))){
                        result_peg_331.nextPosition();
                    } else {
                        goto out_peg_406;
                    }
                }
            
            
            
            {
                    int position_peg_420 = result_peg_331.getPosition();
                    
                    result_peg_331.reset();
                    do{
                        Result result_peg_422(result_peg_331.getPosition());
                        {
                            int position_peg_424 = result_peg_422.getPosition();
                            
                            result_peg_422.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_422.getPosition()))){
                                    result_peg_422.nextPosition();
                                } else {
                                    result_peg_422.setPosition(position_peg_424);
                                    goto out_peg_425;
                                }
                            }
                                
                        }
                        goto success_peg_423;
                        out_peg_425:
                        {
                            int position_peg_426 = result_peg_422.getPosition();
                            
                            result_peg_422.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_422.getPosition()))){
                                    result_peg_422.nextPosition();
                                } else {
                                    result_peg_422.setPosition(position_peg_426);
                                    goto out_peg_427;
                                }
                            }
                                
                        }
                        goto success_peg_423;
                        out_peg_427:
                        goto loop_peg_421;
                        success_peg_423:
                        ;
                        result_peg_331.addResult(result_peg_422);
                    } while (true);
                    loop_peg_421:
                    ;
                            
                }
                goto success_peg_419;
                goto out_peg_406;
                success_peg_419:
                ;
            
            
            
            result_peg_331 = rule_args(stream, result_peg_331.getPosition());
                if (result_peg_331.error()){
                    goto out_peg_406;
                }
                args = result_peg_331.getValues();
            
            
            
            {
                    int position_peg_431 = result_peg_331.getPosition();
                    
                    result_peg_331.reset();
                    do{
                        Result result_peg_433(result_peg_331.getPosition());
                        {
                            int position_peg_435 = result_peg_433.getPosition();
                            
                            result_peg_433.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_433.getPosition()))){
                                    result_peg_433.nextPosition();
                                } else {
                                    result_peg_433.setPosition(position_peg_435);
                                    goto out_peg_436;
                                }
                            }
                                
                        }
                        goto success_peg_434;
                        out_peg_436:
                        {
                            int position_peg_437 = result_peg_433.getPosition();
                            
                            result_peg_433.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_433.getPosition()))){
                                    result_peg_433.nextPosition();
                                } else {
                                    result_peg_433.setPosition(position_peg_437);
                                    goto out_peg_438;
                                }
                            }
                                
                        }
                        goto success_peg_434;
                        out_peg_438:
                        goto loop_peg_432;
                        success_peg_434:
                        ;
                        result_peg_331.addResult(result_peg_433);
                    } while (true);
                    loop_peg_432:
                    ;
                            
                }
                goto success_peg_430;
                goto out_peg_406;
                success_peg_430:
                ;
            
            
            
            result_peg_331.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_331.getPosition()))){
                        result_peg_331.nextPosition();
                    } else {
                        goto out_peg_406;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction(name, args);
                    result_peg_331.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = result_peg_331;
        stream.update(result_peg_331.getPosition());
        
        
        return result_peg_331;
        out_peg_406:
    
        if (column_peg_2.chunk8 == 0){
            column_peg_2.chunk8 = new Chunk8();
        }
        column_peg_2.chunk8->chunk_function = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_paren_integer(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk9 != 0 && column_peg_2.chunk9->chunk_paren_integer.calculated()){
        return column_peg_2.chunk9->chunk_paren_integer;
    }
    
    
    RuleTrace trace_peg_1(stream, "paren_integer");
    int myposition = position;
    
    
    Value id;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                id = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_5;
                success_peg_18:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = id;
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_paren_integer = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_28(myposition);
        
        
        result_peg_28 = rule_integer(stream, result_peg_28.getPosition());
        if (result_peg_28.error()){
            goto out_peg_29;
        }
        
        
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_paren_integer = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_29:
    
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_paren_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk9 != 0 && column_peg_2.chunk9->chunk_hitdef__attack__attribute.calculated()){
        return column_peg_2.chunk9->chunk_hitdef__attack__attribute;
    }
    
    
    RuleTrace trace_peg_1(stream, "hitdef__attack__attribute");
    int myposition = position;
    
    
    Value attribute;
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    Value value((void*) 0);
                    value = makeHitDefAttackAttribute();
                    result_peg_3.setValue(value);
                }
                attribute = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_hitdef__attack__attribute__item(stream, result_peg_3.getPosition(), attribute);
                if (result_peg_3.error()){
                    goto out_peg_6;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_9(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_12 = result_peg_9.getPosition();
                                
                                result_peg_9.reset();
                                do{
                                    Result result_peg_14(result_peg_9.getPosition());
                                    {
                                        int position_peg_16 = result_peg_14.getPosition();
                                        
                                        result_peg_14.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_14.getPosition()))){
                                                result_peg_14.nextPosition();
                                            } else {
                                                result_peg_14.setPosition(position_peg_16);
                                                goto out_peg_17;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_15;
                                    out_peg_17:
                                    {
                                        int position_peg_18 = result_peg_14.getPosition();
                                        
                                        result_peg_14.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_14.getPosition()))){
                                                result_peg_14.nextPosition();
                                            } else {
                                                result_peg_14.setPosition(position_peg_18);
                                                goto out_peg_19;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_15;
                                    out_peg_19:
                                    goto loop_peg_13;
                                    success_peg_15:
                                    ;
                                    result_peg_9.addResult(result_peg_14);
                                } while (true);
                                loop_peg_13:
                                ;
                                        
                            }
                            goto success_peg_11;
                            goto loop_peg_8;
                            success_peg_11:
                            ;
                        
                        
                        
                        result_peg_9.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    goto loop_peg_8;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_23 = result_peg_9.getPosition();
                                
                                result_peg_9.reset();
                                do{
                                    Result result_peg_25(result_peg_9.getPosition());
                                    {
                                        int position_peg_27 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_25.getPosition()))){
                                                result_peg_25.nextPosition();
                                            } else {
                                                result_peg_25.setPosition(position_peg_27);
                                                goto out_peg_28;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_26;
                                    out_peg_28:
                                    {
                                        int position_peg_29 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_25.getPosition()))){
                                                result_peg_25.nextPosition();
                                            } else {
                                                result_peg_25.setPosition(position_peg_29);
                                                goto out_peg_30;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_26;
                                    out_peg_30:
                                    goto loop_peg_24;
                                    success_peg_26:
                                    ;
                                    result_peg_9.addResult(result_peg_25);
                                } while (true);
                                loop_peg_24:
                                ;
                                        
                            }
                            goto success_peg_22;
                            goto loop_peg_8;
                            success_peg_22:
                            ;
                        
                        
                        
                        result_peg_9 = rule_hitdef__attack__attribute__item(stream, result_peg_9.getPosition(), attribute);
                            if (result_peg_9.error()){
                                goto loop_peg_8;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = attribute;
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_hitdef__attack__attribute = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_6:
    
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_hitdef__attack__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute__item(Stream & stream, const int position, Value attribute){
    
    
    RuleTrace trace_peg_1(stream, "hitdef__attack__attribute__item");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "n"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("n"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_6);
                            goto out_peg_7;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_7:
                {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "s"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("s"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_8);
                            goto out_peg_9;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_9:
                {
                    int position_peg_10 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "h"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("h"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_10);
                            goto out_peg_11;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_11:
                {
                    int position_peg_12 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "a"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("a"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_12);
                            goto out_peg_13;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_13:
                goto out_peg_14;
                success_peg_5:
                ;
            
            Result result_peg_4 = result_peg_3;
            
            {
                    int position_peg_17 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "a"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("a"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_17);
                            goto out_peg_18;
                        }
                    }
                        
                }
                goto success_peg_16;
                out_peg_18:
                {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "t"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("t"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_19);
                            goto out_peg_20;
                        }
                    }
                        
                }
                goto success_peg_16;
                out_peg_20:
                {
                    int position_peg_21 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "p"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("p"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_21);
                            goto out_peg_22;
                        }
                    }
                        
                }
                goto success_peg_16;
                out_peg_22:
                goto out_peg_14;
                success_peg_16:
                ;
            
            Result result_peg_15 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    std::string * result = toString(as<const char *>(result_peg_4.getValues()));
                            std::string * other = toString(as<const char *>(result_peg_15.getValues()));
                            as<Ast::HitDefAttackAttribute*>(attribute)->addAttribute(*result + *other);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_14:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitdef__attribute(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk9 != 0 && column_peg_2.chunk9->chunk_hitdef__attribute.calculated()){
        return column_peg_2.chunk9->chunk_hitdef__attribute;
    }
    
    
    RuleTrace trace_peg_1(stream, "hitdef__attribute");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.reset();
                do{
                    Result result_peg_6(result_peg_3.getPosition());
                    {
                        int position_peg_8 = result_peg_6.getPosition();
                        
                        result_peg_6.setValue(Value((void*) "s"));
                        for (int i = 0; i < 1; i++){
                            if (compareCharCase("s"[i], stream.get(result_peg_6.getPosition()))){
                                result_peg_6.nextPosition();
                            } else {
                                result_peg_6.setPosition(position_peg_8);
                                goto out_peg_9;
                            }
                        }
                            
                    }
                    goto success_peg_7;
                    out_peg_9:
                    {
                        int position_peg_10 = result_peg_6.getPosition();
                        
                        result_peg_6.setValue(Value((void*) "c"));
                        for (int i = 0; i < 1; i++){
                            if (compareCharCase("c"[i], stream.get(result_peg_6.getPosition()))){
                                result_peg_6.nextPosition();
                            } else {
                                result_peg_6.setPosition(position_peg_10);
                                goto out_peg_11;
                            }
                        }
                            
                    }
                    goto success_peg_7;
                    out_peg_11:
                    {
                        int position_peg_12 = result_peg_6.getPosition();
                        
                        result_peg_6.setValue(Value((void*) "a"));
                        for (int i = 0; i < 1; i++){
                            if (compareCharCase("a"[i], stream.get(result_peg_6.getPosition()))){
                                result_peg_6.nextPosition();
                            } else {
                                result_peg_6.setPosition(position_peg_12);
                                goto out_peg_13;
                            }
                        }
                            
                    }
                    goto success_peg_7;
                    out_peg_13:
                    goto loop_peg_5;
                    success_peg_7:
                    ;
                    result_peg_3.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                if (result_peg_3.matches() == 0){
                    goto out_peg_14;
                }
            
            Result result_peg_4 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = makeHitDefAttribute(result_peg_4.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_hitdef__attribute = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_14:
    
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_hitdef__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk9 != 0 && column_peg_2.chunk9->chunk_args.calculated()){
        return column_peg_2.chunk9->chunk_args;
    }
    
    
    RuleTrace trace_peg_1(stream, "args");
    int myposition = position;
    
    
    Value expr1;
        Value expr_rest;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_expr_c(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                expr1 = result_peg_3.getValues();
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_8(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_11 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_13(result_peg_8.getPosition());
                                    {
                                        int position_peg_15 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_15);
                                                goto out_peg_16;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_16:
                                    {
                                        int position_peg_17 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_17);
                                                goto out_peg_18;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_18:
                                    goto loop_peg_12;
                                    success_peg_14:
                                    ;
                                    result_peg_8.addResult(result_peg_13);
                                } while (true);
                                loop_peg_12:
                                ;
                                        
                            }
                            goto success_peg_10;
                            goto loop_peg_7;
                            success_peg_10:
                            ;
                        
                        
                        
                        result_peg_8.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    goto loop_peg_7;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_22 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_24(result_peg_8.getPosition());
                                    {
                                        int position_peg_26 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_26);
                                                goto out_peg_27;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_27:
                                    {
                                        int position_peg_28 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_28);
                                                goto out_peg_29;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_29:
                                    goto loop_peg_23;
                                    success_peg_25:
                                    ;
                                    result_peg_8.addResult(result_peg_24);
                                } while (true);
                                loop_peg_23:
                                ;
                                        
                            }
                            goto success_peg_21;
                            goto loop_peg_7;
                            success_peg_21:
                            ;
                        
                        
                        
                        result_peg_8 = rule_expr_c(stream, result_peg_8.getPosition());
                            if (result_peg_8.error()){
                                goto loop_peg_7;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
                expr_rest = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(expr1, expr_rest);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_args = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk9 == 0){
            column_peg_2.chunk9 = new Chunk9();
        }
        column_peg_2.chunk9->chunk_args = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk10 != 0 && column_peg_2.chunk10->chunk_function_rest.calculated()){
        return column_peg_2.chunk10->chunk_function_rest;
    }
    
    
    RuleTrace trace_peg_1(stream, "function_rest");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_8(result_peg_3.getPosition());
                        {
                            int position_peg_10 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_10);
                                    goto out_peg_11;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_11:
                        {
                            int position_peg_12 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_13:
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_3.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_14;
                success_peg_5:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_14;
                    }
                }
            
            
            
            {
                    int position_peg_18 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_20(result_peg_3.getPosition());
                        {
                            int position_peg_22 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_22);
                                    goto out_peg_23;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_23:
                        {
                            int position_peg_24 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_24);
                                    goto out_peg_25;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_25:
                        goto loop_peg_19;
                        success_peg_21:
                        ;
                        result_peg_3.addResult(result_peg_20);
                    } while (true);
                    loop_peg_19:
                    ;
                            
                }
                goto success_peg_17;
                goto out_peg_14;
                success_peg_17:
                ;
            
            
            
            int save_peg_27 = result_peg_3.getPosition();
                
                result_peg_3 = rule_all_compare(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    
                    result_peg_3 = Result(save_peg_27);
                    result_peg_3.setValue(Value((void*) 0));
                    
                }
            
            
            
            {
                    int position_peg_30 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_32(result_peg_3.getPosition());
                        {
                            int position_peg_34 = result_peg_32.getPosition();
                            
                            result_peg_32.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_32.getPosition()))){
                                    result_peg_32.nextPosition();
                                } else {
                                    result_peg_32.setPosition(position_peg_34);
                                    goto out_peg_35;
                                }
                            }
                                
                        }
                        goto success_peg_33;
                        out_peg_35:
                        {
                            int position_peg_36 = result_peg_32.getPosition();
                            
                            result_peg_32.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_32.getPosition()))){
                                    result_peg_32.nextPosition();
                                } else {
                                    result_peg_32.setPosition(position_peg_36);
                                    goto out_peg_37;
                                }
                            }
                                
                        }
                        goto success_peg_33;
                        out_peg_37:
                        goto loop_peg_31;
                        success_peg_33:
                        ;
                        result_peg_3.addResult(result_peg_32);
                    } while (true);
                    loop_peg_31:
                    ;
                            
                }
                goto success_peg_29;
                goto out_peg_14;
                success_peg_29:
                ;
            
            
            
            result_peg_3 = rule_value(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_14;
                }
            
            
        }
        
        
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_function_rest = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_14:
        Result result_peg_38(myposition);
        
        
        
        
        
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_function_rest = result_peg_38;
        stream.update(result_peg_38.getPosition());
        
        
        return result_peg_38;
    
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_function_rest = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk10 != 0 && column_peg_2.chunk10->chunk_keys.calculated()){
        return column_peg_2.chunk10->chunk_keys;
    }
    
    
    RuleTrace trace_peg_1(stream, "keys");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3 = rule_key_value_list(stream, result_peg_3.getPosition());
        if (result_peg_3.error()){
            goto out_peg_4;
        }
        
        
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_keys = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_4:
    
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_keys = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk10 != 0 && column_peg_2.chunk10->chunk_key_value_list.calculated()){
        return column_peg_2.chunk10->chunk_key_value_list;
    }
    
    
    RuleTrace trace_peg_1(stream, "key_value_list");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_key(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                first = result_peg_3.getValues();
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_8(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_11 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_13(result_peg_8.getPosition());
                                    {
                                        int position_peg_15 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_15);
                                                goto out_peg_16;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_16:
                                    {
                                        int position_peg_17 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_17);
                                                goto out_peg_18;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_18:
                                    goto loop_peg_12;
                                    success_peg_14:
                                    ;
                                    result_peg_8.addResult(result_peg_13);
                                } while (true);
                                loop_peg_12:
                                ;
                                        
                            }
                            goto success_peg_10;
                            goto loop_peg_7;
                            success_peg_10:
                            ;
                        
                        
                        
                        result_peg_8.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    goto loop_peg_7;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_22 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_24(result_peg_8.getPosition());
                                    {
                                        int position_peg_26 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_26);
                                                goto out_peg_27;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_27:
                                    {
                                        int position_peg_28 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_28);
                                                goto out_peg_29;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_29:
                                    goto loop_peg_23;
                                    success_peg_25:
                                    ;
                                    result_peg_8.addResult(result_peg_24);
                                } while (true);
                                loop_peg_23:
                                ;
                                        
                            }
                            goto success_peg_21;
                            goto loop_peg_7;
                            success_peg_21:
                            ;
                        
                        
                        
                        result_peg_8 = rule_key(stream, result_peg_8.getPosition());
                            if (result_peg_8.error()){
                                goto loop_peg_7;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
                rest = result_peg_3.getValues();
            
            
            
            int save_peg_31 = result_peg_3.getPosition();
                {
                
                    {
                            int position_peg_34 = result_peg_3.getPosition();
                            
                            result_peg_3.reset();
                            do{
                                Result result_peg_36(result_peg_3.getPosition());
                                {
                                    int position_peg_38 = result_peg_36.getPosition();
                                    
                                    result_peg_36.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_36.getPosition()))){
                                            result_peg_36.nextPosition();
                                        } else {
                                            result_peg_36.setPosition(position_peg_38);
                                            goto out_peg_39;
                                        }
                                    }
                                        
                                }
                                goto success_peg_37;
                                out_peg_39:
                                {
                                    int position_peg_40 = result_peg_36.getPosition();
                                    
                                    result_peg_36.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_36.getPosition()))){
                                            result_peg_36.nextPosition();
                                        } else {
                                            result_peg_36.setPosition(position_peg_40);
                                            goto out_peg_41;
                                        }
                                    }
                                        
                                }
                                goto success_peg_37;
                                out_peg_41:
                                goto loop_peg_35;
                                success_peg_37:
                                ;
                                result_peg_3.addResult(result_peg_36);
                            } while (true);
                            loop_peg_35:
                            ;
                                    
                        }
                        goto success_peg_33;
                        
                        result_peg_3 = Result(save_peg_31);
                        result_peg_3.setValue(Value((void*) 0));
                        
                        success_peg_33:
                        ;
                    
                    
                    
                    result_peg_3.setValue(Value((void*) ","));
                        for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_3.getPosition()))){
                                result_peg_3.nextPosition();
                            } else {
                                
                                result_peg_3 = Result(save_peg_31);
                                result_peg_3.setValue(Value((void*) 0));
                                
                            }
                        }
                    
                    
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyList(first, rest);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_key_value_list = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_42(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = makeEmptyKeyList();
                    result_peg_42.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_key_value_list = result_peg_42;
        stream.update(result_peg_42.getPosition());
        
        
        return result_peg_42;
    
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_key_value_list = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk10 != 0 && column_peg_2.chunk10->chunk_key.calculated()){
        return column_peg_2.chunk10->chunk_key;
    }
    
    
    RuleTrace trace_peg_1(stream, "key");
    int myposition = position;
    
    
    Value ok;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_key_real(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3 = rule_key_rest(stream, result_peg_3.getPosition(), result_peg_4.getValues());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                ok = result_peg_3.getValues();
            
            
            
            Result result_peg_9(result_peg_3);
                result_peg_9 = rule_identifier(stream, result_peg_9.getPosition());
                if (result_peg_9.error()){
                    goto not_peg_8;
                }
                goto out_peg_5;
                not_peg_8:
                result_peg_3.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = ok;
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_key = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk10 == 0){
            column_peg_2.chunk10 = new Chunk10();
        }
        column_peg_2.chunk10->chunk_key = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    
    RuleTrace trace_peg_1(stream, "key_rest");
    int myposition = position;
    
    tail_peg_4:
    Value another;
        Value new_left;
    Result result_peg_3(myposition);
        {
        
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_15;
                success_peg_6:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "+"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_15;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_key_real(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_15;
                }
                another = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyCombined(left, another);
                    result_peg_3.setValue(value);
                }
                new_left = result_peg_3.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_3.getPosition();
        goto tail_peg_4;
        out_peg_15:
        Result result_peg_29(myposition);
        
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_29.getPosition());
        
        return result_peg_29;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk11 != 0 && column_peg_2.chunk11->chunk_key_real.calculated()){
        return column_peg_2.chunk11->chunk_key_real;
    }
    
    
    RuleTrace trace_peg_1(stream, "key_real");
    int myposition = position;
    
    
    Value mods;
        Value name;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.reset();
                do{
                    Result result_peg_6(result_peg_3.getPosition());
                    result_peg_6 = rule_key_modifier(stream, result_peg_6.getPosition());
                    if (result_peg_6.error()){
                        goto loop_peg_5;
                    }
                    result_peg_3.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                ;
                mods = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_9 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "DB"));
                    for (int i = 0; i < 2; i++){
                        if (compareChar("DB"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_9);
                            goto out_peg_10;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_10:
                {
                    int position_peg_11 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "B"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("B"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_11);
                            goto out_peg_12;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_12:
                {
                    int position_peg_13 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "DF"));
                    for (int i = 0; i < 2; i++){
                        if (compareChar("DF"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_13);
                            goto out_peg_14;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_14:
                {
                    int position_peg_15 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "D"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("D"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_15);
                            goto out_peg_16;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_16:
                {
                    int position_peg_17 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "F"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("F"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_17);
                            goto out_peg_18;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_18:
                {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "UF"));
                    for (int i = 0; i < 2; i++){
                        if (compareChar("UF"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_19);
                            goto out_peg_20;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_20:
                {
                    int position_peg_21 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "UB"));
                    for (int i = 0; i < 2; i++){
                        if (compareChar("UB"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_21);
                            goto out_peg_22;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_22:
                {
                    int position_peg_23 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "U"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("U"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_23);
                            goto out_peg_24;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_24:
                {
                    int position_peg_25 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "a"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("a"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_25);
                            goto out_peg_26;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_26:
                {
                    int position_peg_27 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "b"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("b"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_27);
                            goto out_peg_28;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_28:
                {
                    int position_peg_29 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "c"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("c"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_29);
                            goto out_peg_30;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_30:
                {
                    int position_peg_31 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "x"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("x"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_31);
                            goto out_peg_32;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_32:
                {
                    int position_peg_33 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "y"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("y"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_33);
                            goto out_peg_34;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_34:
                {
                    int position_peg_35 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "z"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("z"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_35);
                            goto out_peg_36;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_36:
                {
                    int position_peg_37 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "s"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("s"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_37);
                            goto out_peg_38;
                        }
                    }
                        
                }
                goto success_peg_8;
                out_peg_38:
                goto out_peg_39;
                success_peg_8:
                ;
                name = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = applyKeyModifiers(mods, makeKey(name));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_key_real = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_39:
    
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_key_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk11 != 0 && column_peg_2.chunk11->chunk_key_modifier.calculated()){
        return column_peg_2.chunk11->chunk_key_modifier;
    }
    
    
    RuleTrace trace_peg_1(stream, "key_modifier");
    int myposition = position;
    
    
    Value num;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "~"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_8(result_peg_3.getPosition());
                    {
                        int position_peg_10 = result_peg_8.getPosition();
                        
                        char letter_peg_11 = stream.get(result_peg_8.getPosition());
                        if (letter_peg_11 != '\0' && strchr("0123456789", letter_peg_11) != NULL){
                            result_peg_8.nextPosition();
                            result_peg_8.setValue(Value((void*) (intptr_t) letter_peg_11));
                        } else {
                            result_peg_8.setPosition(position_peg_10);
                            goto out_peg_12;
                        }
                        
                    }
                    goto success_peg_9;
                    out_peg_12:
                    goto loop_peg_7;
                    success_peg_9:
                    ;
                    result_peg_3.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
                num = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new ReleaseKeyModifier((int) *parseDouble(num));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_key_modifier = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_13(myposition);
        
        
        {
        
            result_peg_13.setValue(Value((void*) "$"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("$"[i], stream.get(result_peg_13.getPosition()))){
                        result_peg_13.nextPosition();
                    } else {
                        goto out_peg_15;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = new DirectionKeyModifier();
                    result_peg_13.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_key_modifier = result_peg_13;
        stream.update(result_peg_13.getPosition());
        
        
        return result_peg_13;
        out_peg_15:
        Result result_peg_16(myposition);
        
        
        {
        
            result_peg_16.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_16.getPosition()))){
                        result_peg_16.nextPosition();
                    } else {
                        goto out_peg_18;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = new HeldDownKeyModifier();
                    result_peg_16.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_key_modifier = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_18:
        Result result_peg_19(myposition);
        
        
        {
        
            result_peg_19.setValue(Value((void*) ">"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_19.getPosition()))){
                        result_peg_19.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = new OnlyKeyModifier();
                    result_peg_19.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_key_modifier = result_peg_19;
        stream.update(result_peg_19.getPosition());
        
        
        return result_peg_19;
        out_peg_21:
    
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_key_modifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk11 != 0 && column_peg_2.chunk11->chunk_value.calculated()){
        return column_peg_2.chunk11->chunk_value;
    }
    
    
    RuleTrace trace_peg_1(stream, "value");
    int myposition = position;
    
    
    Value contents;
    Result result_peg_3(myposition);
        
        
        result_peg_3 = rule_float(stream, result_peg_3.getPosition());
        if (result_peg_3.error()){
            goto out_peg_4;
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_4:
        Result result_peg_5(myposition);
        
        
        result_peg_5 = rule_integer(stream, result_peg_5.getPosition());
        if (result_peg_5.error()){
            goto out_peg_6;
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_6:
        Result result_peg_7(myposition);
        
        
        result_peg_7 = rule_keyword(stream, result_peg_7.getPosition());
        if (result_peg_7.error()){
            goto out_peg_8;
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_7;
        stream.update(result_peg_7.getPosition());
        
        
        return result_peg_7;
        out_peg_8:
        Result result_peg_9(myposition);
        
        
        result_peg_9 = rule_resource(stream, result_peg_9.getPosition());
        if (result_peg_9.error()){
            goto out_peg_10;
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_9;
        stream.update(result_peg_9.getPosition());
        
        
        return result_peg_9;
        out_peg_10:
        Result result_peg_11(myposition);
        
        
        {
        
            Result result_peg_14(result_peg_11);
                result_peg_14 = rule_keyword(stream, result_peg_14.getPosition());
                if (result_peg_14.error()){
                    goto not_peg_13;
                }
                goto out_peg_15;
                not_peg_13:
                result_peg_11.setValue(Value((void*)0));
            
            
            
            result_peg_11 = rule_identifier(stream, result_peg_11.getPosition());
                if (result_peg_11.error()){
                    goto out_peg_15;
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_15:
        Result result_peg_16(myposition);
        
        
        result_peg_16 = rule_range(stream, result_peg_16.getPosition());
        if (result_peg_16.error()){
            goto out_peg_17;
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_17:
        Result result_peg_18(myposition);
        
        
        {
            int position_peg_20 = result_peg_18.getPosition();
            {
            
                result_peg_18.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_18.getPosition()))){
                            result_peg_18.nextPosition();
                        } else {
                            result_peg_18.setPosition(position_peg_20);
                            goto out_peg_22;
                        }
                    }
                
                
                
                result_peg_18.reset();
                    do{
                        Result result_peg_25(result_peg_18.getPosition());
                        {
                        
                            Result result_peg_28(result_peg_25);
                                result_peg_28.setValue(Value((void*) "\""));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\""[i], stream.get(result_peg_28.getPosition()))){
                                        result_peg_28.nextPosition();
                                    } else {
                                        goto not_peg_27;
                                    }
                                }
                                goto loop_peg_24;
                                not_peg_27:
                                result_peg_25.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_31(result_peg_25);
                                result_peg_31.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_31.getPosition()))){
                                        result_peg_31.nextPosition();
                                    } else {
                                        goto not_peg_30;
                                    }
                                }
                                goto loop_peg_24;
                                not_peg_30:
                                result_peg_25.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_32 = stream.get(result_peg_25.getPosition());
                                if (temp_peg_32 != '\0'){
                                    result_peg_25.setValue(Value((void*) (intptr_t) temp_peg_32));
                                    result_peg_25.nextPosition();
                                } else {
                                    goto loop_peg_24;
                                }
                            
                            
                        }
                        result_peg_18.addResult(result_peg_25);
                    } while (true);
                    loop_peg_24:
                    ;
                    contents = result_peg_18.getValues();
                
                
                
                result_peg_18.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_18.getPosition()))){
                            result_peg_18.nextPosition();
                        } else {
                            result_peg_18.setPosition(position_peg_20);
                            goto out_peg_22;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = makeString(contents);
                        result_peg_18.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_19;
        out_peg_22:
        goto out_peg_34;
        success_peg_19:
        ;
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_18;
        stream.update(result_peg_18.getPosition());
        
        
        return result_peg_18;
        out_peg_34:
        Result result_peg_35(myposition);
        
        
        result_peg_35 = rule_hitflag(stream, result_peg_35.getPosition());
        if (result_peg_35.error()){
            goto out_peg_36;
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = result_peg_35;
        stream.update(result_peg_35.getPosition());
        
        
        return result_peg_35;
        out_peg_36:
    
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_resource(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk11 != 0 && column_peg_2.chunk11->chunk_resource.calculated()){
        return column_peg_2.chunk11->chunk_resource;
    }
    
    
    RuleTrace trace_peg_1(stream, "resource");
    int myposition = position;
    
    
    Value any;
    Result result_peg_3(myposition);
        
        
        {
        
            Result result_peg_6(result_peg_3);
                result_peg_6 = rule_resource__s(stream, result_peg_6.getPosition());
                if (result_peg_6.error()){
                    goto not_peg_5;
                }
                goto out_peg_7;
                not_peg_5:
                result_peg_3.setValue(Value((void*)0));
            
            
            
            result_peg_3.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
            
            
            
            Result result_peg_11(result_peg_3);
                {
                    int position_peg_13 = result_peg_11.getPosition();
                    {
                    
                        result_peg_11.setValue(Value((void*) "c"));
                            for (int i = 0; i < 1; i++){
                                if (compareCharCase("c"[i], stream.get(result_peg_11.getPosition()))){
                                    result_peg_11.nextPosition();
                                } else {
                                    result_peg_11.setPosition(position_peg_13);
                                    goto out_peg_15;
                                }
                            }
                        
                        
                        
                        result_peg_11.setValue(Value((void*) "a"));
                            for (int i = 0; i < 1; i++){
                                if (compareCharCase("a"[i], stream.get(result_peg_11.getPosition()))){
                                    result_peg_11.nextPosition();
                                } else {
                                    result_peg_11.setPosition(position_peg_13);
                                    goto out_peg_15;
                                }
                            }
                        
                        
                    }
                }
                goto success_peg_12;
                out_peg_15:
                {
                    int position_peg_16 = result_peg_11.getPosition();
                    {
                    
                        result_peg_11.setValue(Value((void*) "a"));
                            for (int i = 0; i < 1; i++){
                                if (compareCharCase("a"[i], stream.get(result_peg_11.getPosition()))){
                                    result_peg_11.nextPosition();
                                } else {
                                    result_peg_11.setPosition(position_peg_16);
                                    goto out_peg_18;
                                }
                            }
                        
                        
                        
                        result_peg_11.setValue(Value((void*) "c"));
                            for (int i = 0; i < 1; i++){
                                if (compareCharCase("c"[i], stream.get(result_peg_11.getPosition()))){
                                    result_peg_11.nextPosition();
                                } else {
                                    result_peg_11.setPosition(position_peg_16);
                                    goto out_peg_18;
                                }
                            }
                        
                        
                    }
                }
                goto success_peg_12;
                out_peg_18:
                {
                    int position_peg_19 = result_peg_11.getPosition();
                    
                    result_peg_11.setValue(Value((void*) "c"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("c"[i], stream.get(result_peg_11.getPosition()))){
                            result_peg_11.nextPosition();
                        } else {
                            result_peg_11.setPosition(position_peg_19);
                            goto out_peg_20;
                        }
                    }
                        
                }
                goto success_peg_12;
                out_peg_20:
                {
                    int position_peg_21 = result_peg_11.getPosition();
                    
                    result_peg_11.setValue(Value((void*) "a"));
                    for (int i = 0; i < 1; i++){
                        if (compareCharCase("a"[i], stream.get(result_peg_11.getPosition()))){
                            result_peg_11.nextPosition();
                        } else {
                            result_peg_11.setPosition(position_peg_21);
                            goto out_peg_22;
                        }
                    }
                        
                }
                goto success_peg_12;
                out_peg_22:
                goto not_peg_10;
                success_peg_12:
                ;
                goto out_peg_7;
                not_peg_10:
                result_peg_3.setValue(Value((void*)0));
            
            
            
            result_peg_3 = rule_expr_c(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_7;
                }
                any = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeOwnResource(any);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_7:
        Result result_peg_24(myposition);
        
        
        {
        
            Result result_peg_27(result_peg_24);
                result_peg_27 = rule_resource__f(stream, result_peg_27.getPosition());
                if (result_peg_27.error()){
                    goto not_peg_26;
                }
                goto out_peg_28;
                not_peg_26:
                result_peg_24.setValue(Value((void*)0));
            
            
            
            result_peg_24.setValue(Value((void*) "f"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("f"[i], stream.get(result_peg_24.getPosition()))){
                        result_peg_24.nextPosition();
                    } else {
                        goto out_peg_28;
                    }
                }
            
            
            
            result_peg_24 = rule_expr_c(stream, result_peg_24.getPosition());
                if (result_peg_24.error()){
                    goto out_peg_28;
                }
                any = result_peg_24.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFightResource(any);
                    result_peg_24.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource = result_peg_24;
        stream.update(result_peg_24.getPosition());
        
        
        return result_peg_24;
        out_peg_28:
    
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_resource__s(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk11 != 0 && column_peg_2.chunk11->chunk_resource__s.calculated()){
        return column_peg_2.chunk11->chunk_resource__s;
    }
    
    
    RuleTrace trace_peg_1(stream, "resource__s");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3.setValue(Value((void*) "stateno"));
        for (int i = 0; i < 7; i++){
            if (compareCharCase("stateno"[i], stream.get(result_peg_3.getPosition()))){
                result_peg_3.nextPosition();
            } else {
                goto out_peg_4;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_4:
        Result result_peg_5(myposition);
        
        
        result_peg_5.setValue(Value((void*) "statetime"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("statetime"[i], stream.get(result_peg_5.getPosition()))){
                result_peg_5.nextPosition();
            } else {
                goto out_peg_6;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_6:
        Result result_peg_7(myposition);
        
        
        result_peg_7.setValue(Value((void*) "statetype"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("statetype"[i], stream.get(result_peg_7.getPosition()))){
                result_peg_7.nextPosition();
            } else {
                goto out_peg_8;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_7;
        stream.update(result_peg_7.getPosition());
        
        
        return result_peg_7;
        out_peg_8:
        Result result_peg_9(myposition);
        
        
        result_peg_9.setValue(Value((void*) "selfstate"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("selfstate"[i], stream.get(result_peg_9.getPosition()))){
                result_peg_9.nextPosition();
            } else {
                goto out_peg_10;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_9;
        stream.update(result_peg_9.getPosition());
        
        
        return result_peg_9;
        out_peg_10:
        Result result_peg_11(myposition);
        
        
        result_peg_11.setValue(Value((void*) "slidetime"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("slidetime"[i], stream.get(result_peg_11.getPosition()))){
                result_peg_11.nextPosition();
            } else {
                goto out_peg_12;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_12:
        Result result_peg_13(myposition);
        
        
        result_peg_13.setValue(Value((void*) "statetypeset"));
        for (int i = 0; i < 12; i++){
            if (compareCharCase("statetypeset"[i], stream.get(result_peg_13.getPosition()))){
                result_peg_13.nextPosition();
            } else {
                goto out_peg_14;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_13;
        stream.update(result_peg_13.getPosition());
        
        
        return result_peg_13;
        out_peg_14:
        Result result_peg_15(myposition);
        
        
        result_peg_15.setValue(Value((void*) "superpause"));
        for (int i = 0; i < 10; i++){
            if (compareCharCase("superpause"[i], stream.get(result_peg_15.getPosition()))){
                result_peg_15.nextPosition();
            } else {
                goto out_peg_16;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_15;
        stream.update(result_peg_15.getPosition());
        
        
        return result_peg_15;
        out_peg_16:
        Result result_peg_17(myposition);
        
        
        result_peg_17.setValue(Value((void*) "screenbound"));
        for (int i = 0; i < 11; i++){
            if (compareCharCase("screenbound"[i], stream.get(result_peg_17.getPosition()))){
                result_peg_17.nextPosition();
            } else {
                goto out_peg_18;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_17;
        stream.update(result_peg_17.getPosition());
        
        
        return result_peg_17;
        out_peg_18:
        Result result_peg_19(myposition);
        
        
        result_peg_19.setValue(Value((void*) "sprpriority"));
        for (int i = 0; i < 11; i++){
            if (compareCharCase("sprpriority"[i], stream.get(result_peg_19.getPosition()))){
                result_peg_19.nextPosition();
            } else {
                goto out_peg_20;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_19;
        stream.update(result_peg_19.getPosition());
        
        
        return result_peg_19;
        out_peg_20:
        Result result_peg_21(myposition);
        
        
        result_peg_21.setValue(Value((void*) "sndpan"));
        for (int i = 0; i < 6; i++){
            if (compareCharCase("sndpan"[i], stream.get(result_peg_21.getPosition()))){
                result_peg_21.nextPosition();
            } else {
                goto out_peg_22;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_21;
        stream.update(result_peg_21.getPosition());
        
        
        return result_peg_21;
        out_peg_22:
        Result result_peg_23(myposition);
        
        
        result_peg_23.setValue(Value((void*) "stopsnd"));
        for (int i = 0; i < 7; i++){
            if (compareCharCase("stopsnd"[i], stream.get(result_peg_23.getPosition()))){
                result_peg_23.nextPosition();
            } else {
                goto out_peg_24;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_23;
        stream.update(result_peg_23.getPosition());
        
        
        return result_peg_23;
        out_peg_24:
        Result result_peg_25(myposition);
        
        
        {
            int position_peg_27 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "abs"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_27);
                    goto out_peg_28;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_28:
        {
            int position_peg_29 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "const"));
            for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_29);
                    goto out_peg_30;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_30:
        {
            int position_peg_31 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "selfanimexist"));
            for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_31);
                    goto out_peg_32;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_32:
        {
            int position_peg_33 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "ifelse"));
            for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_33);
                    goto out_peg_34;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_34:
        {
            int position_peg_35 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "gethitvar"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_35);
                    goto out_peg_36;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_36:
        {
            int position_peg_37 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "floor"));
            for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_37);
                    goto out_peg_38;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_38:
        {
            int position_peg_39 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "ceil"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_39);
                    goto out_peg_40;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_40:
        {
            int position_peg_41 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "exp"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("exp"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_41);
                    goto out_peg_42;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_42:
        {
            int position_peg_43 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "acos"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("acos"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_43);
                    goto out_peg_44;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_44:
        {
            int position_peg_45 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "asin"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("asin"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_45);
                    goto out_peg_46;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_46:
        {
            int position_peg_47 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "atan"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("atan"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_47);
                    goto out_peg_48;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_48:
        {
            int position_peg_49 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "tan"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("tan"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_49);
                    goto out_peg_50;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_50:
        {
            int position_peg_51 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "cos"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("cos"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_51);
                    goto out_peg_52;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_52:
        {
            int position_peg_53 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "sin"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("sin"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_53);
                    goto out_peg_54;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_54:
        {
            int position_peg_55 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "log"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("log"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_55);
                    goto out_peg_56;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_56:
        {
            int position_peg_57 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "ln"));
            for (int i = 0; i < 2; i++){
                if (compareCharCase("ln"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_57);
                    goto out_peg_58;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_58:
        {
            int position_peg_59 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "sysfvar"));
            for (int i = 0; i < 7; i++){
                if (compareCharCase("sysfvar"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_59);
                    goto out_peg_60;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_60:
        {
            int position_peg_61 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "sysvar"));
            for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_61);
                    goto out_peg_62;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_62:
        {
            int position_peg_63 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "var"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_63);
                    goto out_peg_64;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_64:
        {
            int position_peg_65 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "numexplod"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_65);
                    goto out_peg_66;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_66:
        {
            int position_peg_67 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "numhelper"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_67);
                    goto out_peg_68;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_68:
        {
            int position_peg_69 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "numprojid"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_69);
                    goto out_peg_70;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_70:
        {
            int position_peg_71 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "fvar"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_71);
                    goto out_peg_72;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_72:
        {
            int position_peg_73 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "ishelper"));
            for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_73);
                    goto out_peg_74;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_74:
        {
            int position_peg_75 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "numtarget"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numtarget"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_75);
                    goto out_peg_76;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_76:
        {
            int position_peg_77 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "animelemtime"));
            for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_77);
                    goto out_peg_78;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_78:
        {
            int position_peg_79 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "animelemno"));
            for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_79);
                    goto out_peg_80;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_80:
        {
            int position_peg_81 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "animexist"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_81);
                    goto out_peg_82;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_82:
        {
            int position_peg_83 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "playeridexist"));
            for (int i = 0; i < 13; i++){
                if (compareCharCase("playeridexist"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_83);
                    goto out_peg_84;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_84:
        {
            int position_peg_85 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "projguarded"));
            for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_85);
                    goto out_peg_86;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_86:
        {
            int position_peg_87 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "projcanceltime"));
            for (int i = 0; i < 14; i++){
                if (compareCharCase("projcanceltime"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_87);
                    goto out_peg_88;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_88:
        {
            int position_peg_89 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "projhittime"));
            for (int i = 0; i < 11; i++){
                if (compareCharCase("projhittime"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_89);
                    goto out_peg_90;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_90:
        {
            int position_peg_91 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "projhit"));
            for (int i = 0; i < 7; i++){
                if (compareCharCase("projhit"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_91);
                    goto out_peg_92;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_92:
        {
            int position_peg_93 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "projcontacttime"));
            for (int i = 0; i < 15; i++){
                if (compareCharCase("projcontacttime"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_93);
                    goto out_peg_94;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_94:
        {
            int position_peg_95 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "projcontact"));
            for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_95);
                    goto out_peg_96;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_96:
        {
            int position_peg_97 = result_peg_25.getPosition();
            
            result_peg_25.setValue(Value((void*) "numhelper"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_25.getPosition()))){
                    result_peg_25.nextPosition();
                } else {
                    result_peg_25.setPosition(position_peg_97);
                    goto out_peg_98;
                }
            }
                
        }
        goto success_peg_26;
        out_peg_98:
        goto out_peg_99;
        success_peg_26:
        ;
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_25;
        stream.update(result_peg_25.getPosition());
        
        
        return result_peg_25;
        out_peg_99:
        Result result_peg_100(myposition);
        
        
        result_peg_100 = rule_keyword(stream, result_peg_100.getPosition());
        if (result_peg_100.error()){
            goto out_peg_101;
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_100;
        stream.update(result_peg_100.getPosition());
        
        
        return result_peg_100;
        out_peg_101:
        Result result_peg_102(myposition);
        
        
        result_peg_102.setValue(Value((void*) "size.xscale"));
        for (int i = 0; i < 11; i++){
            if (compareCharCase("size.xscale"[i], stream.get(result_peg_102.getPosition()))){
                result_peg_102.nextPosition();
            } else {
                goto out_peg_103;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_102;
        stream.update(result_peg_102.getPosition());
        
        
        return result_peg_102;
        out_peg_103:
        Result result_peg_104(myposition);
        
        
        result_peg_104.setValue(Value((void*) "size.yscale"));
        for (int i = 0; i < 11; i++){
            if (compareCharCase("size.yscale"[i], stream.get(result_peg_104.getPosition()))){
                result_peg_104.nextPosition();
            } else {
                goto out_peg_105;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_104;
        stream.update(result_peg_104.getPosition());
        
        
        return result_peg_104;
        out_peg_105:
        Result result_peg_106(myposition);
        
        
        result_peg_106.setValue(Value((void*) "size.ground.back"));
        for (int i = 0; i < 16; i++){
            if (compareCharCase("size.ground.back"[i], stream.get(result_peg_106.getPosition()))){
                result_peg_106.nextPosition();
            } else {
                goto out_peg_107;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_106;
        stream.update(result_peg_106.getPosition());
        
        
        return result_peg_106;
        out_peg_107:
        Result result_peg_108(myposition);
        
        
        result_peg_108.setValue(Value((void*) "size.ground.front"));
        for (int i = 0; i < 17; i++){
            if (compareCharCase("size.ground.front"[i], stream.get(result_peg_108.getPosition()))){
                result_peg_108.nextPosition();
            } else {
                goto out_peg_109;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_108;
        stream.update(result_peg_108.getPosition());
        
        
        return result_peg_108;
        out_peg_109:
        Result result_peg_110(myposition);
        
        
        result_peg_110.setValue(Value((void*) "size.air.back"));
        for (int i = 0; i < 13; i++){
            if (compareCharCase("size.air.back"[i], stream.get(result_peg_110.getPosition()))){
                result_peg_110.nextPosition();
            } else {
                goto out_peg_111;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_110;
        stream.update(result_peg_110.getPosition());
        
        
        return result_peg_110;
        out_peg_111:
        Result result_peg_112(myposition);
        
        
        result_peg_112.setValue(Value((void*) "size.air.front"));
        for (int i = 0; i < 14; i++){
            if (compareCharCase("size.air.front"[i], stream.get(result_peg_112.getPosition()))){
                result_peg_112.nextPosition();
            } else {
                goto out_peg_113;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_112;
        stream.update(result_peg_112.getPosition());
        
        
        return result_peg_112;
        out_peg_113:
        Result result_peg_114(myposition);
        
        
        result_peg_114.setValue(Value((void*) "size.height"));
        for (int i = 0; i < 11; i++){
            if (compareCharCase("size.height"[i], stream.get(result_peg_114.getPosition()))){
                result_peg_114.nextPosition();
            } else {
                goto out_peg_115;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_114;
        stream.update(result_peg_114.getPosition());
        
        
        return result_peg_114;
        out_peg_115:
        Result result_peg_116(myposition);
        
        
        result_peg_116.setValue(Value((void*) "size.attack.dist"));
        for (int i = 0; i < 16; i++){
            if (compareCharCase("size.attack.dist"[i], stream.get(result_peg_116.getPosition()))){
                result_peg_116.nextPosition();
            } else {
                goto out_peg_117;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_116;
        stream.update(result_peg_116.getPosition());
        
        
        return result_peg_116;
        out_peg_117:
        Result result_peg_118(myposition);
        
        
        result_peg_118.setValue(Value((void*) "size.proj.attack.dist"));
        for (int i = 0; i < 21; i++){
            if (compareCharCase("size.proj.attack.dist"[i], stream.get(result_peg_118.getPosition()))){
                result_peg_118.nextPosition();
            } else {
                goto out_peg_119;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_118;
        stream.update(result_peg_118.getPosition());
        
        
        return result_peg_118;
        out_peg_119:
        Result result_peg_120(myposition);
        
        
        result_peg_120.setValue(Value((void*) "size.proj.doscale"));
        for (int i = 0; i < 17; i++){
            if (compareCharCase("size.proj.doscale"[i], stream.get(result_peg_120.getPosition()))){
                result_peg_120.nextPosition();
            } else {
                goto out_peg_121;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_120;
        stream.update(result_peg_120.getPosition());
        
        
        return result_peg_120;
        out_peg_121:
        Result result_peg_122(myposition);
        
        
        result_peg_122.setValue(Value((void*) "size.head.pos.x"));
        for (int i = 0; i < 15; i++){
            if (compareCharCase("size.head.pos.x"[i], stream.get(result_peg_122.getPosition()))){
                result_peg_122.nextPosition();
            } else {
                goto out_peg_123;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_122;
        stream.update(result_peg_122.getPosition());
        
        
        return result_peg_122;
        out_peg_123:
        Result result_peg_124(myposition);
        
        
        result_peg_124.setValue(Value((void*) "size.head.pos.y"));
        for (int i = 0; i < 15; i++){
            if (compareCharCase("size.head.pos.y"[i], stream.get(result_peg_124.getPosition()))){
                result_peg_124.nextPosition();
            } else {
                goto out_peg_125;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_124;
        stream.update(result_peg_124.getPosition());
        
        
        return result_peg_124;
        out_peg_125:
        Result result_peg_126(myposition);
        
        
        result_peg_126.setValue(Value((void*) "size.mid.pos.x"));
        for (int i = 0; i < 14; i++){
            if (compareCharCase("size.mid.pos.x"[i], stream.get(result_peg_126.getPosition()))){
                result_peg_126.nextPosition();
            } else {
                goto out_peg_127;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_126;
        stream.update(result_peg_126.getPosition());
        
        
        return result_peg_126;
        out_peg_127:
        Result result_peg_128(myposition);
        
        
        result_peg_128.setValue(Value((void*) "size.mid.pos.y"));
        for (int i = 0; i < 14; i++){
            if (compareCharCase("size.mid.pos.y"[i], stream.get(result_peg_128.getPosition()))){
                result_peg_128.nextPosition();
            } else {
                goto out_peg_129;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_128;
        stream.update(result_peg_128.getPosition());
        
        
        return result_peg_128;
        out_peg_129:
        Result result_peg_130(myposition);
        
        
        result_peg_130.setValue(Value((void*) "size.shadowoffset"));
        for (int i = 0; i < 17; i++){
            if (compareCharCase("size.shadowoffset"[i], stream.get(result_peg_130.getPosition()))){
                result_peg_130.nextPosition();
            } else {
                goto out_peg_131;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_130;
        stream.update(result_peg_130.getPosition());
        
        
        return result_peg_130;
        out_peg_131:
        Result result_peg_132(myposition);
        
        
        result_peg_132.setValue(Value((void*) "size.draw.offset.x"));
        for (int i = 0; i < 18; i++){
            if (compareCharCase("size.draw.offset.x"[i], stream.get(result_peg_132.getPosition()))){
                result_peg_132.nextPosition();
            } else {
                goto out_peg_133;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_132;
        stream.update(result_peg_132.getPosition());
        
        
        return result_peg_132;
        out_peg_133:
        Result result_peg_134(myposition);
        
        
        result_peg_134.setValue(Value((void*) "size.draw.offset.y"));
        for (int i = 0; i < 18; i++){
            if (compareCharCase("size.draw.offset.y"[i], stream.get(result_peg_134.getPosition()))){
                result_peg_134.nextPosition();
            } else {
                goto out_peg_135;
            }
        }
        
        
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = result_peg_134;
        stream.update(result_peg_134.getPosition());
        
        
        return result_peg_134;
        out_peg_135:
    
        if (column_peg_2.chunk11 == 0){
            column_peg_2.chunk11 = new Chunk11();
        }
        column_peg_2.chunk11->chunk_resource__s = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_resource__f(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk12 != 0 && column_peg_2.chunk12->chunk_resource__f.calculated()){
        return column_peg_2.chunk12->chunk_resource__f;
    }
    
    
    RuleTrace trace_peg_1(stream, "resource__f");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3.setValue(Value((void*) "frontedgebodydist"));
        for (int i = 0; i < 17; i++){
            if (compareCharCase("frontedgebodydist"[i], stream.get(result_peg_3.getPosition()))){
                result_peg_3.nextPosition();
            } else {
                goto out_peg_4;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_4:
        Result result_peg_5(myposition);
        
        
        result_peg_5.setValue(Value((void*) "frontedgedist"));
        for (int i = 0; i < 13; i++){
            if (compareCharCase("frontedgedist"[i], stream.get(result_peg_5.getPosition()))){
                result_peg_5.nextPosition();
            } else {
                goto out_peg_6;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_6:
        Result result_peg_7(myposition);
        
        
        result_peg_7.setValue(Value((void*) "front"));
        for (int i = 0; i < 5; i++){
            if (compareCharCase("front"[i], stream.get(result_peg_7.getPosition()))){
                result_peg_7.nextPosition();
            } else {
                goto out_peg_8;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_7;
        stream.update(result_peg_7.getPosition());
        
        
        return result_peg_7;
        out_peg_8:
        Result result_peg_9(myposition);
        
        
        result_peg_9.setValue(Value((void*) "forcefeedback"));
        for (int i = 0; i < 13; i++){
            if (compareCharCase("forcefeedback"[i], stream.get(result_peg_9.getPosition()))){
                result_peg_9.nextPosition();
            } else {
                goto out_peg_10;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_9;
        stream.update(result_peg_9.getPosition());
        
        
        return result_peg_9;
        out_peg_10:
        Result result_peg_11(myposition);
        
        
        result_peg_11.setValue(Value((void*) "fallenvshake"));
        for (int i = 0; i < 12; i++){
            if (compareCharCase("fallenvshake"[i], stream.get(result_peg_11.getPosition()))){
                result_peg_11.nextPosition();
            } else {
                goto out_peg_12;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_12:
        Result result_peg_13(myposition);
        
        
        result_peg_13.setValue(Value((void*) "facing"));
        for (int i = 0; i < 6; i++){
            if (compareCharCase("facing"[i], stream.get(result_peg_13.getPosition()))){
                result_peg_13.nextPosition();
            } else {
                goto out_peg_14;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_13;
        stream.update(result_peg_13.getPosition());
        
        
        return result_peg_13;
        out_peg_14:
        Result result_peg_15(myposition);
        
        
        result_peg_15.setValue(Value((void*) "fallcount"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("fallcount"[i], stream.get(result_peg_15.getPosition()))){
                result_peg_15.nextPosition();
            } else {
                goto out_peg_16;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_15;
        stream.update(result_peg_15.getPosition());
        
        
        return result_peg_15;
        out_peg_16:
        Result result_peg_17(myposition);
        
        
        result_peg_17.setValue(Value((void*) "fall.damage"));
        for (int i = 0; i < 11; i++){
            if (compareCharCase("fall.damage"[i], stream.get(result_peg_17.getPosition()))){
                result_peg_17.nextPosition();
            } else {
                goto out_peg_18;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_17;
        stream.update(result_peg_17.getPosition());
        
        
        return result_peg_17;
        out_peg_18:
        Result result_peg_19(myposition);
        
        
        result_peg_19.setValue(Value((void*) "fall.xvel"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("fall.xvel"[i], stream.get(result_peg_19.getPosition()))){
                result_peg_19.nextPosition();
            } else {
                goto out_peg_20;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_19;
        stream.update(result_peg_19.getPosition());
        
        
        return result_peg_19;
        out_peg_20:
        Result result_peg_21(myposition);
        
        
        result_peg_21.setValue(Value((void*) "fall.yvel"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("fall.yvel"[i], stream.get(result_peg_21.getPosition()))){
                result_peg_21.nextPosition();
            } else {
                goto out_peg_22;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_21;
        stream.update(result_peg_21.getPosition());
        
        
        return result_peg_21;
        out_peg_22:
        Result result_peg_23(myposition);
        
        
        result_peg_23.setValue(Value((void*) "fall.recover"));
        for (int i = 0; i < 12; i++){
            if (compareCharCase("fall.recover"[i], stream.get(result_peg_23.getPosition()))){
                result_peg_23.nextPosition();
            } else {
                goto out_peg_24;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_23;
        stream.update(result_peg_23.getPosition());
        
        
        return result_peg_23;
        out_peg_24:
        Result result_peg_25(myposition);
        
        
        result_peg_25.setValue(Value((void*) "fall.time"));
        for (int i = 0; i < 9; i++){
            if (compareCharCase("fall.time"[i], stream.get(result_peg_25.getPosition()))){
                result_peg_25.nextPosition();
            } else {
                goto out_peg_26;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_25;
        stream.update(result_peg_25.getPosition());
        
        
        return result_peg_25;
        out_peg_26:
        Result result_peg_27(myposition);
        
        
        result_peg_27.setValue(Value((void*) "fall.recovertime"));
        for (int i = 0; i < 16; i++){
            if (compareCharCase("fall.recovertime"[i], stream.get(result_peg_27.getPosition()))){
                result_peg_27.nextPosition();
            } else {
                goto out_peg_28;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_27;
        stream.update(result_peg_27.getPosition());
        
        
        return result_peg_27;
        out_peg_28:
        Result result_peg_29(myposition);
        
        
        result_peg_29.setValue(Value((void*) "fall"));
        for (int i = 0; i < 4; i++){
            if (compareCharCase("fall"[i], stream.get(result_peg_29.getPosition()))){
                result_peg_29.nextPosition();
            } else {
                goto out_peg_30;
            }
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_29;
        stream.update(result_peg_29.getPosition());
        
        
        return result_peg_29;
        out_peg_30:
        Result result_peg_31(myposition);
        
        
        {
            int position_peg_33 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "abs"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_33);
                    goto out_peg_34;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_34:
        {
            int position_peg_35 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "const"));
            for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_35);
                    goto out_peg_36;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_36:
        {
            int position_peg_37 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "selfanimexist"));
            for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_37);
                    goto out_peg_38;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_38:
        {
            int position_peg_39 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "ifelse"));
            for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_39);
                    goto out_peg_40;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_40:
        {
            int position_peg_41 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "gethitvar"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_41);
                    goto out_peg_42;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_42:
        {
            int position_peg_43 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "floor"));
            for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_43);
                    goto out_peg_44;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_44:
        {
            int position_peg_45 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "ceil"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_45);
                    goto out_peg_46;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_46:
        {
            int position_peg_47 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "exp"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("exp"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_47);
                    goto out_peg_48;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_48:
        {
            int position_peg_49 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "acos"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("acos"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_49);
                    goto out_peg_50;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_50:
        {
            int position_peg_51 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "asin"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("asin"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_51);
                    goto out_peg_52;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_52:
        {
            int position_peg_53 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "atan"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("atan"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_53);
                    goto out_peg_54;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_54:
        {
            int position_peg_55 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "tan"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("tan"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_55);
                    goto out_peg_56;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_56:
        {
            int position_peg_57 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "cos"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("cos"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_57);
                    goto out_peg_58;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_58:
        {
            int position_peg_59 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "sin"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("sin"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_59);
                    goto out_peg_60;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_60:
        {
            int position_peg_61 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "log"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("log"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_61);
                    goto out_peg_62;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_62:
        {
            int position_peg_63 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "ln"));
            for (int i = 0; i < 2; i++){
                if (compareCharCase("ln"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_63);
                    goto out_peg_64;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_64:
        {
            int position_peg_65 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "sysfvar"));
            for (int i = 0; i < 7; i++){
                if (compareCharCase("sysfvar"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_65);
                    goto out_peg_66;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_66:
        {
            int position_peg_67 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "sysvar"));
            for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_67);
                    goto out_peg_68;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_68:
        {
            int position_peg_69 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "var"));
            for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_69);
                    goto out_peg_70;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_70:
        {
            int position_peg_71 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "numexplod"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_71);
                    goto out_peg_72;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_72:
        {
            int position_peg_73 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "numhelper"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_73);
                    goto out_peg_74;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_74:
        {
            int position_peg_75 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "numprojid"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_75);
                    goto out_peg_76;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_76:
        {
            int position_peg_77 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "fvar"));
            for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_77);
                    goto out_peg_78;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_78:
        {
            int position_peg_79 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "ishelper"));
            for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_79);
                    goto out_peg_80;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_80:
        {
            int position_peg_81 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "numtarget"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numtarget"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_81);
                    goto out_peg_82;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_82:
        {
            int position_peg_83 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "animelemtime"));
            for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_83);
                    goto out_peg_84;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_84:
        {
            int position_peg_85 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "animelemno"));
            for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_85);
                    goto out_peg_86;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_86:
        {
            int position_peg_87 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "animexist"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_87);
                    goto out_peg_88;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_88:
        {
            int position_peg_89 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "playeridexist"));
            for (int i = 0; i < 13; i++){
                if (compareCharCase("playeridexist"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_89);
                    goto out_peg_90;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_90:
        {
            int position_peg_91 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "projguarded"));
            for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_91);
                    goto out_peg_92;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_92:
        {
            int position_peg_93 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "projcanceltime"));
            for (int i = 0; i < 14; i++){
                if (compareCharCase("projcanceltime"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_93);
                    goto out_peg_94;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_94:
        {
            int position_peg_95 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "projhittime"));
            for (int i = 0; i < 11; i++){
                if (compareCharCase("projhittime"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_95);
                    goto out_peg_96;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_96:
        {
            int position_peg_97 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "projhit"));
            for (int i = 0; i < 7; i++){
                if (compareCharCase("projhit"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_97);
                    goto out_peg_98;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_98:
        {
            int position_peg_99 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "projcontacttime"));
            for (int i = 0; i < 15; i++){
                if (compareCharCase("projcontacttime"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_99);
                    goto out_peg_100;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_100:
        {
            int position_peg_101 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "projcontact"));
            for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_101);
                    goto out_peg_102;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_102:
        {
            int position_peg_103 = result_peg_31.getPosition();
            
            result_peg_31.setValue(Value((void*) "numhelper"));
            for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_31.getPosition()))){
                    result_peg_31.nextPosition();
                } else {
                    result_peg_31.setPosition(position_peg_103);
                    goto out_peg_104;
                }
            }
                
        }
        goto success_peg_32;
        out_peg_104:
        goto out_peg_105;
        success_peg_32:
        ;
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_31;
        stream.update(result_peg_31.getPosition());
        
        
        return result_peg_31;
        out_peg_105:
        Result result_peg_106(myposition);
        
        
        result_peg_106 = rule_keyword(stream, result_peg_106.getPosition());
        if (result_peg_106.error()){
            goto out_peg_107;
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = result_peg_106;
        stream.update(result_peg_106.getPosition());
        
        
        return result_peg_106;
        out_peg_107:
    
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_resource__f = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk12 != 0 && column_peg_2.chunk12->chunk_helper.calculated()){
        return column_peg_2.chunk12->chunk_helper;
    }
    
    
    RuleTrace trace_peg_1(stream, "helper");
    int myposition = position;
    
    
    Value name;
        Value id;
        Value to;
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "parent"));
                    for (int i = 0; i < 6; i++){
                        if (compareCharCase("parent"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_6);
                            goto out_peg_7;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_7:
                {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "root"));
                    for (int i = 0; i < 4; i++){
                        if (compareCharCase("root"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_8);
                            goto out_peg_9;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_9:
                {
                    int position_peg_10 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "helper"));
                    for (int i = 0; i < 6; i++){
                        if (compareCharCase("helper"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_10);
                            goto out_peg_11;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_11:
                {
                    int position_peg_12 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "target"));
                    for (int i = 0; i < 6; i++){
                        if (compareCharCase("target"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_12);
                            goto out_peg_13;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_13:
                {
                    int position_peg_14 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "partner"));
                    for (int i = 0; i < 7; i++){
                        if (compareCharCase("partner"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_14);
                            goto out_peg_15;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_15:
                {
                    int position_peg_16 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "enemyNear"));
                    for (int i = 0; i < 9; i++){
                        if (compareCharCase("enemyNear"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_16);
                            goto out_peg_17;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_17:
                {
                    int position_peg_18 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "enemy"));
                    for (int i = 0; i < 5; i++){
                        if (compareCharCase("enemy"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_18);
                            goto out_peg_19;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_19:
                {
                    int position_peg_20 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "playerid"));
                    for (int i = 0; i < 8; i++){
                        if (compareCharCase("playerid"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_20);
                            goto out_peg_21;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_21:
                goto out_peg_22;
                success_peg_5:
                ;
                name = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_25 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_27(result_peg_3.getPosition());
                        {
                            int position_peg_29 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_29);
                                    goto out_peg_30;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_30:
                        {
                            int position_peg_31 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_31);
                                    goto out_peg_32;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_32:
                        goto loop_peg_26;
                        success_peg_28:
                        ;
                        result_peg_3.addResult(result_peg_27);
                    } while (true);
                    loop_peg_26:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_22;
                success_peg_24:
                ;
            
            
            
            int save_peg_34 = result_peg_3.getPosition();
                
                result_peg_3 = rule_helper__expression(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    
                    result_peg_3 = Result(save_peg_34);
                    result_peg_3.setValue(Value((void*) 0));
                    
                }
                id = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_37 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_39(result_peg_3.getPosition());
                        {
                            int position_peg_41 = result_peg_39.getPosition();
                            
                            result_peg_39.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    result_peg_39.setPosition(position_peg_41);
                                    goto out_peg_42;
                                }
                            }
                                
                        }
                        goto success_peg_40;
                        out_peg_42:
                        {
                            int position_peg_43 = result_peg_39.getPosition();
                            
                            result_peg_39.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    result_peg_39.setPosition(position_peg_43);
                                    goto out_peg_44;
                                }
                            }
                                
                        }
                        goto success_peg_40;
                        out_peg_44:
                        goto loop_peg_38;
                        success_peg_40:
                        ;
                        result_peg_3.addResult(result_peg_39);
                    } while (true);
                    loop_peg_38:
                    ;
                            
                }
                goto success_peg_36;
                goto out_peg_22;
                success_peg_36:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_22;
                    }
                }
            
            
            
            {
                    int position_peg_48 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_50(result_peg_3.getPosition());
                        {
                            int position_peg_52 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_52);
                                    goto out_peg_53;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_53:
                        {
                            int position_peg_54 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_54);
                                    goto out_peg_55;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_55:
                        goto loop_peg_49;
                        success_peg_51:
                        ;
                        result_peg_3.addResult(result_peg_50);
                    } while (true);
                    loop_peg_49:
                    ;
                            
                }
                goto success_peg_47;
                goto out_peg_22;
                success_peg_47:
                ;
            
            
            
            result_peg_3 = rule_helper__identifier(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_22;
                }
                to = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHelper(name, id, to);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_22:
    
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk12 != 0 && column_peg_2.chunk12->chunk_helper__expression.calculated()){
        return column_peg_2.chunk12->chunk_helper__expression;
    }
    
    
    RuleTrace trace_peg_1(stream, "helper__expression");
    int myposition = position;
    
    
    Value use;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3 = rule_expr_c(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                use = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_5;
                success_peg_18:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = use;
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper__expression = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper__expression = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk12 != 0 && column_peg_2.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_2.chunk12->chunk_helper__identifier;
    }
    
    
    RuleTrace trace_peg_1(stream, "helper__identifier");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3 = rule_function(stream, result_peg_3.getPosition());
        if (result_peg_3.error()){
            goto out_peg_4;
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper__identifier = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_4:
        Result result_peg_5(myposition);
        
        
        result_peg_5 = rule_keyword(stream, result_peg_5.getPosition());
        if (result_peg_5.error()){
            goto out_peg_6;
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper__identifier = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_6:
        Result result_peg_7(myposition);
        
        
        {
        
            Result result_peg_10(result_peg_7);
                result_peg_10 = rule_keyword(stream, result_peg_10.getPosition());
                if (result_peg_10.error()){
                    goto not_peg_9;
                }
                goto out_peg_11;
                not_peg_9:
                result_peg_7.setValue(Value((void*)0));
            
            
            
            result_peg_7 = rule_identifier(stream, result_peg_7.getPosition());
                if (result_peg_7.error()){
                    goto out_peg_11;
                }
            
            
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper__identifier = result_peg_7;
        stream.update(result_peg_7.getPosition());
        
        
        return result_peg_7;
        out_peg_11:
    
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_helper__identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk12 != 0 && column_peg_2.chunk12->chunk_hitflag.calculated()){
        return column_peg_2.chunk12->chunk_hitflag;
    }
    
    
    RuleTrace trace_peg_1(stream, "hitflag");
    int myposition = position;
    
    
    Value chars;
        Value modifier;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.reset();
                do{
                    Result result_peg_6(result_peg_3.getPosition());
                    {
                        int position_peg_8 = result_peg_6.getPosition();
                        
                        char letter_peg_9 = stream.get(result_peg_6.getPosition());
                        if (letter_peg_9 != '\0' && strchr("HAMFDL", letter_peg_9) != NULL){
                            result_peg_6.nextPosition();
                            result_peg_6.setValue(Value((void*) (intptr_t) letter_peg_9));
                        } else {
                            result_peg_6.setPosition(position_peg_8);
                            goto out_peg_10;
                        }
                        
                    }
                    goto success_peg_7;
                    out_peg_10:
                    goto loop_peg_5;
                    success_peg_7:
                    ;
                    result_peg_3.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                if (result_peg_3.matches() == 0){
                    goto out_peg_11;
                }
                chars = result_peg_3.getValues();
            
            
            
            int save_peg_13 = result_peg_3.getPosition();
                
                {
                    int position_peg_15 = result_peg_3.getPosition();
                    
                    char letter_peg_16 = stream.get(result_peg_3.getPosition());
                    if (letter_peg_16 != '\0' && strchr("+-", letter_peg_16) != NULL){
                        result_peg_3.nextPosition();
                        result_peg_3.setValue(Value((void*) (intptr_t) letter_peg_16));
                    } else {
                        result_peg_3.setPosition(position_peg_15);
                        goto out_peg_17;
                    }
                    
                }
                goto success_peg_14;
                out_peg_17:
                
                result_peg_3 = Result(save_peg_13);
                result_peg_3.setValue(Value((void*) 0));
                
                success_peg_14:
                ;
                modifier = result_peg_3.getValues();
            
            
            
            Result result_peg_20(result_peg_3);
                result_peg_20.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_20.getPosition()))){
                        result_peg_20.nextPosition();
                    } else {
                        goto not_peg_19;
                    }
                }
                goto out_peg_11;
                not_peg_19:
                result_peg_3.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHitFlags(chars, modifier);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_hitflag = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_11:
    
        if (column_peg_2.chunk12 == 0){
            column_peg_2.chunk12 = new Chunk12();
        }
        column_peg_2.chunk12->chunk_hitflag = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk13 != 0 && column_peg_2.chunk13->chunk_keyword.calculated()){
        return column_peg_2.chunk13->chunk_keyword;
    }
    
    
    RuleTrace trace_peg_1(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_keyword_real(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            Result result_peg_4 = result_peg_3;
            
            Result result_peg_8(result_peg_3);
                {
                    int position_peg_10 = result_peg_8.getPosition();
                    
                    {
                        int position_peg_12 = result_peg_8.getPosition();
                        
                        char letter_peg_13 = stream.get(result_peg_8.getPosition());
                        if (letter_peg_13 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_13) != NULL){
                            result_peg_8.nextPosition();
                            result_peg_8.setValue(Value((void*) (intptr_t) letter_peg_13));
                        } else {
                            result_peg_8.setPosition(position_peg_12);
                            goto out_peg_14;
                        }
                        
                    }
                    goto success_peg_11;
                    out_peg_14:
                    result_peg_8.setPosition(position_peg_10);
                    goto out_peg_15;
                    success_peg_11:
                    ;
                    
                }
                goto success_peg_9;
                out_peg_15:
                {
                    int position_peg_16 = result_peg_8.getPosition();
                    
                    {
                        int position_peg_18 = result_peg_8.getPosition();
                        
                        char letter_peg_19 = stream.get(result_peg_8.getPosition());
                        if (letter_peg_19 != '\0' && strchr("0123456789", letter_peg_19) != NULL){
                            result_peg_8.nextPosition();
                            result_peg_8.setValue(Value((void*) (intptr_t) letter_peg_19));
                        } else {
                            result_peg_8.setPosition(position_peg_18);
                            goto out_peg_20;
                        }
                        
                    }
                    goto success_peg_17;
                    out_peg_20:
                    result_peg_8.setPosition(position_peg_16);
                    goto out_peg_21;
                    success_peg_17:
                    ;
                    
                }
                goto success_peg_9;
                out_peg_21:
                goto not_peg_7;
                success_peg_9:
                ;
                goto out_peg_5;
                not_peg_7:
                result_peg_3.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_4.getValues();
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk13 != 0 && column_peg_2.chunk13->chunk_keyword_real.calculated()){
        return column_peg_2.chunk13->chunk_keyword_real;
    }
    
    
    RuleTrace trace_peg_1(stream, "keyword_real");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "vel"));
                for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    if (result_peg_3.matches() == 0){
                        result_peg_3.setPosition(position_peg_8);
                        goto out_peg_16;
                    }
                    
                }
                goto success_peg_7;
                out_peg_16:
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel y");
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_18(myposition);
        
        
        {
        
            result_peg_18.setValue(Value((void*) "vel"));
                for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_18.getPosition()))){
                        result_peg_18.nextPosition();
                    } else {
                        goto out_peg_20;
                    }
                }
            
            
            
            {
                    int position_peg_23 = result_peg_18.getPosition();
                    
                    result_peg_18.reset();
                    do{
                        Result result_peg_25(result_peg_18.getPosition());
                        {
                            int position_peg_27 = result_peg_25.getPosition();
                            
                            result_peg_25.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_25.getPosition()))){
                                    result_peg_25.nextPosition();
                                } else {
                                    result_peg_25.setPosition(position_peg_27);
                                    goto out_peg_28;
                                }
                            }
                                
                        }
                        goto success_peg_26;
                        out_peg_28:
                        {
                            int position_peg_29 = result_peg_25.getPosition();
                            
                            result_peg_25.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_25.getPosition()))){
                                    result_peg_25.nextPosition();
                                } else {
                                    result_peg_25.setPosition(position_peg_29);
                                    goto out_peg_30;
                                }
                            }
                                
                        }
                        goto success_peg_26;
                        out_peg_30:
                        goto loop_peg_24;
                        success_peg_26:
                        ;
                        result_peg_18.addResult(result_peg_25);
                    } while (true);
                    loop_peg_24:
                    if (result_peg_18.matches() == 0){
                        result_peg_18.setPosition(position_peg_23);
                        goto out_peg_31;
                    }
                    
                }
                goto success_peg_22;
                out_peg_31:
                goto out_peg_20;
                success_peg_22:
                ;
            
            
            
            result_peg_18.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_18.getPosition()))){
                        result_peg_18.nextPosition();
                    } else {
                        goto out_peg_20;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel x");
                    result_peg_18.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_18;
        stream.update(result_peg_18.getPosition());
        
        
        return result_peg_18;
        out_peg_20:
        Result result_peg_33(myposition);
        
        
        {
        
            result_peg_33.setValue(Value((void*) "pos"));
                for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto out_peg_35;
                    }
                }
            
            
            
            {
                    int position_peg_38 = result_peg_33.getPosition();
                    
                    result_peg_33.reset();
                    do{
                        Result result_peg_40(result_peg_33.getPosition());
                        {
                            int position_peg_42 = result_peg_40.getPosition();
                            
                            result_peg_40.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    result_peg_40.setPosition(position_peg_42);
                                    goto out_peg_43;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_43:
                        {
                            int position_peg_44 = result_peg_40.getPosition();
                            
                            result_peg_40.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    result_peg_40.setPosition(position_peg_44);
                                    goto out_peg_45;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_45:
                        goto loop_peg_39;
                        success_peg_41:
                        ;
                        result_peg_33.addResult(result_peg_40);
                    } while (true);
                    loop_peg_39:
                    if (result_peg_33.matches() == 0){
                        result_peg_33.setPosition(position_peg_38);
                        goto out_peg_46;
                    }
                    
                }
                goto success_peg_37;
                out_peg_46:
                goto out_peg_35;
                success_peg_37:
                ;
            
            
            
            result_peg_33.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto out_peg_35;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos y");
                    result_peg_33.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_33;
        stream.update(result_peg_33.getPosition());
        
        
        return result_peg_33;
        out_peg_35:
        Result result_peg_48(myposition);
        
        
        {
        
            result_peg_48.setValue(Value((void*) "pos"));
                for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_48.getPosition()))){
                        result_peg_48.nextPosition();
                    } else {
                        goto out_peg_50;
                    }
                }
            
            
            
            {
                    int position_peg_53 = result_peg_48.getPosition();
                    
                    result_peg_48.reset();
                    do{
                        Result result_peg_55(result_peg_48.getPosition());
                        {
                            int position_peg_57 = result_peg_55.getPosition();
                            
                            result_peg_55.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    result_peg_55.setPosition(position_peg_57);
                                    goto out_peg_58;
                                }
                            }
                                
                        }
                        goto success_peg_56;
                        out_peg_58:
                        {
                            int position_peg_59 = result_peg_55.getPosition();
                            
                            result_peg_55.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    result_peg_55.setPosition(position_peg_59);
                                    goto out_peg_60;
                                }
                            }
                                
                        }
                        goto success_peg_56;
                        out_peg_60:
                        goto loop_peg_54;
                        success_peg_56:
                        ;
                        result_peg_48.addResult(result_peg_55);
                    } while (true);
                    loop_peg_54:
                    if (result_peg_48.matches() == 0){
                        result_peg_48.setPosition(position_peg_53);
                        goto out_peg_61;
                    }
                    
                }
                goto success_peg_52;
                out_peg_61:
                goto out_peg_50;
                success_peg_52:
                ;
            
            
            
            result_peg_48.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_48.getPosition()))){
                        result_peg_48.nextPosition();
                    } else {
                        goto out_peg_50;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos x");
                    result_peg_48.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_48;
        stream.update(result_peg_48.getPosition());
        
        
        return result_peg_48;
        out_peg_50:
        Result result_peg_63(myposition);
        
        
        {
        
            result_peg_63.setValue(Value((void*) "p2dist"));
                for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_63.getPosition()))){
                        result_peg_63.nextPosition();
                    } else {
                        goto out_peg_65;
                    }
                }
            
            
            
            {
                    int position_peg_68 = result_peg_63.getPosition();
                    
                    result_peg_63.reset();
                    do{
                        Result result_peg_70(result_peg_63.getPosition());
                        {
                            int position_peg_72 = result_peg_70.getPosition();
                            
                            result_peg_70.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_70.getPosition()))){
                                    result_peg_70.nextPosition();
                                } else {
                                    result_peg_70.setPosition(position_peg_72);
                                    goto out_peg_73;
                                }
                            }
                                
                        }
                        goto success_peg_71;
                        out_peg_73:
                        {
                            int position_peg_74 = result_peg_70.getPosition();
                            
                            result_peg_70.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_70.getPosition()))){
                                    result_peg_70.nextPosition();
                                } else {
                                    result_peg_70.setPosition(position_peg_74);
                                    goto out_peg_75;
                                }
                            }
                                
                        }
                        goto success_peg_71;
                        out_peg_75:
                        goto loop_peg_69;
                        success_peg_71:
                        ;
                        result_peg_63.addResult(result_peg_70);
                    } while (true);
                    loop_peg_69:
                    if (result_peg_63.matches() == 0){
                        result_peg_63.setPosition(position_peg_68);
                        goto out_peg_76;
                    }
                    
                }
                goto success_peg_67;
                out_peg_76:
                goto out_peg_65;
                success_peg_67:
                ;
            
            
            
            result_peg_63.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_63.getPosition()))){
                        result_peg_63.nextPosition();
                    } else {
                        goto out_peg_65;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist x");
                    result_peg_63.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_63;
        stream.update(result_peg_63.getPosition());
        
        
        return result_peg_63;
        out_peg_65:
        Result result_peg_78(myposition);
        
        
        {
        
            result_peg_78.setValue(Value((void*) "p2dist"));
                for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_78.getPosition()))){
                        result_peg_78.nextPosition();
                    } else {
                        goto out_peg_80;
                    }
                }
            
            
            
            {
                    int position_peg_83 = result_peg_78.getPosition();
                    
                    result_peg_78.reset();
                    do{
                        Result result_peg_85(result_peg_78.getPosition());
                        {
                            int position_peg_87 = result_peg_85.getPosition();
                            
                            result_peg_85.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_85.getPosition()))){
                                    result_peg_85.nextPosition();
                                } else {
                                    result_peg_85.setPosition(position_peg_87);
                                    goto out_peg_88;
                                }
                            }
                                
                        }
                        goto success_peg_86;
                        out_peg_88:
                        {
                            int position_peg_89 = result_peg_85.getPosition();
                            
                            result_peg_85.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_85.getPosition()))){
                                    result_peg_85.nextPosition();
                                } else {
                                    result_peg_85.setPosition(position_peg_89);
                                    goto out_peg_90;
                                }
                            }
                                
                        }
                        goto success_peg_86;
                        out_peg_90:
                        goto loop_peg_84;
                        success_peg_86:
                        ;
                        result_peg_78.addResult(result_peg_85);
                    } while (true);
                    loop_peg_84:
                    if (result_peg_78.matches() == 0){
                        result_peg_78.setPosition(position_peg_83);
                        goto out_peg_91;
                    }
                    
                }
                goto success_peg_82;
                out_peg_91:
                goto out_peg_80;
                success_peg_82:
                ;
            
            
            
            result_peg_78.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_78.getPosition()))){
                        result_peg_78.nextPosition();
                    } else {
                        goto out_peg_80;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist y");
                    result_peg_78.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_78;
        stream.update(result_peg_78.getPosition());
        
        
        return result_peg_78;
        out_peg_80:
        Result result_peg_93(myposition);
        
        
        {
        
            result_peg_93.setValue(Value((void*) "p1dist"));
                for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_93.getPosition()))){
                        result_peg_93.nextPosition();
                    } else {
                        goto out_peg_95;
                    }
                }
            
            
            
            {
                    int position_peg_98 = result_peg_93.getPosition();
                    
                    result_peg_93.reset();
                    do{
                        Result result_peg_100(result_peg_93.getPosition());
                        {
                            int position_peg_102 = result_peg_100.getPosition();
                            
                            result_peg_100.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_100.getPosition()))){
                                    result_peg_100.nextPosition();
                                } else {
                                    result_peg_100.setPosition(position_peg_102);
                                    goto out_peg_103;
                                }
                            }
                                
                        }
                        goto success_peg_101;
                        out_peg_103:
                        {
                            int position_peg_104 = result_peg_100.getPosition();
                            
                            result_peg_100.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_100.getPosition()))){
                                    result_peg_100.nextPosition();
                                } else {
                                    result_peg_100.setPosition(position_peg_104);
                                    goto out_peg_105;
                                }
                            }
                                
                        }
                        goto success_peg_101;
                        out_peg_105:
                        goto loop_peg_99;
                        success_peg_101:
                        ;
                        result_peg_93.addResult(result_peg_100);
                    } while (true);
                    loop_peg_99:
                    if (result_peg_93.matches() == 0){
                        result_peg_93.setPosition(position_peg_98);
                        goto out_peg_106;
                    }
                    
                }
                goto success_peg_97;
                out_peg_106:
                goto out_peg_95;
                success_peg_97:
                ;
            
            
            
            result_peg_93.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_93.getPosition()))){
                        result_peg_93.nextPosition();
                    } else {
                        goto out_peg_95;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist x");
                    result_peg_93.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_93;
        stream.update(result_peg_93.getPosition());
        
        
        return result_peg_93;
        out_peg_95:
        Result result_peg_108(myposition);
        
        
        {
        
            result_peg_108.setValue(Value((void*) "p1dist"));
                for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_108.getPosition()))){
                        result_peg_108.nextPosition();
                    } else {
                        goto out_peg_110;
                    }
                }
            
            
            
            {
                    int position_peg_113 = result_peg_108.getPosition();
                    
                    result_peg_108.reset();
                    do{
                        Result result_peg_115(result_peg_108.getPosition());
                        {
                            int position_peg_117 = result_peg_115.getPosition();
                            
                            result_peg_115.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_115.getPosition()))){
                                    result_peg_115.nextPosition();
                                } else {
                                    result_peg_115.setPosition(position_peg_117);
                                    goto out_peg_118;
                                }
                            }
                                
                        }
                        goto success_peg_116;
                        out_peg_118:
                        {
                            int position_peg_119 = result_peg_115.getPosition();
                            
                            result_peg_115.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_115.getPosition()))){
                                    result_peg_115.nextPosition();
                                } else {
                                    result_peg_115.setPosition(position_peg_119);
                                    goto out_peg_120;
                                }
                            }
                                
                        }
                        goto success_peg_116;
                        out_peg_120:
                        goto loop_peg_114;
                        success_peg_116:
                        ;
                        result_peg_108.addResult(result_peg_115);
                    } while (true);
                    loop_peg_114:
                    if (result_peg_108.matches() == 0){
                        result_peg_108.setPosition(position_peg_113);
                        goto out_peg_121;
                    }
                    
                }
                goto success_peg_112;
                out_peg_121:
                goto out_peg_110;
                success_peg_112:
                ;
            
            
            
            result_peg_108.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_108.getPosition()))){
                        result_peg_108.nextPosition();
                    } else {
                        goto out_peg_110;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist y");
                    result_peg_108.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_108;
        stream.update(result_peg_108.getPosition());
        
        
        return result_peg_108;
        out_peg_110:
        Result result_peg_123(myposition);
        
        
        {
        
            result_peg_123.setValue(Value((void*) "p2bodydist"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_123.getPosition()))){
                        result_peg_123.nextPosition();
                    } else {
                        goto out_peg_125;
                    }
                }
            
            
            
            {
                    int position_peg_128 = result_peg_123.getPosition();
                    
                    result_peg_123.reset();
                    do{
                        Result result_peg_130(result_peg_123.getPosition());
                        {
                            int position_peg_132 = result_peg_130.getPosition();
                            
                            result_peg_130.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_130.getPosition()))){
                                    result_peg_130.nextPosition();
                                } else {
                                    result_peg_130.setPosition(position_peg_132);
                                    goto out_peg_133;
                                }
                            }
                                
                        }
                        goto success_peg_131;
                        out_peg_133:
                        {
                            int position_peg_134 = result_peg_130.getPosition();
                            
                            result_peg_130.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_130.getPosition()))){
                                    result_peg_130.nextPosition();
                                } else {
                                    result_peg_130.setPosition(position_peg_134);
                                    goto out_peg_135;
                                }
                            }
                                
                        }
                        goto success_peg_131;
                        out_peg_135:
                        goto loop_peg_129;
                        success_peg_131:
                        ;
                        result_peg_123.addResult(result_peg_130);
                    } while (true);
                    loop_peg_129:
                    if (result_peg_123.matches() == 0){
                        result_peg_123.setPosition(position_peg_128);
                        goto out_peg_136;
                    }
                    
                }
                goto success_peg_127;
                out_peg_136:
                goto out_peg_125;
                success_peg_127:
                ;
            
            
            
            result_peg_123.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_123.getPosition()))){
                        result_peg_123.nextPosition();
                    } else {
                        goto out_peg_125;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist x");
                    result_peg_123.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_123;
        stream.update(result_peg_123.getPosition());
        
        
        return result_peg_123;
        out_peg_125:
        Result result_peg_138(myposition);
        
        
        {
        
            result_peg_138.setValue(Value((void*) "p2bodydist"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_138.getPosition()))){
                        result_peg_138.nextPosition();
                    } else {
                        goto out_peg_140;
                    }
                }
            
            
            
            {
                    int position_peg_143 = result_peg_138.getPosition();
                    
                    result_peg_138.reset();
                    do{
                        Result result_peg_145(result_peg_138.getPosition());
                        {
                            int position_peg_147 = result_peg_145.getPosition();
                            
                            result_peg_145.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_145.getPosition()))){
                                    result_peg_145.nextPosition();
                                } else {
                                    result_peg_145.setPosition(position_peg_147);
                                    goto out_peg_148;
                                }
                            }
                                
                        }
                        goto success_peg_146;
                        out_peg_148:
                        {
                            int position_peg_149 = result_peg_145.getPosition();
                            
                            result_peg_145.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_145.getPosition()))){
                                    result_peg_145.nextPosition();
                                } else {
                                    result_peg_145.setPosition(position_peg_149);
                                    goto out_peg_150;
                                }
                            }
                                
                        }
                        goto success_peg_146;
                        out_peg_150:
                        goto loop_peg_144;
                        success_peg_146:
                        ;
                        result_peg_138.addResult(result_peg_145);
                    } while (true);
                    loop_peg_144:
                    if (result_peg_138.matches() == 0){
                        result_peg_138.setPosition(position_peg_143);
                        goto out_peg_151;
                    }
                    
                }
                goto success_peg_142;
                out_peg_151:
                goto out_peg_140;
                success_peg_142:
                ;
            
            
            
            result_peg_138.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_138.getPosition()))){
                        result_peg_138.nextPosition();
                    } else {
                        goto out_peg_140;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist y");
                    result_peg_138.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_138;
        stream.update(result_peg_138.getPosition());
        
        
        return result_peg_138;
        out_peg_140:
        Result result_peg_153(myposition);
        
        
        {
        
            result_peg_153.setValue(Value((void*) "p1bodydist"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_153.getPosition()))){
                        result_peg_153.nextPosition();
                    } else {
                        goto out_peg_155;
                    }
                }
            
            
            
            {
                    int position_peg_158 = result_peg_153.getPosition();
                    
                    result_peg_153.reset();
                    do{
                        Result result_peg_160(result_peg_153.getPosition());
                        {
                            int position_peg_162 = result_peg_160.getPosition();
                            
                            result_peg_160.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_160.getPosition()))){
                                    result_peg_160.nextPosition();
                                } else {
                                    result_peg_160.setPosition(position_peg_162);
                                    goto out_peg_163;
                                }
                            }
                                
                        }
                        goto success_peg_161;
                        out_peg_163:
                        {
                            int position_peg_164 = result_peg_160.getPosition();
                            
                            result_peg_160.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_160.getPosition()))){
                                    result_peg_160.nextPosition();
                                } else {
                                    result_peg_160.setPosition(position_peg_164);
                                    goto out_peg_165;
                                }
                            }
                                
                        }
                        goto success_peg_161;
                        out_peg_165:
                        goto loop_peg_159;
                        success_peg_161:
                        ;
                        result_peg_153.addResult(result_peg_160);
                    } while (true);
                    loop_peg_159:
                    if (result_peg_153.matches() == 0){
                        result_peg_153.setPosition(position_peg_158);
                        goto out_peg_166;
                    }
                    
                }
                goto success_peg_157;
                out_peg_166:
                goto out_peg_155;
                success_peg_157:
                ;
            
            
            
            result_peg_153.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_153.getPosition()))){
                        result_peg_153.nextPosition();
                    } else {
                        goto out_peg_155;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist x");
                    result_peg_153.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_153;
        stream.update(result_peg_153.getPosition());
        
        
        return result_peg_153;
        out_peg_155:
        Result result_peg_168(myposition);
        
        
        {
        
            result_peg_168.setValue(Value((void*) "p1bodydist"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_168.getPosition()))){
                        result_peg_168.nextPosition();
                    } else {
                        goto out_peg_170;
                    }
                }
            
            
            
            {
                    int position_peg_173 = result_peg_168.getPosition();
                    
                    result_peg_168.reset();
                    do{
                        Result result_peg_175(result_peg_168.getPosition());
                        {
                            int position_peg_177 = result_peg_175.getPosition();
                            
                            result_peg_175.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_175.getPosition()))){
                                    result_peg_175.nextPosition();
                                } else {
                                    result_peg_175.setPosition(position_peg_177);
                                    goto out_peg_178;
                                }
                            }
                                
                        }
                        goto success_peg_176;
                        out_peg_178:
                        {
                            int position_peg_179 = result_peg_175.getPosition();
                            
                            result_peg_175.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_175.getPosition()))){
                                    result_peg_175.nextPosition();
                                } else {
                                    result_peg_175.setPosition(position_peg_179);
                                    goto out_peg_180;
                                }
                            }
                                
                        }
                        goto success_peg_176;
                        out_peg_180:
                        goto loop_peg_174;
                        success_peg_176:
                        ;
                        result_peg_168.addResult(result_peg_175);
                    } while (true);
                    loop_peg_174:
                    if (result_peg_168.matches() == 0){
                        result_peg_168.setPosition(position_peg_173);
                        goto out_peg_181;
                    }
                    
                }
                goto success_peg_172;
                out_peg_181:
                goto out_peg_170;
                success_peg_172:
                ;
            
            
            
            result_peg_168.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_168.getPosition()))){
                        result_peg_168.nextPosition();
                    } else {
                        goto out_peg_170;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist y");
                    result_peg_168.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_168;
        stream.update(result_peg_168.getPosition());
        
        
        return result_peg_168;
        out_peg_170:
        Result result_peg_183(myposition);
        
        
        {
        
            result_peg_183.setValue(Value((void*) "parentdist"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_183.getPosition()))){
                        result_peg_183.nextPosition();
                    } else {
                        goto out_peg_185;
                    }
                }
            
            
            
            {
                    int position_peg_188 = result_peg_183.getPosition();
                    
                    result_peg_183.reset();
                    do{
                        Result result_peg_190(result_peg_183.getPosition());
                        {
                            int position_peg_192 = result_peg_190.getPosition();
                            
                            result_peg_190.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_190.getPosition()))){
                                    result_peg_190.nextPosition();
                                } else {
                                    result_peg_190.setPosition(position_peg_192);
                                    goto out_peg_193;
                                }
                            }
                                
                        }
                        goto success_peg_191;
                        out_peg_193:
                        {
                            int position_peg_194 = result_peg_190.getPosition();
                            
                            result_peg_190.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_190.getPosition()))){
                                    result_peg_190.nextPosition();
                                } else {
                                    result_peg_190.setPosition(position_peg_194);
                                    goto out_peg_195;
                                }
                            }
                                
                        }
                        goto success_peg_191;
                        out_peg_195:
                        goto loop_peg_189;
                        success_peg_191:
                        ;
                        result_peg_183.addResult(result_peg_190);
                    } while (true);
                    loop_peg_189:
                    if (result_peg_183.matches() == 0){
                        result_peg_183.setPosition(position_peg_188);
                        goto out_peg_196;
                    }
                    
                }
                goto success_peg_187;
                out_peg_196:
                goto out_peg_185;
                success_peg_187:
                ;
            
            
            
            result_peg_183.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_183.getPosition()))){
                        result_peg_183.nextPosition();
                    } else {
                        goto out_peg_185;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_183.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_183;
        stream.update(result_peg_183.getPosition());
        
        
        return result_peg_183;
        out_peg_185:
        Result result_peg_198(myposition);
        
        
        {
        
            result_peg_198.setValue(Value((void*) "screenpos"));
                for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_198.getPosition()))){
                        result_peg_198.nextPosition();
                    } else {
                        goto out_peg_200;
                    }
                }
            
            
            
            {
                    int position_peg_203 = result_peg_198.getPosition();
                    
                    result_peg_198.reset();
                    do{
                        Result result_peg_205(result_peg_198.getPosition());
                        {
                            int position_peg_207 = result_peg_205.getPosition();
                            
                            result_peg_205.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_205.getPosition()))){
                                    result_peg_205.nextPosition();
                                } else {
                                    result_peg_205.setPosition(position_peg_207);
                                    goto out_peg_208;
                                }
                            }
                                
                        }
                        goto success_peg_206;
                        out_peg_208:
                        {
                            int position_peg_209 = result_peg_205.getPosition();
                            
                            result_peg_205.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_205.getPosition()))){
                                    result_peg_205.nextPosition();
                                } else {
                                    result_peg_205.setPosition(position_peg_209);
                                    goto out_peg_210;
                                }
                            }
                                
                        }
                        goto success_peg_206;
                        out_peg_210:
                        goto loop_peg_204;
                        success_peg_206:
                        ;
                        result_peg_198.addResult(result_peg_205);
                    } while (true);
                    loop_peg_204:
                    if (result_peg_198.matches() == 0){
                        result_peg_198.setPosition(position_peg_203);
                        goto out_peg_211;
                    }
                    
                }
                goto success_peg_202;
                out_peg_211:
                goto out_peg_200;
                success_peg_202:
                ;
            
            
            
            result_peg_198.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_198.getPosition()))){
                        result_peg_198.nextPosition();
                    } else {
                        goto out_peg_200;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos x");
                    result_peg_198.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_198;
        stream.update(result_peg_198.getPosition());
        
        
        return result_peg_198;
        out_peg_200:
        Result result_peg_213(myposition);
        
        
        {
        
            result_peg_213.setValue(Value((void*) "screenpos"));
                for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_213.getPosition()))){
                        result_peg_213.nextPosition();
                    } else {
                        goto out_peg_215;
                    }
                }
            
            
            
            {
                    int position_peg_218 = result_peg_213.getPosition();
                    
                    result_peg_213.reset();
                    do{
                        Result result_peg_220(result_peg_213.getPosition());
                        {
                            int position_peg_222 = result_peg_220.getPosition();
                            
                            result_peg_220.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    result_peg_220.setPosition(position_peg_222);
                                    goto out_peg_223;
                                }
                            }
                                
                        }
                        goto success_peg_221;
                        out_peg_223:
                        {
                            int position_peg_224 = result_peg_220.getPosition();
                            
                            result_peg_220.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    result_peg_220.setPosition(position_peg_224);
                                    goto out_peg_225;
                                }
                            }
                                
                        }
                        goto success_peg_221;
                        out_peg_225:
                        goto loop_peg_219;
                        success_peg_221:
                        ;
                        result_peg_213.addResult(result_peg_220);
                    } while (true);
                    loop_peg_219:
                    if (result_peg_213.matches() == 0){
                        result_peg_213.setPosition(position_peg_218);
                        goto out_peg_226;
                    }
                    
                }
                goto success_peg_217;
                out_peg_226:
                goto out_peg_215;
                success_peg_217:
                ;
            
            
            
            result_peg_213.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_213.getPosition()))){
                        result_peg_213.nextPosition();
                    } else {
                        goto out_peg_215;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos y");
                    result_peg_213.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_213;
        stream.update(result_peg_213.getPosition());
        
        
        return result_peg_213;
        out_peg_215:
        Result result_peg_228(myposition);
        
        
        {
        
            result_peg_228.setValue(Value((void*) "parentdist"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_228.getPosition()))){
                        result_peg_228.nextPosition();
                    } else {
                        goto out_peg_230;
                    }
                }
            
            
            
            {
                    int position_peg_233 = result_peg_228.getPosition();
                    
                    result_peg_228.reset();
                    do{
                        Result result_peg_235(result_peg_228.getPosition());
                        {
                            int position_peg_237 = result_peg_235.getPosition();
                            
                            result_peg_235.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_235.getPosition()))){
                                    result_peg_235.nextPosition();
                                } else {
                                    result_peg_235.setPosition(position_peg_237);
                                    goto out_peg_238;
                                }
                            }
                                
                        }
                        goto success_peg_236;
                        out_peg_238:
                        {
                            int position_peg_239 = result_peg_235.getPosition();
                            
                            result_peg_235.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_235.getPosition()))){
                                    result_peg_235.nextPosition();
                                } else {
                                    result_peg_235.setPosition(position_peg_239);
                                    goto out_peg_240;
                                }
                            }
                                
                        }
                        goto success_peg_236;
                        out_peg_240:
                        goto loop_peg_234;
                        success_peg_236:
                        ;
                        result_peg_228.addResult(result_peg_235);
                    } while (true);
                    loop_peg_234:
                    if (result_peg_228.matches() == 0){
                        result_peg_228.setPosition(position_peg_233);
                        goto out_peg_241;
                    }
                    
                }
                goto success_peg_232;
                out_peg_241:
                goto out_peg_230;
                success_peg_232:
                ;
            
            
            
            result_peg_228.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_228.getPosition()))){
                        result_peg_228.nextPosition();
                    } else {
                        goto out_peg_230;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_228.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_228;
        stream.update(result_peg_228.getPosition());
        
        
        return result_peg_228;
        out_peg_230:
        Result result_peg_243(myposition);
        
        
        {
        
            result_peg_243.setValue(Value((void*) "parentdist"));
                for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_243.getPosition()))){
                        result_peg_243.nextPosition();
                    } else {
                        goto out_peg_245;
                    }
                }
            
            
            
            {
                    int position_peg_248 = result_peg_243.getPosition();
                    
                    result_peg_243.reset();
                    do{
                        Result result_peg_250(result_peg_243.getPosition());
                        {
                            int position_peg_252 = result_peg_250.getPosition();
                            
                            result_peg_250.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_250.getPosition()))){
                                    result_peg_250.nextPosition();
                                } else {
                                    result_peg_250.setPosition(position_peg_252);
                                    goto out_peg_253;
                                }
                            }
                                
                        }
                        goto success_peg_251;
                        out_peg_253:
                        {
                            int position_peg_254 = result_peg_250.getPosition();
                            
                            result_peg_250.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_250.getPosition()))){
                                    result_peg_250.nextPosition();
                                } else {
                                    result_peg_250.setPosition(position_peg_254);
                                    goto out_peg_255;
                                }
                            }
                                
                        }
                        goto success_peg_251;
                        out_peg_255:
                        goto loop_peg_249;
                        success_peg_251:
                        ;
                        result_peg_243.addResult(result_peg_250);
                    } while (true);
                    loop_peg_249:
                    if (result_peg_243.matches() == 0){
                        result_peg_243.setPosition(position_peg_248);
                        goto out_peg_256;
                    }
                    
                }
                goto success_peg_247;
                out_peg_256:
                goto out_peg_245;
                success_peg_247:
                ;
            
            
            
            result_peg_243.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_243.getPosition()))){
                        result_peg_243.nextPosition();
                    } else {
                        goto out_peg_245;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist y");
                    result_peg_243.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_243;
        stream.update(result_peg_243.getPosition());
        
        
        return result_peg_243;
        out_peg_245:
        Result result_peg_258(myposition);
        
        
        {
        
            result_peg_258.setValue(Value((void*) "rootdist"));
                for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_258.getPosition()))){
                        result_peg_258.nextPosition();
                    } else {
                        goto out_peg_260;
                    }
                }
            
            
            
            {
                    int position_peg_263 = result_peg_258.getPosition();
                    
                    result_peg_258.reset();
                    do{
                        Result result_peg_265(result_peg_258.getPosition());
                        {
                            int position_peg_267 = result_peg_265.getPosition();
                            
                            result_peg_265.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_265.getPosition()))){
                                    result_peg_265.nextPosition();
                                } else {
                                    result_peg_265.setPosition(position_peg_267);
                                    goto out_peg_268;
                                }
                            }
                                
                        }
                        goto success_peg_266;
                        out_peg_268:
                        {
                            int position_peg_269 = result_peg_265.getPosition();
                            
                            result_peg_265.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_265.getPosition()))){
                                    result_peg_265.nextPosition();
                                } else {
                                    result_peg_265.setPosition(position_peg_269);
                                    goto out_peg_270;
                                }
                            }
                                
                        }
                        goto success_peg_266;
                        out_peg_270:
                        goto loop_peg_264;
                        success_peg_266:
                        ;
                        result_peg_258.addResult(result_peg_265);
                    } while (true);
                    loop_peg_264:
                    if (result_peg_258.matches() == 0){
                        result_peg_258.setPosition(position_peg_263);
                        goto out_peg_271;
                    }
                    
                }
                goto success_peg_262;
                out_peg_271:
                goto out_peg_260;
                success_peg_262:
                ;
            
            
            
            result_peg_258.setValue(Value((void*) "x"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_258.getPosition()))){
                        result_peg_258.nextPosition();
                    } else {
                        goto out_peg_260;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist x");
                    result_peg_258.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_258;
        stream.update(result_peg_258.getPosition());
        
        
        return result_peg_258;
        out_peg_260:
        Result result_peg_273(myposition);
        
        
        {
        
            result_peg_273.setValue(Value((void*) "rootdist"));
                for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_273.getPosition()))){
                        result_peg_273.nextPosition();
                    } else {
                        goto out_peg_275;
                    }
                }
            
            
            
            {
                    int position_peg_278 = result_peg_273.getPosition();
                    
                    result_peg_273.reset();
                    do{
                        Result result_peg_280(result_peg_273.getPosition());
                        {
                            int position_peg_282 = result_peg_280.getPosition();
                            
                            result_peg_280.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_280.getPosition()))){
                                    result_peg_280.nextPosition();
                                } else {
                                    result_peg_280.setPosition(position_peg_282);
                                    goto out_peg_283;
                                }
                            }
                                
                        }
                        goto success_peg_281;
                        out_peg_283:
                        {
                            int position_peg_284 = result_peg_280.getPosition();
                            
                            result_peg_280.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_280.getPosition()))){
                                    result_peg_280.nextPosition();
                                } else {
                                    result_peg_280.setPosition(position_peg_284);
                                    goto out_peg_285;
                                }
                            }
                                
                        }
                        goto success_peg_281;
                        out_peg_285:
                        goto loop_peg_279;
                        success_peg_281:
                        ;
                        result_peg_273.addResult(result_peg_280);
                    } while (true);
                    loop_peg_279:
                    if (result_peg_273.matches() == 0){
                        result_peg_273.setPosition(position_peg_278);
                        goto out_peg_286;
                    }
                    
                }
                goto success_peg_277;
                out_peg_286:
                goto out_peg_275;
                success_peg_277:
                ;
            
            
            
            result_peg_273.setValue(Value((void*) "y"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_273.getPosition()))){
                        result_peg_273.nextPosition();
                    } else {
                        goto out_peg_275;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist y");
                    result_peg_273.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_273;
        stream.update(result_peg_273.getPosition());
        
        
        return result_peg_273;
        out_peg_275:
        Result result_peg_288(myposition);
        
        
        result_peg_288 = rule_hitflag(stream, result_peg_288.getPosition());
        if (result_peg_288.error()){
            goto out_peg_289;
        }
        
        
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = result_peg_288;
        stream.update(result_peg_288.getPosition());
        
        
        return result_peg_288;
        out_peg_289:
    
        if (column_peg_2.chunk13 == 0){
            column_peg_2.chunk13 = new Chunk13();
        }
        column_peg_2.chunk13->chunk_keyword_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

static const void * doParse(Stream & stream, bool stats, const std::string & context){
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        stream.reportError(context);
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



    
    } /* Cmd */
    
} /* Mugen */

    
