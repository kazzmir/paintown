

#include "../ast/all.h"
#include <map>
typedef std::list<Ast::Section*> SectionList;


#include <list>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>


namespace Mugen{

    namespace Def{
    
    



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

    Result(const Result & r){
        position = r.position;
        value = r.value;
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
    Result chunk_s;
    Result chunk_section;
};

struct Chunk1{
    Result chunk_section_line;
    Result chunk_section_start;
    Result chunk_loopstart;
    Result chunk_name;
    Result chunk_alpha_digit;
};

struct Chunk2{
    Result chunk_line_end_or_comment;
    Result chunk_filename;
    Result chunk_filename_char;
    Result chunk_attribute;
    Result chunk_identifier;
};

struct Chunk3{
    Result chunk_identifier_list;
    Result chunk_valuelist;
    Result chunk_value;
    Result chunk_keyword;
    Result chunk_date;
};

struct Chunk4{
    Result chunk_string;
    Result chunk_number;
    Result chunk_float_or_integer;
};

struct Column{
    Column():
        chunk0(0)
        ,chunk1(0)
        ,chunk2(0)
        ,chunk3(0)
        ,chunk4(0){
    }

    Chunk0 * chunk0;
    Chunk1 * chunk1;
    Chunk2 * chunk2;
    Chunk3 * chunk3;
    Chunk4 * chunk4;

    ~Column(){
        delete chunk0;
        delete chunk1;
        delete chunk2;
        delete chunk3;
        delete chunk4;
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
        memo_size = 1024;
        memo = new Column*[memo_size];
        for (int i = 0; i < memo_size; i++){
            memo[i] = new Column();
        }
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
        if (position >= memo_size){
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

    std::string getReason() const {
        return message;
    }

    virtual ~ParseException() throw(){
    }

protected:
    std::string message;
};

Result errorResult(-1);


Result rule_start(Stream &, const int);
Result rule_line(Stream &, const int, Value current);
Result rule_line_end(Stream &, const int);
Result rule_s(Stream &, const int);
Result rule_section(Stream &, const int);
Result rule_section_line(Stream &, const int, Value section);
Result rule_section_start(Stream &, const int);
Result rule_loopstart(Stream &, const int);
Result rule_name(Stream &, const int);
Result rule_alpha_digit(Stream &, const int);
Result rule_line_end_or_comment(Stream &, const int);
Result rule_filename(Stream &, const int);
Result rule_filename_char(Stream &, const int);
Result rule_attribute(Stream &, const int);
Result rule_identifier(Stream &, const int);
Result rule_identifier_list(Stream &, const int);
Result rule_valuelist(Stream &, const int);
Result rule_value(Stream &, const int);
Result rule_keyword(Stream &, const int);
Result rule_date(Stream &, const int);
Result rule_string(Stream &, const int);
Result rule_number(Stream &, const int);
Result rule_float_or_integer(Stream &, const int);



/* poor man's garbage collection.
 * after parsing every pointer stored in this list is checked to see if it
 * is stored in the resulting AST. If the AST does not know about the pointer
 * then no one else does and the pointer should be deleted.
 *
 * void* doesn't work because it doesn't call the destructor. instead use
 * a new class, Collectable, with constructors and fields for every class
 * that is allocated. Collectable will call the appropriate destructor.
 */
static std::list<Ast::Collectable> saved_pointers;

template<class X>
X as(const Value & value){
    return (X) value.getValue();
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
  return new Ast::Section(as<std::string*>(str));
}

template<class X>
void save(const X x){
    saved_pointers.push_back(Ast::Collectable(x));
}

SectionList * makeSectionList(){
  return new SectionList();
}

std::string * toString(const Value & input){
  std::ostringstream out;
  for (Value::iterator it = input.getValues().begin(); it != input.getValues().end(); it++){
    out << (char) (long) (*it).getValue();
  }
  return new std::string(out.str());
}

double * parseDouble(const Value & value){
    std::string * str = toString(value);
    std::istringstream get(*str);
    double * number = new double;
    get >> *number;
    delete str;
    return number;
}

double * parseDouble(const Value & left, const Value & right){
    std::string * str1 = toString(left);
    std::string * str2 = toString(right);
    std::istringstream get(*str1 + "." + *str2);
    double * number = new double;
    get >> *number;
    delete str1;
    delete str2;
    return number;
}

std::string * toString(char front, const Value & input){
  std::string * str = toString(input);
  str->insert(str->begin(), front);
  return str;
}

Ast::Attribute * makeAttribute(const Value & id, const Value & data){
    return new Ast::AttributeSimple(as<Ast::Identifier*>(id), as<Ast::Value*>(data));
}

Ast::Attribute * makeAttribute(const Value & id){
    return new Ast::AttributeSimple(as<Ast::Identifier*>(id));
}

Ast::Attribute * makeAttributeFilename(const Value & id, const Value & data){
    return makeAttribute(id, data);
}

Ast::Attribute * makeIndexedAttribute(const Value & id, const Value & index, const Value & data){
    /* wrong, fix this */
    return new Ast::Attribute(Ast::Attribute::None);
}

Ast::Keyword * makeKeyword(const Value & value){
    return new Ast::Keyword(as<char*>(value));
}

Ast::Attribute * makeAttributes(const Value & id, const Value & data){
    /* wrong, fix this */
    return new Ast::Attribute(Ast::Attribute::None);
}

/* FIXME */
Ast::Value * makeValue(){
    return new Ast::Number(0);
}

Ast::Value * makeValueList(const Value & front, const Value & rest){
    std::list<Ast::Value*> values;
    values.push_back(as<Ast::Value*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        Ast::Value * value = as<Ast::Value*>((*it).getValue());
        if (value == 0){
            /* FIXME! replace empty with a new node */
            value = makeKeyword("empty");
            save(value);
            values.push_back(value);
        } else {
            values.push_back(value);
        }
    }
    return new Ast::ValueList(values);
}

Ast::Identifier * makeIdentifier(const Value & front, const Value & rest){
    std::list<std::string*> ids;
    ids.push_back(as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        ids.push_back(as<std::string*>((*it).getValue()));
    }
    return new Ast::Identifier(ids);
}

Ast::Value * makeNumber(const Value & sign, const Value & number){
    double value = *(as<double*>(number));
    if (sign.getValue() != 0){
        value = -value;
    }

    return new Ast::Number(value);
}

Ast::String * makeString(const Value & value){
    return new Ast::String(toString(value));
}

Ast::Section * asSection(const Value & value){
    return as<Ast::Section*>(value);
}

Ast::Attribute * asAttribute(const Value & value){
    return as<Ast::Attribute*>(value);
}

Ast::Value * asValue(const Value & value){
    return as<Ast::Value*>(value);
}

/* FIXME */
Ast::Value * makeDate(const Value & month, const Value & day, const Value & year){
    return new Ast::Number(0);
}

/* garbage collection */
void cleanup(const Value & value){
    SectionList * list = as<SectionList*>(value);

    /* pointers that we should keep will be marked true in the map */
    std::map<const void*, bool> marks;

    /* first mark roots */
    marks[list] = true;
    for (SectionList::iterator it = list->begin(); it != list->end(); it++){
        Ast::Section * section = *it;
        section->mark(marks);
    }

    /* all unmarked pointers should be deleted but since the destructors
     * of AST nodes will delete child objects we only need to delete
     * AST nodes that are not the child object of another node.
     * If we mark every single node and add up all the times a node was marked
     * then nodes marked only once will be top-level nodes. Those are the
     * nodes we can destroy safely and ensure the rest of the nodes will
     * die with them.
     *
     * this has worst-case performance of O(N^2)
     */
    std::map<const void *, int> memory;
    for (std::list<Ast::Collectable>::iterator it = saved_pointers.begin(); it != saved_pointers.end(); it++){
        Ast::Collectable & collect = *it;

        /* only look at unmarked objects */
        if (! marks[collect.pointer()]){
            std::map<const void *, bool> temp;
            collect.mark(temp);

            /* merge marks into the total count */
            for (std::map<const void *, bool>::iterator mit = temp.begin(); mit != temp.end(); mit++){
                memory[mit->first] += 1;
            }
        }
    }

    /* finally destroy the nodes with no parents */
    for (std::list<Ast::Collectable>::iterator it = saved_pointers.begin(); it != saved_pointers.end(); it++){
        Ast::Collectable & collect = *it;
        if (memory[collect.pointer()] == 1){
            collect.destroy();
        }
    }
    // std::cout << "Destroying everything" << std::endl;
    saved_pointers.clear();
}




Result rule_start(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_start.calculated()){
            return column.chunk0->chunk_start;
        }
    }
    
    int myposition = position;
    tail_peg_1:
    Value current;
    Result result_peg_2(myposition);
    
    {
    
        {
                Value value((void*) 0);
                value = makeSectionList(); save(as<SectionList*>(value));
                result_peg_2.setValue(value);
            }
            current = result_peg_2.getValues();
        
        
        
        {
                
                result_peg_2.reset();
                do{
                    Result result_peg_101(result_peg_2.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_101.getPosition()))){
                                result_peg_101.nextPosition();
                            } else {
                                goto out_peg_104;
                            }
                        }
                        result_peg_101.setValue((void*) " ");
                            
                    }
                    goto success_peg_102;
                    out_peg_104:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_101.getPosition()))){
                                result_peg_101.nextPosition();
                            } else {
                                goto out_peg_106;
                            }
                        }
                        result_peg_101.setValue((void*) "\t");
                            
                    }
                    goto success_peg_102;
                    out_peg_106:
                    {
                        
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar(";"[i], stream.get(result_peg_101.getPosition()))){
                                            result_peg_101.nextPosition();
                                        } else {
                                            goto out_peg_152;
                                        }
                                    }
                                    result_peg_101.setValue((void*) ";");
                                
                                
                                
                                result_peg_101.reset();
                                    do{
                                        Result result_peg_162(result_peg_101.getPosition());
                                        {
                                        
                                            Result result_peg_165(result_peg_162);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_165.getPosition()))){
                                                        result_peg_165.nextPosition();
                                                    } else {
                                                        goto not_peg_164;
                                                    }
                                                }
                                                result_peg_165.setValue((void*) "\n");
                                                goto loop_peg_161;
                                                not_peg_164:
                                                result_peg_162.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_166 = stream.get(result_peg_162.getPosition());
                                                if (temp_peg_166 != '\0'){
                                                    result_peg_162.setValue((void*) temp_peg_166);
                                                    result_peg_162.nextPosition();
                                                } else {
                                                    goto loop_peg_161;
                                                }
                                            
                                            
                                        }
                                        result_peg_101.addResult(result_peg_162);
                                    } while (true);
                                    loop_peg_161:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_150;
                        out_peg_152:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("="[i], stream.get(result_peg_101.getPosition()))){
                                            result_peg_101.nextPosition();
                                        } else {
                                            goto out_peg_168;
                                        }
                                    }
                                    result_peg_101.setValue((void*) "=");
                                
                                
                                
                                result_peg_101.reset();
                                    do{
                                        Result result_peg_178(result_peg_101.getPosition());
                                        {
                                        
                                            Result result_peg_181(result_peg_178);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_181.getPosition()))){
                                                        result_peg_181.nextPosition();
                                                    } else {
                                                        goto not_peg_180;
                                                    }
                                                }
                                                result_peg_181.setValue((void*) "\n");
                                                goto loop_peg_177;
                                                not_peg_180:
                                                result_peg_178.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_182 = stream.get(result_peg_178.getPosition());
                                                if (temp_peg_182 != '\0'){
                                                    result_peg_178.setValue((void*) temp_peg_182);
                                                    result_peg_178.nextPosition();
                                                } else {
                                                    goto loop_peg_177;
                                                }
                                            
                                            
                                        }
                                        result_peg_101.addResult(result_peg_178);
                                    } while (true);
                                    loop_peg_177:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_150;
                        out_peg_168:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("-"[i], stream.get(result_peg_101.getPosition()))){
                                            result_peg_101.nextPosition();
                                        } else {
                                            goto out_peg_184;
                                        }
                                    }
                                    result_peg_101.setValue((void*) "-");
                                
                                
                                
                                result_peg_101.reset();
                                    do{
                                        Result result_peg_190(result_peg_101.getPosition());
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_190.getPosition()))){
                                                result_peg_190.nextPosition();
                                            } else {
                                                goto loop_peg_189;
                                            }
                                        }
                                        result_peg_190.setValue((void*) "-");
                                        result_peg_101.addResult(result_peg_190);
                                    } while (true);
                                    loop_peg_189:
                                    if (result_peg_101.matches() == 0){
                                        goto out_peg_184;
                                    }
                                
                                
                            }
                        }
                        goto success_peg_150;
                        out_peg_184:
                        goto out_peg_108;
                        success_peg_150:
                        ;
                        
                    }
                    goto success_peg_102;
                    out_peg_108:
                    goto loop_peg_100;
                    success_peg_102:
                    ;
                    result_peg_2.addResult(result_peg_101);
                } while (true);
                loop_peg_100:
                ;
                        
            }
            goto success_peg_6;
            out_peg_8:
            goto out_peg_3;
            success_peg_6:
            ;
        
        
        
        result_peg_2.reset();
            do{
                Result result_peg_193(result_peg_2.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_193.getPosition()))){
                            result_peg_193.nextPosition();
                        } else {
                            goto out_peg_196;
                        }
                    }
                    result_peg_193.setValue((void*) "\n");
                        
                }
                goto success_peg_194;
                out_peg_196:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_193.getPosition()))){
                            result_peg_193.nextPosition();
                        } else {
                            goto out_peg_198;
                        }
                    }
                    result_peg_193.setValue((void*) "\r");
                        
                }
                goto success_peg_194;
                out_peg_198:
                goto loop_peg_192;
                success_peg_194:
                ;
                result_peg_2.addResult(result_peg_193);
            } while (true);
            loop_peg_192:
            ;
        
        
        
        result_peg_2.reset();
            do{
                Result result_peg_201(result_peg_2.getPosition());
                {
                
                    result_peg_201 = rule_line(stream, result_peg_201.getPosition(), current);
                        if (result_peg_201.error()){
                            goto loop_peg_200;
                        }
                    
                    
                    
                    {
                            
                            result_peg_201.reset();
                            do{
                                Result result_peg_299(result_peg_201.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_299.getPosition()))){
                                            result_peg_299.nextPosition();
                                        } else {
                                            goto out_peg_302;
                                        }
                                    }
                                    result_peg_299.setValue((void*) " ");
                                        
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
                                    result_peg_299.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_300;
                                out_peg_304:
                                {
                                    
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar(";"[i], stream.get(result_peg_299.getPosition()))){
                                                        result_peg_299.nextPosition();
                                                    } else {
                                                        goto out_peg_350;
                                                    }
                                                }
                                                result_peg_299.setValue((void*) ";");
                                            
                                            
                                            
                                            result_peg_299.reset();
                                                do{
                                                    Result result_peg_360(result_peg_299.getPosition());
                                                    {
                                                    
                                                        Result result_peg_363(result_peg_360);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_363.getPosition()))){
                                                                    result_peg_363.nextPosition();
                                                                } else {
                                                                    goto not_peg_362;
                                                                }
                                                            }
                                                            result_peg_363.setValue((void*) "\n");
                                                            goto loop_peg_359;
                                                            not_peg_362:
                                                            result_peg_360.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_364 = stream.get(result_peg_360.getPosition());
                                                            if (temp_peg_364 != '\0'){
                                                                result_peg_360.setValue((void*) temp_peg_364);
                                                                result_peg_360.nextPosition();
                                                            } else {
                                                                goto loop_peg_359;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_299.addResult(result_peg_360);
                                                } while (true);
                                                loop_peg_359:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_348;
                                    out_peg_350:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("="[i], stream.get(result_peg_299.getPosition()))){
                                                        result_peg_299.nextPosition();
                                                    } else {
                                                        goto out_peg_366;
                                                    }
                                                }
                                                result_peg_299.setValue((void*) "=");
                                            
                                            
                                            
                                            result_peg_299.reset();
                                                do{
                                                    Result result_peg_376(result_peg_299.getPosition());
                                                    {
                                                    
                                                        Result result_peg_379(result_peg_376);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_379.getPosition()))){
                                                                    result_peg_379.nextPosition();
                                                                } else {
                                                                    goto not_peg_378;
                                                                }
                                                            }
                                                            result_peg_379.setValue((void*) "\n");
                                                            goto loop_peg_375;
                                                            not_peg_378:
                                                            result_peg_376.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_380 = stream.get(result_peg_376.getPosition());
                                                            if (temp_peg_380 != '\0'){
                                                                result_peg_376.setValue((void*) temp_peg_380);
                                                                result_peg_376.nextPosition();
                                                            } else {
                                                                goto loop_peg_375;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_299.addResult(result_peg_376);
                                                } while (true);
                                                loop_peg_375:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_348;
                                    out_peg_366:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("-"[i], stream.get(result_peg_299.getPosition()))){
                                                        result_peg_299.nextPosition();
                                                    } else {
                                                        goto out_peg_382;
                                                    }
                                                }
                                                result_peg_299.setValue((void*) "-");
                                            
                                            
                                            
                                            result_peg_299.reset();
                                                do{
                                                    Result result_peg_388(result_peg_299.getPosition());
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_388.getPosition()))){
                                                            result_peg_388.nextPosition();
                                                        } else {
                                                            goto loop_peg_387;
                                                        }
                                                    }
                                                    result_peg_388.setValue((void*) "-");
                                                    result_peg_299.addResult(result_peg_388);
                                                } while (true);
                                                loop_peg_387:
                                                if (result_peg_299.matches() == 0){
                                                    goto out_peg_382;
                                                }
                                            
                                            
                                        }
                                    }
                                    goto success_peg_348;
                                    out_peg_382:
                                    goto out_peg_306;
                                    success_peg_348:
                                    ;
                                    
                                }
                                goto success_peg_300;
                                out_peg_306:
                                goto loop_peg_298;
                                success_peg_300:
                                ;
                                result_peg_201.addResult(result_peg_299);
                            } while (true);
                            loop_peg_298:
                            ;
                                    
                        }
                        goto success_peg_204;
                        out_peg_206:
                        goto loop_peg_200;
                        success_peg_204:
                        ;
                    
                    
                    
                    int save_peg_389 = result_peg_201.getPosition();
                        
                        result_peg_201 = rule_line_end(stream, result_peg_201.getPosition());
                        if (result_peg_201.error()){
                            
                            result_peg_201 = Result(save_peg_389);
                            result_peg_201.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_2.addResult(result_peg_201);
            } while (true);
            loop_peg_200:
            ;
        
        
        
        if ('\0' == stream.get(result_peg_2.getPosition())){
                result_peg_2.nextPosition();
                result_peg_2.setValue((void *) '\0');
            } else {
                goto out_peg_3;
            }
        
        
        
        {
                Value value((void*) 0);
                value = current; cleanup(value);
                result_peg_2.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_start = result_peg_2;
        stream.update(result_peg_2.getPosition());
    }
    
    
    return result_peg_2;
    out_peg_3:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_start = errorResult;
        stream.update(errorResult.getPosition());
    }
    
     cleanup(current); 
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_line.calculated()){
            return column.chunk0->chunk_line;
        }
    }
    
    int myposition = position;
    tail_peg_391:
    
    Result result_peg_392(myposition);
    
    {
    
        result_peg_392 = rule_s(stream, result_peg_392.getPosition());
            if (result_peg_392.error()){
                goto out_peg_393;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_392.getPosition()))){
                                result_peg_392.nextPosition();
                            } else {
                                goto out_peg_397;
                            }
                        }
                        result_peg_392.setValue((void*) ";");
                    
                    
                    
                    result_peg_392.reset();
                        do{
                            Result result_peg_407(result_peg_392.getPosition());
                            {
                            
                                Result result_peg_410(result_peg_407);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_410.getPosition()))){
                                            result_peg_410.nextPosition();
                                        } else {
                                            goto not_peg_409;
                                        }
                                    }
                                    result_peg_410.setValue((void*) "\n");
                                    goto loop_peg_406;
                                    not_peg_409:
                                    result_peg_407.setValue((void*)0);
                                
                                
                                
                                char temp_peg_411 = stream.get(result_peg_407.getPosition());
                                    if (temp_peg_411 != '\0'){
                                        result_peg_407.setValue((void*) temp_peg_411);
                                        result_peg_407.nextPosition();
                                    } else {
                                        goto loop_peg_406;
                                    }
                                
                                
                            }
                            result_peg_392.addResult(result_peg_407);
                        } while (true);
                        loop_peg_406:
                        ;
                    
                    
                }
            }
            goto success_peg_395;
            out_peg_397:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_392.getPosition()))){
                                result_peg_392.nextPosition();
                            } else {
                                goto out_peg_413;
                            }
                        }
                        result_peg_392.setValue((void*) "=");
                    
                    
                    
                    result_peg_392.reset();
                        do{
                            Result result_peg_423(result_peg_392.getPosition());
                            {
                            
                                Result result_peg_426(result_peg_423);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_426.getPosition()))){
                                            result_peg_426.nextPosition();
                                        } else {
                                            goto not_peg_425;
                                        }
                                    }
                                    result_peg_426.setValue((void*) "\n");
                                    goto loop_peg_422;
                                    not_peg_425:
                                    result_peg_423.setValue((void*)0);
                                
                                
                                
                                char temp_peg_427 = stream.get(result_peg_423.getPosition());
                                    if (temp_peg_427 != '\0'){
                                        result_peg_423.setValue((void*) temp_peg_427);
                                        result_peg_423.nextPosition();
                                    } else {
                                        goto loop_peg_422;
                                    }
                                
                                
                            }
                            result_peg_392.addResult(result_peg_423);
                        } while (true);
                        loop_peg_422:
                        ;
                    
                    
                }
            }
            goto success_peg_395;
            out_peg_413:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_392.getPosition()))){
                                result_peg_392.nextPosition();
                            } else {
                                goto out_peg_429;
                            }
                        }
                        result_peg_392.setValue((void*) "-");
                    
                    
                    
                    result_peg_392.reset();
                        do{
                            Result result_peg_435(result_peg_392.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_435.getPosition()))){
                                    result_peg_435.nextPosition();
                                } else {
                                    goto loop_peg_434;
                                }
                            }
                            result_peg_435.setValue((void*) "-");
                            result_peg_392.addResult(result_peg_435);
                        } while (true);
                        loop_peg_434:
                        if (result_peg_392.matches() == 0){
                            goto out_peg_429;
                        }
                    
                    
                }
            }
            goto success_peg_395;
            out_peg_429:
            goto out_peg_393;
            success_peg_395:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_392;
        stream.update(result_peg_392.getPosition());
    }
    
    
    return result_peg_392;
    out_peg_393:
    Result result_peg_436(myposition);
    
    {
    
        result_peg_436 = rule_s(stream, result_peg_436.getPosition());
            if (result_peg_436.error()){
                goto out_peg_437;
            }
        
        
        
        result_peg_436 = rule_section(stream, result_peg_436.getPosition());
            if (result_peg_436.error()){
                goto out_peg_437;
            }
        
        Result result_peg_439 = result_peg_436;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_439.getValues());
                result_peg_436.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_436;
        stream.update(result_peg_436.getPosition());
    }
    
    
    return result_peg_436;
    out_peg_437:
    Result result_peg_440(myposition);
    
    result_peg_440.reset();
    do{
        Result result_peg_443(result_peg_440.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_443.getPosition()))){
                    result_peg_443.nextPosition();
                } else {
                    goto out_peg_446;
                }
            }
            result_peg_443.setValue((void*) " ");
                
        }
        goto success_peg_444;
        out_peg_446:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_443.getPosition()))){
                    result_peg_443.nextPosition();
                } else {
                    goto out_peg_448;
                }
            }
            result_peg_443.setValue((void*) "\t");
                
        }
        goto success_peg_444;
        out_peg_448:
        goto loop_peg_442;
        success_peg_444:
        ;
        result_peg_440.addResult(result_peg_443);
    } while (true);
    loop_peg_442:
    if (result_peg_440.matches() == 0){
        goto out_peg_441;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_440;
        stream.update(result_peg_440.getPosition());
    }
    
    
    return result_peg_440;
    out_peg_441:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_line_end.calculated()){
            return column.chunk0->chunk_line_end;
        }
    }
    
    int myposition = position;
    tail_peg_449:
    
    Result result_peg_450(myposition);
    
    result_peg_450.reset();
    do{
        Result result_peg_453(result_peg_450.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_453.getPosition()))){
                    result_peg_453.nextPosition();
                } else {
                    goto out_peg_456;
                }
            }
            result_peg_453.setValue((void*) "\n");
                
        }
        goto success_peg_454;
        out_peg_456:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_453.getPosition()))){
                    result_peg_453.nextPosition();
                } else {
                    goto out_peg_458;
                }
            }
            result_peg_453.setValue((void*) "\r");
                
        }
        goto success_peg_454;
        out_peg_458:
        goto loop_peg_452;
        success_peg_454:
        ;
        result_peg_450.addResult(result_peg_453);
    } while (true);
    loop_peg_452:
    if (result_peg_450.matches() == 0){
        goto out_peg_451;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_450;
        stream.update(result_peg_450.getPosition());
    }
    
    
    return result_peg_450;
    out_peg_451:
    Result result_peg_459(myposition);
    
    {
    
        Result result_peg_462(result_peg_459.getPosition());
            if ('\0' == stream.get(result_peg_462.getPosition())){
                result_peg_462.nextPosition();
                result_peg_462.setValue((void *) '\0');
            } else {
                goto out_peg_460;
            }
        
        
        
        
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_459;
        stream.update(result_peg_459.getPosition());
    }
    
    
    return result_peg_459;
    out_peg_460:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_s.calculated()){
            return column.chunk0->chunk_s;
        }
    }
    
    int myposition = position;
    tail_peg_463:
    
    Result result_peg_464(myposition);
    
    result_peg_464.reset();
    do{
        Result result_peg_467(result_peg_464.getPosition());
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
        result_peg_464.addResult(result_peg_467);
    } while (true);
    loop_peg_466:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_s = result_peg_464;
        stream.update(result_peg_464.getPosition());
    }
    
    
    return result_peg_464;
    out_peg_465:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_s = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_section.calculated()){
            return column.chunk0->chunk_section;
        }
    }
    
    int myposition = position;
    tail_peg_473:
    Value ast;
    Result result_peg_474(myposition);
    
    {
    
        result_peg_474 = rule_section_start(stream, result_peg_474.getPosition());
            if (result_peg_474.error()){
                goto out_peg_475;
            }
        
        Result result_peg_476 = result_peg_474;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_476.getValues()); save(as<Ast::Section*>(value));
                result_peg_474.setValue(value);
            }
            ast = result_peg_474.getValues();
        
        
        
        {
                
                result_peg_474.reset();
                do{
                    Result result_peg_574(result_peg_474.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_574.getPosition()))){
                                result_peg_574.nextPosition();
                            } else {
                                goto out_peg_577;
                            }
                        }
                        result_peg_574.setValue((void*) " ");
                            
                    }
                    goto success_peg_575;
                    out_peg_577:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_574.getPosition()))){
                                result_peg_574.nextPosition();
                            } else {
                                goto out_peg_579;
                            }
                        }
                        result_peg_574.setValue((void*) "\t");
                            
                    }
                    goto success_peg_575;
                    out_peg_579:
                    {
                        
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar(";"[i], stream.get(result_peg_574.getPosition()))){
                                            result_peg_574.nextPosition();
                                        } else {
                                            goto out_peg_625;
                                        }
                                    }
                                    result_peg_574.setValue((void*) ";");
                                
                                
                                
                                result_peg_574.reset();
                                    do{
                                        Result result_peg_635(result_peg_574.getPosition());
                                        {
                                        
                                            Result result_peg_638(result_peg_635);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_638.getPosition()))){
                                                        result_peg_638.nextPosition();
                                                    } else {
                                                        goto not_peg_637;
                                                    }
                                                }
                                                result_peg_638.setValue((void*) "\n");
                                                goto loop_peg_634;
                                                not_peg_637:
                                                result_peg_635.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_639 = stream.get(result_peg_635.getPosition());
                                                if (temp_peg_639 != '\0'){
                                                    result_peg_635.setValue((void*) temp_peg_639);
                                                    result_peg_635.nextPosition();
                                                } else {
                                                    goto loop_peg_634;
                                                }
                                            
                                            
                                        }
                                        result_peg_574.addResult(result_peg_635);
                                    } while (true);
                                    loop_peg_634:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_623;
                        out_peg_625:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("="[i], stream.get(result_peg_574.getPosition()))){
                                            result_peg_574.nextPosition();
                                        } else {
                                            goto out_peg_641;
                                        }
                                    }
                                    result_peg_574.setValue((void*) "=");
                                
                                
                                
                                result_peg_574.reset();
                                    do{
                                        Result result_peg_651(result_peg_574.getPosition());
                                        {
                                        
                                            Result result_peg_654(result_peg_651);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_654.getPosition()))){
                                                        result_peg_654.nextPosition();
                                                    } else {
                                                        goto not_peg_653;
                                                    }
                                                }
                                                result_peg_654.setValue((void*) "\n");
                                                goto loop_peg_650;
                                                not_peg_653:
                                                result_peg_651.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_655 = stream.get(result_peg_651.getPosition());
                                                if (temp_peg_655 != '\0'){
                                                    result_peg_651.setValue((void*) temp_peg_655);
                                                    result_peg_651.nextPosition();
                                                } else {
                                                    goto loop_peg_650;
                                                }
                                            
                                            
                                        }
                                        result_peg_574.addResult(result_peg_651);
                                    } while (true);
                                    loop_peg_650:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_623;
                        out_peg_641:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("-"[i], stream.get(result_peg_574.getPosition()))){
                                            result_peg_574.nextPosition();
                                        } else {
                                            goto out_peg_657;
                                        }
                                    }
                                    result_peg_574.setValue((void*) "-");
                                
                                
                                
                                result_peg_574.reset();
                                    do{
                                        Result result_peg_663(result_peg_574.getPosition());
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_663.getPosition()))){
                                                result_peg_663.nextPosition();
                                            } else {
                                                goto loop_peg_662;
                                            }
                                        }
                                        result_peg_663.setValue((void*) "-");
                                        result_peg_574.addResult(result_peg_663);
                                    } while (true);
                                    loop_peg_662:
                                    if (result_peg_574.matches() == 0){
                                        goto out_peg_657;
                                    }
                                
                                
                            }
                        }
                        goto success_peg_623;
                        out_peg_657:
                        goto out_peg_581;
                        success_peg_623:
                        ;
                        
                    }
                    goto success_peg_575;
                    out_peg_581:
                    goto loop_peg_573;
                    success_peg_575:
                    ;
                    result_peg_474.addResult(result_peg_574);
                } while (true);
                loop_peg_573:
                ;
                        
            }
            goto success_peg_479;
            out_peg_481:
            goto out_peg_475;
            success_peg_479:
            ;
        
        
        
        result_peg_474.reset();
            do{
                Result result_peg_666(result_peg_474.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_666.getPosition()))){
                            result_peg_666.nextPosition();
                        } else {
                            goto out_peg_669;
                        }
                    }
                    result_peg_666.setValue((void*) "\n");
                        
                }
                goto success_peg_667;
                out_peg_669:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_666.getPosition()))){
                            result_peg_666.nextPosition();
                        } else {
                            goto out_peg_671;
                        }
                    }
                    result_peg_666.setValue((void*) "\r");
                        
                }
                goto success_peg_667;
                out_peg_671:
                goto loop_peg_665;
                success_peg_667:
                ;
                result_peg_474.addResult(result_peg_666);
            } while (true);
            loop_peg_665:
            if (result_peg_474.matches() == 0){
                goto out_peg_475;
            }
        
        
        
        result_peg_474.reset();
            do{
                Result result_peg_674(result_peg_474.getPosition());
                {
                
                    result_peg_674 = rule_section_line(stream, result_peg_674.getPosition(), ast);
                        if (result_peg_674.error()){
                            goto loop_peg_673;
                        }
                    
                    
                    
                    {
                            
                            result_peg_674.reset();
                            do{
                                Result result_peg_772(result_peg_674.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_772.getPosition()))){
                                            result_peg_772.nextPosition();
                                        } else {
                                            goto out_peg_775;
                                        }
                                    }
                                    result_peg_772.setValue((void*) " ");
                                        
                                }
                                goto success_peg_773;
                                out_peg_775:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_772.getPosition()))){
                                            result_peg_772.nextPosition();
                                        } else {
                                            goto out_peg_777;
                                        }
                                    }
                                    result_peg_772.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_773;
                                out_peg_777:
                                {
                                    
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar(";"[i], stream.get(result_peg_772.getPosition()))){
                                                        result_peg_772.nextPosition();
                                                    } else {
                                                        goto out_peg_823;
                                                    }
                                                }
                                                result_peg_772.setValue((void*) ";");
                                            
                                            
                                            
                                            result_peg_772.reset();
                                                do{
                                                    Result result_peg_833(result_peg_772.getPosition());
                                                    {
                                                    
                                                        Result result_peg_836(result_peg_833);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_836.getPosition()))){
                                                                    result_peg_836.nextPosition();
                                                                } else {
                                                                    goto not_peg_835;
                                                                }
                                                            }
                                                            result_peg_836.setValue((void*) "\n");
                                                            goto loop_peg_832;
                                                            not_peg_835:
                                                            result_peg_833.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_837 = stream.get(result_peg_833.getPosition());
                                                            if (temp_peg_837 != '\0'){
                                                                result_peg_833.setValue((void*) temp_peg_837);
                                                                result_peg_833.nextPosition();
                                                            } else {
                                                                goto loop_peg_832;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_772.addResult(result_peg_833);
                                                } while (true);
                                                loop_peg_832:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_821;
                                    out_peg_823:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("="[i], stream.get(result_peg_772.getPosition()))){
                                                        result_peg_772.nextPosition();
                                                    } else {
                                                        goto out_peg_839;
                                                    }
                                                }
                                                result_peg_772.setValue((void*) "=");
                                            
                                            
                                            
                                            result_peg_772.reset();
                                                do{
                                                    Result result_peg_849(result_peg_772.getPosition());
                                                    {
                                                    
                                                        Result result_peg_852(result_peg_849);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_852.getPosition()))){
                                                                    result_peg_852.nextPosition();
                                                                } else {
                                                                    goto not_peg_851;
                                                                }
                                                            }
                                                            result_peg_852.setValue((void*) "\n");
                                                            goto loop_peg_848;
                                                            not_peg_851:
                                                            result_peg_849.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_853 = stream.get(result_peg_849.getPosition());
                                                            if (temp_peg_853 != '\0'){
                                                                result_peg_849.setValue((void*) temp_peg_853);
                                                                result_peg_849.nextPosition();
                                                            } else {
                                                                goto loop_peg_848;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_772.addResult(result_peg_849);
                                                } while (true);
                                                loop_peg_848:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_821;
                                    out_peg_839:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("-"[i], stream.get(result_peg_772.getPosition()))){
                                                        result_peg_772.nextPosition();
                                                    } else {
                                                        goto out_peg_855;
                                                    }
                                                }
                                                result_peg_772.setValue((void*) "-");
                                            
                                            
                                            
                                            result_peg_772.reset();
                                                do{
                                                    Result result_peg_861(result_peg_772.getPosition());
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_861.getPosition()))){
                                                            result_peg_861.nextPosition();
                                                        } else {
                                                            goto loop_peg_860;
                                                        }
                                                    }
                                                    result_peg_861.setValue((void*) "-");
                                                    result_peg_772.addResult(result_peg_861);
                                                } while (true);
                                                loop_peg_860:
                                                if (result_peg_772.matches() == 0){
                                                    goto out_peg_855;
                                                }
                                            
                                            
                                        }
                                    }
                                    goto success_peg_821;
                                    out_peg_855:
                                    goto out_peg_779;
                                    success_peg_821:
                                    ;
                                    
                                }
                                goto success_peg_773;
                                out_peg_779:
                                goto loop_peg_771;
                                success_peg_773:
                                ;
                                result_peg_674.addResult(result_peg_772);
                            } while (true);
                            loop_peg_771:
                            ;
                                    
                        }
                        goto success_peg_677;
                        out_peg_679:
                        goto loop_peg_673;
                        success_peg_677:
                        ;
                    
                    
                    
                    result_peg_674 = rule_line_end(stream, result_peg_674.getPosition());
                        if (result_peg_674.error()){
                            goto loop_peg_673;
                        }
                    
                    
                }
                result_peg_474.addResult(result_peg_674);
            } while (true);
            loop_peg_673:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_474.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_section = result_peg_474;
        stream.update(result_peg_474.getPosition());
    }
    
    
    return result_peg_474;
    out_peg_475:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_section = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_section_line.calculated()){
            return column.chunk1->chunk_section_line;
        }
    }
    
    int myposition = position;
    tail_peg_862:
    Value data;
    Result result_peg_863(myposition);
    
    {
    
        result_peg_863 = rule_s(stream, result_peg_863.getPosition());
            if (result_peg_863.error()){
                goto out_peg_864;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_863.getPosition()))){
                                result_peg_863.nextPosition();
                            } else {
                                goto out_peg_868;
                            }
                        }
                        result_peg_863.setValue((void*) ";");
                    
                    
                    
                    result_peg_863.reset();
                        do{
                            Result result_peg_878(result_peg_863.getPosition());
                            {
                            
                                Result result_peg_881(result_peg_878);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_881.getPosition()))){
                                            result_peg_881.nextPosition();
                                        } else {
                                            goto not_peg_880;
                                        }
                                    }
                                    result_peg_881.setValue((void*) "\n");
                                    goto loop_peg_877;
                                    not_peg_880:
                                    result_peg_878.setValue((void*)0);
                                
                                
                                
                                char temp_peg_882 = stream.get(result_peg_878.getPosition());
                                    if (temp_peg_882 != '\0'){
                                        result_peg_878.setValue((void*) temp_peg_882);
                                        result_peg_878.nextPosition();
                                    } else {
                                        goto loop_peg_877;
                                    }
                                
                                
                            }
                            result_peg_863.addResult(result_peg_878);
                        } while (true);
                        loop_peg_877:
                        ;
                    
                    
                }
            }
            goto success_peg_866;
            out_peg_868:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_863.getPosition()))){
                                result_peg_863.nextPosition();
                            } else {
                                goto out_peg_884;
                            }
                        }
                        result_peg_863.setValue((void*) "=");
                    
                    
                    
                    result_peg_863.reset();
                        do{
                            Result result_peg_894(result_peg_863.getPosition());
                            {
                            
                                Result result_peg_897(result_peg_894);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_897.getPosition()))){
                                            result_peg_897.nextPosition();
                                        } else {
                                            goto not_peg_896;
                                        }
                                    }
                                    result_peg_897.setValue((void*) "\n");
                                    goto loop_peg_893;
                                    not_peg_896:
                                    result_peg_894.setValue((void*)0);
                                
                                
                                
                                char temp_peg_898 = stream.get(result_peg_894.getPosition());
                                    if (temp_peg_898 != '\0'){
                                        result_peg_894.setValue((void*) temp_peg_898);
                                        result_peg_894.nextPosition();
                                    } else {
                                        goto loop_peg_893;
                                    }
                                
                                
                            }
                            result_peg_863.addResult(result_peg_894);
                        } while (true);
                        loop_peg_893:
                        ;
                    
                    
                }
            }
            goto success_peg_866;
            out_peg_884:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_863.getPosition()))){
                                result_peg_863.nextPosition();
                            } else {
                                goto out_peg_900;
                            }
                        }
                        result_peg_863.setValue((void*) "-");
                    
                    
                    
                    result_peg_863.reset();
                        do{
                            Result result_peg_906(result_peg_863.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_906.getPosition()))){
                                    result_peg_906.nextPosition();
                                } else {
                                    goto loop_peg_905;
                                }
                            }
                            result_peg_906.setValue((void*) "-");
                            result_peg_863.addResult(result_peg_906);
                        } while (true);
                        loop_peg_905:
                        if (result_peg_863.matches() == 0){
                            goto out_peg_900;
                        }
                    
                    
                }
            }
            goto success_peg_866;
            out_peg_900:
            goto out_peg_864;
            success_peg_866:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_863;
        stream.update(result_peg_863.getPosition());
    }
    
    
    return result_peg_863;
    out_peg_864:
    Result result_peg_907(myposition);
    
    {
    
        result_peg_907 = rule_s(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_908;
            }
        
        
        
        result_peg_907 = rule_attribute(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_908;
            }
            data = result_peg_907.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addAttribute(asAttribute(data));
                result_peg_907.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_907;
        stream.update(result_peg_907.getPosition());
    }
    
    
    return result_peg_907;
    out_peg_908:
    Result result_peg_911(myposition);
    
    {
    
        result_peg_911 = rule_s(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_912;
            }
        
        
        
        result_peg_911 = rule_valuelist(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_912;
            }
            data = result_peg_911.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_911.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_911;
        stream.update(result_peg_911.getPosition());
    }
    
    
    return result_peg_911;
    out_peg_912:
    Result result_peg_915(myposition);
    
    {
    
        result_peg_915 = rule_s(stream, result_peg_915.getPosition());
            if (result_peg_915.error()){
                goto out_peg_916;
            }
        
        
        
        result_peg_915 = rule_loopstart(stream, result_peg_915.getPosition());
            if (result_peg_915.error()){
                goto out_peg_916;
            }
            data = result_peg_915.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_915.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_915;
        stream.update(result_peg_915.getPosition());
    }
    
    
    return result_peg_915;
    out_peg_916:
    Result result_peg_919(myposition);
    
    {
    
        result_peg_919.reset();
            do{
                Result result_peg_923(result_peg_919.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_923.getPosition()))){
                            result_peg_923.nextPosition();
                        } else {
                            goto out_peg_926;
                        }
                    }
                    result_peg_923.setValue((void*) " ");
                        
                }
                goto success_peg_924;
                out_peg_926:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_923.getPosition()))){
                            result_peg_923.nextPosition();
                        } else {
                            goto out_peg_928;
                        }
                    }
                    result_peg_923.setValue((void*) "\t");
                        
                }
                goto success_peg_924;
                out_peg_928:
                goto loop_peg_922;
                success_peg_924:
                ;
                result_peg_919.addResult(result_peg_923);
            } while (true);
            loop_peg_922:
            if (result_peg_919.matches() == 0){
                goto out_peg_920;
            }
        
        
        
        Result result_peg_930(result_peg_919);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_930.getPosition()))){
                    result_peg_930.nextPosition();
                } else {
                    goto not_peg_929;
                }
            }
            result_peg_930.setValue((void*) "[");
            goto out_peg_920;
            not_peg_929:
            result_peg_919.setValue((void*)0);
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_919;
        stream.update(result_peg_919.getPosition());
    }
    
    
    return result_peg_919;
    out_peg_920:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_section_start.calculated()){
            return column.chunk1->chunk_section_start;
        }
    }
    
    int myposition = position;
    tail_peg_931:
    Value data;
    Result result_peg_932(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_932.getPosition()))){
                    result_peg_932.nextPosition();
                } else {
                    goto out_peg_933;
                }
            }
            result_peg_932.setValue((void*) "[");
        
        
        
        result_peg_932 = rule_s(stream, result_peg_932.getPosition());
            if (result_peg_932.error()){
                goto out_peg_933;
            }
        
        
        
        result_peg_932.reset();
            do{
                Result result_peg_938(result_peg_932.getPosition());
                {
                
                    Result result_peg_941(result_peg_938);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_941.getPosition()))){
                                result_peg_941.nextPosition();
                            } else {
                                goto not_peg_940;
                            }
                        }
                        result_peg_941.setValue((void*) "]");
                        goto loop_peg_937;
                        not_peg_940:
                        result_peg_938.setValue((void*)0);
                    
                    
                    
                    char temp_peg_942 = stream.get(result_peg_938.getPosition());
                        if (temp_peg_942 != '\0'){
                            result_peg_938.setValue((void*) temp_peg_942);
                            result_peg_938.nextPosition();
                        } else {
                            goto loop_peg_937;
                        }
                    
                    
                }
                result_peg_932.addResult(result_peg_938);
            } while (true);
            loop_peg_937:
            if (result_peg_932.matches() == 0){
                goto out_peg_933;
            }
            data = result_peg_932.getValues();
        
        
        
        result_peg_932 = rule_s(stream, result_peg_932.getPosition());
            if (result_peg_932.error()){
                goto out_peg_933;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_932.getPosition()))){
                    result_peg_932.nextPosition();
                } else {
                    goto out_peg_933;
                }
            }
            result_peg_932.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(data); save(as<std::string*>(value));
                result_peg_932.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_start = result_peg_932;
        stream.update(result_peg_932.getPosition());
    }
    
    
    return result_peg_932;
    out_peg_933:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_start = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_loopstart.calculated()){
            return column.chunk1->chunk_loopstart;
        }
    }
    
    int myposition = position;
    tail_peg_945:
    
    Result result_peg_946(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_946.getPosition()))){
                    result_peg_946.nextPosition();
                } else {
                    goto out_peg_947;
                }
            }
            result_peg_946.setValue((void*) "loopstart");
        
        
        
        {
                Value value((void*) 0);
                value = makeValue(); save(as<Ast::Value*>(value));
                result_peg_946.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_loopstart = result_peg_946;
        stream.update(result_peg_946.getPosition());
    }
    
    
    return result_peg_946;
    out_peg_947:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_loopstart = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_name.calculated()){
            return column.chunk1->chunk_name;
        }
    }
    
    int myposition = position;
    tail_peg_949:
    
    Result result_peg_950(myposition);
    
    {
    
        {
                
                char letter_peg_957 = stream.get(result_peg_950.getPosition());
                if (letter_peg_957 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_957) != NULL){
                    result_peg_950.nextPosition();
                    result_peg_950.setValue((void*) letter_peg_957);
                } else {
                    goto out_peg_955;
                }
                
            }
            goto success_peg_953;
            out_peg_955:
            goto out_peg_951;
            success_peg_953:
            ;
        
        Result result_peg_952 = result_peg_950;
        
        result_peg_950.reset();
            do{
                Result result_peg_960(result_peg_950.getPosition());
                result_peg_960 = rule_alpha_digit(stream, result_peg_960.getPosition());
                if (result_peg_960.error()){
                    goto loop_peg_959;
                }
                result_peg_950.addResult(result_peg_960);
            } while (true);
            loop_peg_959:
            ;
        
        Result result_peg_958 = result_peg_950;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_952.getValues().getValue(),result_peg_958.getValues()); save(as<std::string*>(value));
                result_peg_950.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_name = result_peg_950;
        stream.update(result_peg_950.getPosition());
    }
    
    
    return result_peg_950;
    out_peg_951:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_alpha_digit.calculated()){
            return column.chunk1->chunk_alpha_digit;
        }
    }
    
    int myposition = position;
    tail_peg_961:
    
    Result result_peg_962(myposition);
    
    {
        
        char letter_peg_968 = stream.get(result_peg_962.getPosition());
        if (letter_peg_968 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_968) != NULL){
            result_peg_962.nextPosition();
            result_peg_962.setValue((void*) letter_peg_968);
        } else {
            goto out_peg_966;
        }
        
    }
    goto success_peg_964;
    out_peg_966:
    goto out_peg_963;
    success_peg_964:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_alpha_digit = result_peg_962;
        stream.update(result_peg_962.getPosition());
    }
    
    
    return result_peg_962;
    out_peg_963:
    Result result_peg_969(myposition);
    
    {
        
        char letter_peg_975 = stream.get(result_peg_969.getPosition());
        if (letter_peg_975 != '\0' && strchr("0123456789", letter_peg_975) != NULL){
            result_peg_969.nextPosition();
            result_peg_969.setValue((void*) letter_peg_975);
        } else {
            goto out_peg_973;
        }
        
    }
    goto success_peg_971;
    out_peg_973:
    goto out_peg_970;
    success_peg_971:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_alpha_digit = result_peg_969;
        stream.update(result_peg_969.getPosition());
    }
    
    
    return result_peg_969;
    out_peg_970:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_alpha_digit = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_line_end_or_comment.calculated()){
            return column.chunk2->chunk_line_end_or_comment;
        }
    }
    
    int myposition = position;
    tail_peg_976:
    
    Result result_peg_977(myposition);
    
    result_peg_977 = rule_line_end(stream, result_peg_977.getPosition());
    if (result_peg_977.error()){
        goto out_peg_978;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = result_peg_977;
        stream.update(result_peg_977.getPosition());
    }
    
    
    return result_peg_977;
    out_peg_978:
    Result result_peg_979(myposition);
    
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_979.getPosition()))){
                        result_peg_979.nextPosition();
                    } else {
                        goto out_peg_983;
                    }
                }
                result_peg_979.setValue((void*) ";");
            
            
            
            result_peg_979.reset();
                do{
                    Result result_peg_993(result_peg_979.getPosition());
                    {
                    
                        Result result_peg_996(result_peg_993);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_996.getPosition()))){
                                    result_peg_996.nextPosition();
                                } else {
                                    goto not_peg_995;
                                }
                            }
                            result_peg_996.setValue((void*) "\n");
                            goto loop_peg_992;
                            not_peg_995:
                            result_peg_993.setValue((void*)0);
                        
                        
                        
                        char temp_peg_997 = stream.get(result_peg_993.getPosition());
                            if (temp_peg_997 != '\0'){
                                result_peg_993.setValue((void*) temp_peg_997);
                                result_peg_993.nextPosition();
                            } else {
                                goto loop_peg_992;
                            }
                        
                        
                    }
                    result_peg_979.addResult(result_peg_993);
                } while (true);
                loop_peg_992:
                ;
            
            
        }
    }
    goto success_peg_981;
    out_peg_983:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_979.getPosition()))){
                        result_peg_979.nextPosition();
                    } else {
                        goto out_peg_999;
                    }
                }
                result_peg_979.setValue((void*) "=");
            
            
            
            result_peg_979.reset();
                do{
                    Result result_peg_1009(result_peg_979.getPosition());
                    {
                    
                        Result result_peg_1012(result_peg_1009);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_1012.getPosition()))){
                                    result_peg_1012.nextPosition();
                                } else {
                                    goto not_peg_1011;
                                }
                            }
                            result_peg_1012.setValue((void*) "\n");
                            goto loop_peg_1008;
                            not_peg_1011:
                            result_peg_1009.setValue((void*)0);
                        
                        
                        
                        char temp_peg_1013 = stream.get(result_peg_1009.getPosition());
                            if (temp_peg_1013 != '\0'){
                                result_peg_1009.setValue((void*) temp_peg_1013);
                                result_peg_1009.nextPosition();
                            } else {
                                goto loop_peg_1008;
                            }
                        
                        
                    }
                    result_peg_979.addResult(result_peg_1009);
                } while (true);
                loop_peg_1008:
                ;
            
            
        }
    }
    goto success_peg_981;
    out_peg_999:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_979.getPosition()))){
                        result_peg_979.nextPosition();
                    } else {
                        goto out_peg_1015;
                    }
                }
                result_peg_979.setValue((void*) "-");
            
            
            
            result_peg_979.reset();
                do{
                    Result result_peg_1021(result_peg_979.getPosition());
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_1021.getPosition()))){
                            result_peg_1021.nextPosition();
                        } else {
                            goto loop_peg_1020;
                        }
                    }
                    result_peg_1021.setValue((void*) "-");
                    result_peg_979.addResult(result_peg_1021);
                } while (true);
                loop_peg_1020:
                if (result_peg_979.matches() == 0){
                    goto out_peg_1015;
                }
            
            
        }
    }
    goto success_peg_981;
    out_peg_1015:
    goto out_peg_980;
    success_peg_981:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = result_peg_979;
        stream.update(result_peg_979.getPosition());
    }
    
    
    return result_peg_979;
    out_peg_980:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_filename.calculated()){
            return column.chunk2->chunk_filename;
        }
    }
    
    int myposition = position;
    tail_peg_1022:
    Value file;
    Result result_peg_1023(myposition);
    
    {
    
        Result result_peg_1027(result_peg_1023);
            for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1027.getPosition()))){
                    result_peg_1027.nextPosition();
                } else {
                    goto not_peg_1026;
                }
            }
            result_peg_1027.setValue((void*) "\"");
            goto out_peg_1024;
            not_peg_1026:
            result_peg_1023.setValue((void*)0);
        
        
        
        result_peg_1023.reset();
            do{
                Result result_peg_1030(result_peg_1023.getPosition());
                result_peg_1030 = rule_filename_char(stream, result_peg_1030.getPosition());
                if (result_peg_1030.error()){
                    goto loop_peg_1029;
                }
                result_peg_1023.addResult(result_peg_1030);
            } while (true);
            loop_peg_1029:
            if (result_peg_1023.matches() == 0){
                goto out_peg_1024;
            }
            file = result_peg_1023.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new Ast::Filename(toString(file)); save(as<Ast::Filename*>(value));
                result_peg_1023.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename = result_peg_1023;
        stream.update(result_peg_1023.getPosition());
    }
    
    
    return result_peg_1023;
    out_peg_1024:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_filename_char(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_filename_char.calculated()){
            return column.chunk2->chunk_filename_char;
        }
    }
    
    int myposition = position;
    tail_peg_1031:
    
    Result result_peg_1032(myposition);
    
    {
    
        Result result_peg_1036(result_peg_1032);
            for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_1036.getPosition()))){
                    result_peg_1036.nextPosition();
                } else {
                    goto not_peg_1035;
                }
            }
            result_peg_1036.setValue((void*) ",");
            goto out_peg_1033;
            not_peg_1035:
            result_peg_1032.setValue((void*)0);
        
        
        
        Result result_peg_1039(result_peg_1032);
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_1039.getPosition()))){
                    result_peg_1039.nextPosition();
                } else {
                    goto not_peg_1038;
                }
            }
            result_peg_1039.setValue((void*) "\n");
            goto out_peg_1033;
            not_peg_1038:
            result_peg_1032.setValue((void*)0);
        
        
        
        Result result_peg_1042(result_peg_1032);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_1042.getPosition()))){
                    result_peg_1042.nextPosition();
                } else {
                    goto not_peg_1041;
                }
            }
            result_peg_1042.setValue((void*) "[");
            goto out_peg_1033;
            not_peg_1041:
            result_peg_1032.setValue((void*)0);
        
        
        
        Result result_peg_1045(result_peg_1032);
            if ((unsigned char) stream.get(result_peg_1045.getPosition()) == (unsigned char) 13){
                result_peg_1045.nextPosition();
            } else {
                goto not_peg_1044;
            }
            result_peg_1045.setValue((void*) 13);
            goto out_peg_1033;
            not_peg_1044:
            result_peg_1032.setValue((void*)0);
        
        
        
        Result result_peg_1048(result_peg_1032);
            for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_1048.getPosition()))){
                    result_peg_1048.nextPosition();
                } else {
                    goto not_peg_1047;
                }
            }
            result_peg_1048.setValue((void*) ";");
            goto out_peg_1033;
            not_peg_1047:
            result_peg_1032.setValue((void*)0);
        
        
        
        char temp_peg_1049 = stream.get(result_peg_1032.getPosition());
            if (temp_peg_1049 != '\0'){
                result_peg_1032.setValue((void*) temp_peg_1049);
                result_peg_1032.nextPosition();
            } else {
                goto out_peg_1033;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename_char = result_peg_1032;
        stream.update(result_peg_1032.getPosition());
    }
    
    
    return result_peg_1032;
    out_peg_1033:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename_char = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_attribute.calculated()){
            return column.chunk2->chunk_attribute;
        }
    }
    
    int myposition = position;
    tail_peg_1050:
    Value id;
    Value data;
    Value index;
    Result result_peg_1051(myposition);
    
    {
    
        result_peg_1051 = rule_identifier(stream, result_peg_1051.getPosition());
            if (result_peg_1051.error()){
                goto out_peg_1052;
            }
            id = result_peg_1051.getValues();
        
        
        
        result_peg_1051 = rule_s(stream, result_peg_1051.getPosition());
            if (result_peg_1051.error()){
                goto out_peg_1052;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1051.getPosition()))){
                    result_peg_1051.nextPosition();
                } else {
                    goto out_peg_1052;
                }
            }
            result_peg_1051.setValue((void*) "=");
        
        
        
        result_peg_1051 = rule_s(stream, result_peg_1051.getPosition());
            if (result_peg_1051.error()){
                goto out_peg_1052;
            }
        
        
        
        Result result_peg_1058(result_peg_1051.getPosition());
            result_peg_1058 = rule_line_end_or_comment(stream, result_peg_1058.getPosition());
            if (result_peg_1058.error()){
                goto out_peg_1052;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id); save(as<Ast::Attribute*>(value));
                result_peg_1051.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1051;
        stream.update(result_peg_1051.getPosition());
    }
    
    
    return result_peg_1051;
    out_peg_1052:
    Result result_peg_1059(myposition);
    
    {
    
        result_peg_1059 = rule_identifier(stream, result_peg_1059.getPosition());
            if (result_peg_1059.error()){
                goto out_peg_1060;
            }
            id = result_peg_1059.getValues();
        
        
        
        result_peg_1059 = rule_s(stream, result_peg_1059.getPosition());
            if (result_peg_1059.error()){
                goto out_peg_1060;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1059.getPosition()))){
                    result_peg_1059.nextPosition();
                } else {
                    goto out_peg_1060;
                }
            }
            result_peg_1059.setValue((void*) "=");
        
        
        
        result_peg_1059 = rule_s(stream, result_peg_1059.getPosition());
            if (result_peg_1059.error()){
                goto out_peg_1060;
            }
        
        
        
        result_peg_1059 = rule_valuelist(stream, result_peg_1059.getPosition());
            if (result_peg_1059.error()){
                goto out_peg_1060;
            }
            data = result_peg_1059.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id, data); save(as<Ast::Attribute*>(value));
                result_peg_1059.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1059;
        stream.update(result_peg_1059.getPosition());
    }
    
    
    return result_peg_1059;
    out_peg_1060:
    Result result_peg_1066(myposition);
    
    {
    
        result_peg_1066 = rule_identifier(stream, result_peg_1066.getPosition());
            if (result_peg_1066.error()){
                goto out_peg_1067;
            }
            id = result_peg_1066.getValues();
        
        
        
        result_peg_1066 = rule_s(stream, result_peg_1066.getPosition());
            if (result_peg_1066.error()){
                goto out_peg_1067;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1066.getPosition()))){
                    result_peg_1066.nextPosition();
                } else {
                    goto out_peg_1067;
                }
            }
            result_peg_1066.setValue((void*) "=");
        
        
        
        result_peg_1066 = rule_s(stream, result_peg_1066.getPosition());
            if (result_peg_1066.error()){
                goto out_peg_1067;
            }
        
        
        
        result_peg_1066 = rule_filename(stream, result_peg_1066.getPosition());
            if (result_peg_1066.error()){
                goto out_peg_1067;
            }
            data = result_peg_1066.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeFilename(id, data); save(as<Ast::Attribute*>(value));
                result_peg_1066.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1066;
        stream.update(result_peg_1066.getPosition());
    }
    
    
    return result_peg_1066;
    out_peg_1067:
    Result result_peg_1073(myposition);
    
    {
    
        result_peg_1073 = rule_identifier(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
            id = result_peg_1073.getValues();
        
        
        
        result_peg_1073 = rule_s(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1073.getPosition()))){
                    result_peg_1073.nextPosition();
                } else {
                    goto out_peg_1074;
                }
            }
            result_peg_1073.setValue((void*) "(");
        
        
        
        result_peg_1073 = rule_s(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
        
        
        
        result_peg_1073 = rule_number(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
            index = result_peg_1073.getValues();
        
        
        
        result_peg_1073 = rule_s(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1073.getPosition()))){
                    result_peg_1073.nextPosition();
                } else {
                    goto out_peg_1074;
                }
            }
            result_peg_1073.setValue((void*) ")");
        
        
        
        result_peg_1073 = rule_s(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1073.getPosition()))){
                    result_peg_1073.nextPosition();
                } else {
                    goto out_peg_1074;
                }
            }
            result_peg_1073.setValue((void*) "=");
        
        
        
        result_peg_1073 = rule_s(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
        
        
        
        result_peg_1073 = rule_valuelist(stream, result_peg_1073.getPosition());
            if (result_peg_1073.error()){
                goto out_peg_1074;
            }
            data = result_peg_1073.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(id, index, data); save(as<Ast::Value*>(value));
                result_peg_1073.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1073;
        stream.update(result_peg_1073.getPosition());
    }
    
    
    return result_peg_1073;
    out_peg_1074:
    Result result_peg_1086(myposition);
    
    {
    
        result_peg_1086 = rule_identifier_list(stream, result_peg_1086.getPosition());
            if (result_peg_1086.error()){
                goto out_peg_1087;
            }
            id = result_peg_1086.getValues();
        
        
        
        result_peg_1086 = rule_s(stream, result_peg_1086.getPosition());
            if (result_peg_1086.error()){
                goto out_peg_1087;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1086.getPosition()))){
                    result_peg_1086.nextPosition();
                } else {
                    goto out_peg_1087;
                }
            }
            result_peg_1086.setValue((void*) "=");
        
        
        
        result_peg_1086 = rule_s(stream, result_peg_1086.getPosition());
            if (result_peg_1086.error()){
                goto out_peg_1087;
            }
        
        
        
        result_peg_1086 = rule_valuelist(stream, result_peg_1086.getPosition());
            if (result_peg_1086.error()){
                goto out_peg_1087;
            }
            data = result_peg_1086.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributes(id, data); save(as<Ast::Attribute*>(value));
                result_peg_1086.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1086;
        stream.update(result_peg_1086.getPosition());
    }
    
    
    return result_peg_1086;
    out_peg_1087:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_identifier.calculated()){
            return column.chunk2->chunk_identifier;
        }
    }
    
    int myposition = position;
    tail_peg_1093:
    
    Result result_peg_1094(myposition);
    
    {
    
        result_peg_1094 = rule_name(stream, result_peg_1094.getPosition());
            if (result_peg_1094.error()){
                goto out_peg_1095;
            }
        
        Result result_peg_1096 = result_peg_1094;
        
        result_peg_1094.reset();
            do{
                Result result_peg_1099(result_peg_1094.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_1099.getPosition()))){
                                result_peg_1099.nextPosition();
                            } else {
                                goto loop_peg_1098;
                            }
                        }
                        result_peg_1099.setValue((void*) ".");
                    
                    
                    
                    result_peg_1099 = rule_name(stream, result_peg_1099.getPosition());
                        if (result_peg_1099.error()){
                            goto loop_peg_1098;
                        }
                    
                    
                }
                result_peg_1094.addResult(result_peg_1099);
            } while (true);
            loop_peg_1098:
            ;
        
        Result result_peg_1097 = result_peg_1094;
        
        {
                Value value((void*) 0);
                value = makeIdentifier(result_peg_1096.getValues(),result_peg_1097.getValues()); save(as<Ast::Identifier*>(value));
                result_peg_1094.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_identifier = result_peg_1094;
        stream.update(result_peg_1094.getPosition());
    }
    
    
    return result_peg_1094;
    out_peg_1095:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_identifier = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_identifier_list(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_identifier_list.calculated()){
            return column.chunk3->chunk_identifier_list;
        }
    }
    
    int myposition = position;
    tail_peg_1101:
    
    Result result_peg_1102(myposition);
    
    {
    
        result_peg_1102 = rule_identifier(stream, result_peg_1102.getPosition());
            if (result_peg_1102.error()){
                goto out_peg_1103;
            }
        
        
        
        result_peg_1102.reset();
            do{
                Result result_peg_1106(result_peg_1102.getPosition());
                {
                
                    result_peg_1106 = rule_s(stream, result_peg_1106.getPosition());
                        if (result_peg_1106.error()){
                            goto loop_peg_1105;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1106.getPosition()))){
                                result_peg_1106.nextPosition();
                            } else {
                                goto loop_peg_1105;
                            }
                        }
                        result_peg_1106.setValue((void*) ",");
                    
                    
                    
                    result_peg_1106 = rule_s(stream, result_peg_1106.getPosition());
                        if (result_peg_1106.error()){
                            goto loop_peg_1105;
                        }
                    
                    
                    
                    result_peg_1106 = rule_filename(stream, result_peg_1106.getPosition());
                        if (result_peg_1106.error()){
                            goto loop_peg_1105;
                        }
                    
                    
                }
                result_peg_1102.addResult(result_peg_1106);
            } while (true);
            loop_peg_1105:
            if (result_peg_1102.matches() == 0){
                goto out_peg_1103;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier_list = result_peg_1102;
        stream.update(result_peg_1102.getPosition());
    }
    
    
    return result_peg_1102;
    out_peg_1103:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier_list = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_valuelist.calculated()){
            return column.chunk3->chunk_valuelist;
        }
    }
    
    int myposition = position;
    tail_peg_1110:
    
    Result result_peg_1111(myposition);
    
    {
    
        result_peg_1111 = rule_value(stream, result_peg_1111.getPosition());
            if (result_peg_1111.error()){
                goto out_peg_1112;
            }
        
        Result result_peg_1113 = result_peg_1111;
        
        result_peg_1111.reset();
            do{
                Result result_peg_1116(result_peg_1111.getPosition());
                {
                
                    result_peg_1116 = rule_s(stream, result_peg_1116.getPosition());
                        if (result_peg_1116.error()){
                            goto loop_peg_1115;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1116.getPosition()))){
                                result_peg_1116.nextPosition();
                            } else {
                                goto loop_peg_1115;
                            }
                        }
                        result_peg_1116.setValue((void*) ",");
                    
                    
                    
                    result_peg_1116 = rule_s(stream, result_peg_1116.getPosition());
                        if (result_peg_1116.error()){
                            goto loop_peg_1115;
                        }
                    
                    
                    
                    int save_peg_1120 = result_peg_1116.getPosition();
                        
                        result_peg_1116 = rule_value(stream, result_peg_1116.getPosition());
                        if (result_peg_1116.error()){
                            
                            result_peg_1116 = Result(save_peg_1120);
                            result_peg_1116.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_1111.addResult(result_peg_1116);
            } while (true);
            loop_peg_1115:
            ;
        
        Result result_peg_1114 = result_peg_1111;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_1113.getValues(),result_peg_1114.getValues()); save(as<Ast::Value*>(value));
                result_peg_1111.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = result_peg_1111;
        stream.update(result_peg_1111.getPosition());
    }
    
    
    return result_peg_1111;
    out_peg_1112:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_value.calculated()){
            return column.chunk3->chunk_value;
        }
    }
    
    int myposition = position;
    tail_peg_1121:
    
    Result result_peg_1122(myposition);
    
    result_peg_1122 = rule_string(stream, result_peg_1122.getPosition());
    if (result_peg_1122.error()){
        goto out_peg_1123;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1122;
        stream.update(result_peg_1122.getPosition());
    }
    
    
    return result_peg_1122;
    out_peg_1123:
    Result result_peg_1124(myposition);
    
    result_peg_1124 = rule_date(stream, result_peg_1124.getPosition());
    if (result_peg_1124.error()){
        goto out_peg_1125;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1124;
        stream.update(result_peg_1124.getPosition());
    }
    
    
    return result_peg_1124;
    out_peg_1125:
    Result result_peg_1126(myposition);
    
    result_peg_1126 = rule_number(stream, result_peg_1126.getPosition());
    if (result_peg_1126.error()){
        goto out_peg_1127;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1126;
        stream.update(result_peg_1126.getPosition());
    }
    
    
    return result_peg_1126;
    out_peg_1127:
    Result result_peg_1128(myposition);
    
    {
    
        result_peg_1128 = rule_keyword(stream, result_peg_1128.getPosition());
            if (result_peg_1128.error()){
                goto out_peg_1129;
            }
        
        Result result_peg_1130 = result_peg_1128;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1130.getValues()); save(as<Ast::Value*>(value));
                result_peg_1128.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1128;
        stream.update(result_peg_1128.getPosition());
    }
    
    
    return result_peg_1128;
    out_peg_1129:
    Result result_peg_1131(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_1131.getPosition()))){
                    result_peg_1131.nextPosition();
                } else {
                    goto out_peg_1132;
                }
            }
            result_peg_1131.setValue((void*) "s");
        
        Result result_peg_1133 = result_peg_1131;
        
        Result result_peg_1136(result_peg_1131);
            {
                
                char letter_peg_1141 = stream.get(result_peg_1136.getPosition());
                if (letter_peg_1141 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1141) != NULL){
                    result_peg_1136.nextPosition();
                    result_peg_1136.setValue((void*) letter_peg_1141);
                } else {
                    goto out_peg_1139;
                }
                
            }
            goto success_peg_1137;
            out_peg_1139:
            goto not_peg_1135;
            success_peg_1137:
            ;
            goto out_peg_1132;
            not_peg_1135:
            result_peg_1131.setValue((void*)0);
        
        
        
        Result result_peg_1144(result_peg_1131);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1144.getPosition()))){
                    result_peg_1144.nextPosition();
                } else {
                    goto not_peg_1143;
                }
            }
            result_peg_1144.setValue((void*) ".");
            goto out_peg_1132;
            not_peg_1143:
            result_peg_1131.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1133.getValues()); save(as<Ast::Value*>(value));
                result_peg_1131.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1131;
        stream.update(result_peg_1131.getPosition());
    }
    
    
    return result_peg_1131;
    out_peg_1132:
    Result result_peg_1145(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_1145.getPosition()))){
                    result_peg_1145.nextPosition();
                } else {
                    goto out_peg_1146;
                }
            }
            result_peg_1145.setValue((void*) "h");
        
        Result result_peg_1147 = result_peg_1145;
        
        Result result_peg_1150(result_peg_1145);
            {
                
                char letter_peg_1155 = stream.get(result_peg_1150.getPosition());
                if (letter_peg_1155 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1155) != NULL){
                    result_peg_1150.nextPosition();
                    result_peg_1150.setValue((void*) letter_peg_1155);
                } else {
                    goto out_peg_1153;
                }
                
            }
            goto success_peg_1151;
            out_peg_1153:
            goto not_peg_1149;
            success_peg_1151:
            ;
            goto out_peg_1146;
            not_peg_1149:
            result_peg_1145.setValue((void*)0);
        
        
        
        Result result_peg_1158(result_peg_1145);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1158.getPosition()))){
                    result_peg_1158.nextPosition();
                } else {
                    goto not_peg_1157;
                }
            }
            result_peg_1158.setValue((void*) ".");
            goto out_peg_1146;
            not_peg_1157:
            result_peg_1145.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1147.getValues()); save(as<Ast::Value*>(value));
                result_peg_1145.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1145;
        stream.update(result_peg_1145.getPosition());
    }
    
    
    return result_peg_1145;
    out_peg_1146:
    Result result_peg_1159(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_1159.getPosition()))){
                    result_peg_1159.nextPosition();
                } else {
                    goto out_peg_1160;
                }
            }
            result_peg_1159.setValue((void*) "a");
        
        Result result_peg_1161 = result_peg_1159;
        
        int save_peg_1163 = result_peg_1159.getPosition();
            
            result_peg_1159 = rule_number(stream, result_peg_1159.getPosition());
            if (result_peg_1159.error()){
                
                result_peg_1159 = Result(save_peg_1163);
                result_peg_1159.setValue((void*) 0);
                
            }
        
        
        
        Result result_peg_1166(result_peg_1159);
            {
                
                char letter_peg_1171 = stream.get(result_peg_1166.getPosition());
                if (letter_peg_1171 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1171) != NULL){
                    result_peg_1166.nextPosition();
                    result_peg_1166.setValue((void*) letter_peg_1171);
                } else {
                    goto out_peg_1169;
                }
                
            }
            goto success_peg_1167;
            out_peg_1169:
            goto not_peg_1165;
            success_peg_1167:
            ;
            goto out_peg_1160;
            not_peg_1165:
            result_peg_1159.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1161.getValues()); save(as<Ast::Value*>(value));
                result_peg_1159.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1159;
        stream.update(result_peg_1159.getPosition());
    }
    
    
    return result_peg_1159;
    out_peg_1160:
    Result result_peg_1172(myposition);
    
    result_peg_1172 = rule_filename(stream, result_peg_1172.getPosition());
    if (result_peg_1172.error()){
        goto out_peg_1173;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1172;
        stream.update(result_peg_1172.getPosition());
    }
    
    
    return result_peg_1172;
    out_peg_1173:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_keyword.calculated()){
            return column.chunk3->chunk_keyword;
        }
    }
    
    int myposition = position;
    tail_peg_1174:
    
    Result result_peg_1175(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareChar("normal"[i], stream.get(result_peg_1175.getPosition()))){
            result_peg_1175.nextPosition();
        } else {
            goto out_peg_1176;
        }
    }
    result_peg_1175.setValue((void*) "normal");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1175;
        stream.update(result_peg_1175.getPosition());
    }
    
    
    return result_peg_1175;
    out_peg_1176:
    Result result_peg_1177(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("parallax"[i], stream.get(result_peg_1177.getPosition()))){
            result_peg_1177.nextPosition();
        } else {
            goto out_peg_1178;
        }
    }
    result_peg_1177.setValue((void*) "parallax");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1177;
        stream.update(result_peg_1177.getPosition());
    }
    
    
    return result_peg_1177;
    out_peg_1178:
    Result result_peg_1179(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("addalpha"[i], stream.get(result_peg_1179.getPosition()))){
            result_peg_1179.nextPosition();
        } else {
            goto out_peg_1180;
        }
    }
    result_peg_1179.setValue((void*) "addalpha");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1179;
        stream.update(result_peg_1179.getPosition());
    }
    
    
    return result_peg_1179;
    out_peg_1180:
    Result result_peg_1181(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareChar("add1"[i], stream.get(result_peg_1181.getPosition()))){
            result_peg_1181.nextPosition();
        } else {
            goto out_peg_1182;
        }
    }
    result_peg_1181.setValue((void*) "add1");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1181;
        stream.update(result_peg_1181.getPosition());
    }
    
    
    return result_peg_1181;
    out_peg_1182:
    Result result_peg_1183(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("add"[i], stream.get(result_peg_1183.getPosition()))){
            result_peg_1183.nextPosition();
        } else {
            goto out_peg_1184;
        }
    }
    result_peg_1183.setValue((void*) "add");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1183;
        stream.update(result_peg_1183.getPosition());
    }
    
    
    return result_peg_1183;
    out_peg_1184:
    Result result_peg_1185(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("sub"[i], stream.get(result_peg_1185.getPosition()))){
            result_peg_1185.nextPosition();
        } else {
            goto out_peg_1186;
        }
    }
    result_peg_1185.setValue((void*) "sub");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1185;
        stream.update(result_peg_1185.getPosition());
    }
    
    
    return result_peg_1185;
    out_peg_1186:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_date.calculated()){
            return column.chunk3->chunk_date;
        }
    }
    
    int myposition = position;
    tail_peg_1187:
    
    Result result_peg_1188(myposition);
    
    {
    
        result_peg_1188.reset();
            do{
                Result result_peg_1192(result_peg_1188.getPosition());
                {
                    
                    char letter_peg_1197 = stream.get(result_peg_1192.getPosition());
                    if (letter_peg_1197 != '\0' && strchr("0123456789", letter_peg_1197) != NULL){
                        result_peg_1192.nextPosition();
                        result_peg_1192.setValue((void*) letter_peg_1197);
                    } else {
                        goto out_peg_1195;
                    }
                    
                }
                goto success_peg_1193;
                out_peg_1195:
                goto loop_peg_1191;
                success_peg_1193:
                ;
                result_peg_1188.addResult(result_peg_1192);
            } while (true);
            loop_peg_1191:
            if (result_peg_1188.matches() == 0){
                goto out_peg_1189;
            }
        
        Result result_peg_1190 = result_peg_1188;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1188.getPosition()))){
                    result_peg_1188.nextPosition();
                } else {
                    goto out_peg_1189;
                }
            }
            result_peg_1188.setValue((void*) ".");
        
        
        
        result_peg_1188.reset();
            do{
                Result result_peg_1201(result_peg_1188.getPosition());
                {
                    
                    char letter_peg_1206 = stream.get(result_peg_1201.getPosition());
                    if (letter_peg_1206 != '\0' && strchr("0123456789", letter_peg_1206) != NULL){
                        result_peg_1201.nextPosition();
                        result_peg_1201.setValue((void*) letter_peg_1206);
                    } else {
                        goto out_peg_1204;
                    }
                    
                }
                goto success_peg_1202;
                out_peg_1204:
                goto loop_peg_1200;
                success_peg_1202:
                ;
                result_peg_1188.addResult(result_peg_1201);
            } while (true);
            loop_peg_1200:
            if (result_peg_1188.matches() == 0){
                goto out_peg_1189;
            }
        
        Result result_peg_1199 = result_peg_1188;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1188.getPosition()))){
                    result_peg_1188.nextPosition();
                } else {
                    goto out_peg_1189;
                }
            }
            result_peg_1188.setValue((void*) ".");
        
        
        
        result_peg_1188.reset();
            do{
                Result result_peg_1210(result_peg_1188.getPosition());
                {
                    
                    char letter_peg_1215 = stream.get(result_peg_1210.getPosition());
                    if (letter_peg_1215 != '\0' && strchr("0123456789", letter_peg_1215) != NULL){
                        result_peg_1210.nextPosition();
                        result_peg_1210.setValue((void*) letter_peg_1215);
                    } else {
                        goto out_peg_1213;
                    }
                    
                }
                goto success_peg_1211;
                out_peg_1213:
                goto loop_peg_1209;
                success_peg_1211:
                ;
                result_peg_1188.addResult(result_peg_1210);
            } while (true);
            loop_peg_1209:
            if (result_peg_1188.matches() == 0){
                goto out_peg_1189;
            }
        
        Result result_peg_1208 = result_peg_1188;
        
        {
                Value value((void*) 0);
                value = makeDate(result_peg_1190.getValues(),result_peg_1199.getValues(),result_peg_1208.getValues()); save(as<Ast::Value*>(value));
                result_peg_1188.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_date = result_peg_1188;
        stream.update(result_peg_1188.getPosition());
    }
    
    
    return result_peg_1188;
    out_peg_1189:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_date = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_string.calculated()){
            return column.chunk4->chunk_string;
        }
    }
    
    int myposition = position;
    tail_peg_1216:
    Value data;
    Result result_peg_1217(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1217.getPosition()))){
                    result_peg_1217.nextPosition();
                } else {
                    goto out_peg_1218;
                }
            }
            result_peg_1217.setValue((void*) "\"");
        
        
        
        result_peg_1217.reset();
            do{
                Result result_peg_1222(result_peg_1217.getPosition());
                {
                
                    Result result_peg_1225(result_peg_1222);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_1225.getPosition()))){
                                result_peg_1225.nextPosition();
                            } else {
                                goto not_peg_1224;
                            }
                        }
                        result_peg_1225.setValue((void*) "\"");
                        goto loop_peg_1221;
                        not_peg_1224:
                        result_peg_1222.setValue((void*)0);
                    
                    
                    
                    Result result_peg_1228(result_peg_1222);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_1228.getPosition()))){
                                result_peg_1228.nextPosition();
                            } else {
                                goto not_peg_1227;
                            }
                        }
                        result_peg_1228.setValue((void*) "\n");
                        goto loop_peg_1221;
                        not_peg_1227:
                        result_peg_1222.setValue((void*)0);
                    
                    
                    
                    char temp_peg_1229 = stream.get(result_peg_1222.getPosition());
                        if (temp_peg_1229 != '\0'){
                            result_peg_1222.setValue((void*) temp_peg_1229);
                            result_peg_1222.nextPosition();
                        } else {
                            goto loop_peg_1221;
                        }
                    
                    
                }
                result_peg_1217.addResult(result_peg_1222);
            } while (true);
            loop_peg_1221:
            ;
            data = result_peg_1217.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1217.getPosition()))){
                    result_peg_1217.nextPosition();
                } else {
                    goto out_peg_1218;
                }
            }
            result_peg_1217.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(data); save(as<Ast::String*>(value));
                result_peg_1217.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_string = result_peg_1217;
        stream.update(result_peg_1217.getPosition());
    }
    
    
    return result_peg_1217;
    out_peg_1218:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_string = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_number.calculated()){
            return column.chunk4->chunk_number;
        }
    }
    
    int myposition = position;
    tail_peg_1231:
    
    Result result_peg_1232(myposition);
    
    {
    
        int save_peg_1235 = result_peg_1232.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1232.getPosition()))){
                        result_peg_1232.nextPosition();
                    } else {
                        goto out_peg_1238;
                    }
                }
                result_peg_1232.setValue((void*) "+");
                    
            }
            goto success_peg_1236;
            out_peg_1238:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1232.getPosition()))){
                        result_peg_1232.nextPosition();
                    } else {
                        goto out_peg_1240;
                    }
                }
                result_peg_1232.setValue((void*) "-");
                    
            }
            goto success_peg_1236;
            out_peg_1240:
            
            result_peg_1232 = Result(save_peg_1235);
            result_peg_1232.setValue((void*) 0);
            
            success_peg_1236:
            ;
        
        Result result_peg_1234 = result_peg_1232;
        
        result_peg_1232 = rule_float_or_integer(stream, result_peg_1232.getPosition());
            if (result_peg_1232.error()){
                goto out_peg_1233;
            }
        
        Result result_peg_1241 = result_peg_1232;
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_1234.getValues(),result_peg_1241.getValues()); save(as<Ast::Value*>(value));
                result_peg_1232.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_number = result_peg_1232;
        stream.update(result_peg_1232.getPosition());
    }
    
    
    return result_peg_1232;
    out_peg_1233:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_number = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_float_or_integer.calculated()){
            return column.chunk4->chunk_float_or_integer;
        }
    }
    
    int myposition = position;
    tail_peg_1242:
    Value left;
    Value right;
    Result result_peg_1243(myposition);
    
    {
    
        result_peg_1243.reset();
            do{
                Result result_peg_1247(result_peg_1243.getPosition());
                {
                    
                    char letter_peg_1252 = stream.get(result_peg_1247.getPosition());
                    if (letter_peg_1252 != '\0' && strchr("0123456789", letter_peg_1252) != NULL){
                        result_peg_1247.nextPosition();
                        result_peg_1247.setValue((void*) letter_peg_1252);
                    } else {
                        goto out_peg_1250;
                    }
                    
                }
                goto success_peg_1248;
                out_peg_1250:
                goto loop_peg_1246;
                success_peg_1248:
                ;
                result_peg_1243.addResult(result_peg_1247);
            } while (true);
            loop_peg_1246:
            ;
            left = result_peg_1243.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1243.getPosition()))){
                    result_peg_1243.nextPosition();
                } else {
                    goto out_peg_1244;
                }
            }
            result_peg_1243.setValue((void*) ".");
        
        
        
        result_peg_1243.reset();
            do{
                Result result_peg_1256(result_peg_1243.getPosition());
                {
                    
                    char letter_peg_1261 = stream.get(result_peg_1256.getPosition());
                    if (letter_peg_1261 != '\0' && strchr("0123456789", letter_peg_1261) != NULL){
                        result_peg_1256.nextPosition();
                        result_peg_1256.setValue((void*) letter_peg_1261);
                    } else {
                        goto out_peg_1259;
                    }
                    
                }
                goto success_peg_1257;
                out_peg_1259:
                goto loop_peg_1255;
                success_peg_1257:
                ;
                result_peg_1243.addResult(result_peg_1256);
            } while (true);
            loop_peg_1255:
            if (result_peg_1243.matches() == 0){
                goto out_peg_1244;
            }
            right = result_peg_1243.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(left,right); save(as<double*>(value));
                result_peg_1243.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_1243;
        stream.update(result_peg_1243.getPosition());
    }
    
    
    return result_peg_1243;
    out_peg_1244:
    Result result_peg_1262(myposition);
    
    {
    
        result_peg_1262.reset();
            do{
                Result result_peg_1266(result_peg_1262.getPosition());
                {
                    
                    char letter_peg_1271 = stream.get(result_peg_1266.getPosition());
                    if (letter_peg_1271 != '\0' && strchr("0123456789", letter_peg_1271) != NULL){
                        result_peg_1266.nextPosition();
                        result_peg_1266.setValue((void*) letter_peg_1271);
                    } else {
                        goto out_peg_1269;
                    }
                    
                }
                goto success_peg_1267;
                out_peg_1269:
                goto loop_peg_1265;
                success_peg_1267:
                ;
                result_peg_1262.addResult(result_peg_1266);
            } while (true);
            loop_peg_1265:
            if (result_peg_1262.matches() == 0){
                goto out_peg_1263;
            }
        
        Result result_peg_1264 = result_peg_1262;
        
        Result result_peg_1274(result_peg_1262);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1274.getPosition()))){
                    result_peg_1274.nextPosition();
                } else {
                    goto not_peg_1273;
                }
            }
            result_peg_1274.setValue((void*) ".");
            goto out_peg_1263;
            not_peg_1273:
            result_peg_1262.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_1264.getValues()); save(as<double*>(value));
                result_peg_1262.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_1262;
        stream.update(result_peg_1262.getPosition());
    }
    
    
    return result_peg_1262;
    out_peg_1263:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

const void * main(const std::string & filename) throw (ParseException){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    return done.getValues().getValue();
}

const void * main(const char * in) throw (ParseException){
    Stream stream(in);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    return done.getValues().getValue();
}



    
    } /* Def */
    
} /* Mugen */

        
