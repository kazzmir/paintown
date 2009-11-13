

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

/*
bool referenced(SectionList * list, const void * value){
    if (value == list){
        return true;
    }
    for (SectionList::iterator it = list->begin(); it != list->end(); it++){
        Ast::Section * section = *it;
        if (section->referenced(value)){
            return true;
        }
    }

    return false;
}

bool referenced(const std::list<Ast::Collectable> & list, Ast::Collectable & collect){
    for (std::list<Ast::Collectable>::const_iterator it = list.begin(); it != list.end(); it++){
        const Ast::Collectable & collect2 = *it;
        if (&collect != &collect2 && collect2.referenced(collect.pointer())){
            return true;
        }
    }
    return false;
}
*/

/* garbage collection */
void cleanup(const Value & value){
    SectionList * list = as<SectionList*>(value);
    std::map<const void*, bool> marks;
    marks[list] = true;

    for (SectionList::iterator it = list->begin(); it != list->end(); it++){
        Ast::Section * section = *it;
        section->mark(marks);
    }

    std::map<const void *, int> memory;
    for (std::list<Ast::Collectable>::iterator it = saved_pointers.begin(); it != saved_pointers.end(); it++){
        Ast::Collectable & collect = *it;
        if (! marks[collect.pointer()]){
            std::map<const void *, bool> temp;
            collect.mark(temp);
            for (std::map<const void *, bool>::iterator mit = temp.begin(); mit != temp.end(); mit++){
                memory[mit->first] += 1;
            }
        }
        /*
        if (! referenced(list, collect.pointer()) && ! referenced(saved_pointers, collect)){
            collect.destroy();
        }
        */
    }
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
        
        
        
        char temp_peg_1046 = stream.get(result_peg_1032.getPosition());
            if (temp_peg_1046 != '\0'){
                result_peg_1032.setValue((void*) temp_peg_1046);
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
    tail_peg_1047:
    Value id;
    Value data;
    Value index;
    Result result_peg_1048(myposition);
    
    {
    
        result_peg_1048 = rule_identifier(stream, result_peg_1048.getPosition());
            if (result_peg_1048.error()){
                goto out_peg_1049;
            }
            id = result_peg_1048.getValues();
        
        
        
        result_peg_1048 = rule_s(stream, result_peg_1048.getPosition());
            if (result_peg_1048.error()){
                goto out_peg_1049;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1048.getPosition()))){
                    result_peg_1048.nextPosition();
                } else {
                    goto out_peg_1049;
                }
            }
            result_peg_1048.setValue((void*) "=");
        
        
        
        result_peg_1048 = rule_s(stream, result_peg_1048.getPosition());
            if (result_peg_1048.error()){
                goto out_peg_1049;
            }
        
        
        
        Result result_peg_1055(result_peg_1048.getPosition());
            result_peg_1055 = rule_line_end_or_comment(stream, result_peg_1055.getPosition());
            if (result_peg_1055.error()){
                goto out_peg_1049;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id); save(as<Ast::Attribute*>(value));
                result_peg_1048.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1048;
        stream.update(result_peg_1048.getPosition());
    }
    
    
    return result_peg_1048;
    out_peg_1049:
    Result result_peg_1056(myposition);
    
    {
    
        result_peg_1056 = rule_identifier(stream, result_peg_1056.getPosition());
            if (result_peg_1056.error()){
                goto out_peg_1057;
            }
            id = result_peg_1056.getValues();
        
        
        
        result_peg_1056 = rule_s(stream, result_peg_1056.getPosition());
            if (result_peg_1056.error()){
                goto out_peg_1057;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1056.getPosition()))){
                    result_peg_1056.nextPosition();
                } else {
                    goto out_peg_1057;
                }
            }
            result_peg_1056.setValue((void*) "=");
        
        
        
        result_peg_1056 = rule_s(stream, result_peg_1056.getPosition());
            if (result_peg_1056.error()){
                goto out_peg_1057;
            }
        
        
        
        result_peg_1056 = rule_valuelist(stream, result_peg_1056.getPosition());
            if (result_peg_1056.error()){
                goto out_peg_1057;
            }
            data = result_peg_1056.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id, data); save(as<Ast::Attribute*>(value));
                result_peg_1056.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1056;
        stream.update(result_peg_1056.getPosition());
    }
    
    
    return result_peg_1056;
    out_peg_1057:
    Result result_peg_1063(myposition);
    
    {
    
        result_peg_1063 = rule_identifier(stream, result_peg_1063.getPosition());
            if (result_peg_1063.error()){
                goto out_peg_1064;
            }
            id = result_peg_1063.getValues();
        
        
        
        result_peg_1063 = rule_s(stream, result_peg_1063.getPosition());
            if (result_peg_1063.error()){
                goto out_peg_1064;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1063.getPosition()))){
                    result_peg_1063.nextPosition();
                } else {
                    goto out_peg_1064;
                }
            }
            result_peg_1063.setValue((void*) "=");
        
        
        
        result_peg_1063 = rule_s(stream, result_peg_1063.getPosition());
            if (result_peg_1063.error()){
                goto out_peg_1064;
            }
        
        
        
        result_peg_1063 = rule_filename(stream, result_peg_1063.getPosition());
            if (result_peg_1063.error()){
                goto out_peg_1064;
            }
            data = result_peg_1063.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeFilename(id, data); save(as<Ast::Attribute*>(value));
                result_peg_1063.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1063;
        stream.update(result_peg_1063.getPosition());
    }
    
    
    return result_peg_1063;
    out_peg_1064:
    Result result_peg_1070(myposition);
    
    {
    
        result_peg_1070 = rule_identifier(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
            id = result_peg_1070.getValues();
        
        
        
        result_peg_1070 = rule_s(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1070.getPosition()))){
                    result_peg_1070.nextPosition();
                } else {
                    goto out_peg_1071;
                }
            }
            result_peg_1070.setValue((void*) "(");
        
        
        
        result_peg_1070 = rule_s(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
        
        
        
        result_peg_1070 = rule_number(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
            index = result_peg_1070.getValues();
        
        
        
        result_peg_1070 = rule_s(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1070.getPosition()))){
                    result_peg_1070.nextPosition();
                } else {
                    goto out_peg_1071;
                }
            }
            result_peg_1070.setValue((void*) ")");
        
        
        
        result_peg_1070 = rule_s(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1070.getPosition()))){
                    result_peg_1070.nextPosition();
                } else {
                    goto out_peg_1071;
                }
            }
            result_peg_1070.setValue((void*) "=");
        
        
        
        result_peg_1070 = rule_s(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
        
        
        
        result_peg_1070 = rule_valuelist(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1071;
            }
            data = result_peg_1070.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(id, index, data); save(as<Ast::Value*>(value));
                result_peg_1070.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1070;
        stream.update(result_peg_1070.getPosition());
    }
    
    
    return result_peg_1070;
    out_peg_1071:
    Result result_peg_1083(myposition);
    
    {
    
        result_peg_1083 = rule_identifier_list(stream, result_peg_1083.getPosition());
            if (result_peg_1083.error()){
                goto out_peg_1084;
            }
            id = result_peg_1083.getValues();
        
        
        
        result_peg_1083 = rule_s(stream, result_peg_1083.getPosition());
            if (result_peg_1083.error()){
                goto out_peg_1084;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1083.getPosition()))){
                    result_peg_1083.nextPosition();
                } else {
                    goto out_peg_1084;
                }
            }
            result_peg_1083.setValue((void*) "=");
        
        
        
        result_peg_1083 = rule_s(stream, result_peg_1083.getPosition());
            if (result_peg_1083.error()){
                goto out_peg_1084;
            }
        
        
        
        result_peg_1083 = rule_valuelist(stream, result_peg_1083.getPosition());
            if (result_peg_1083.error()){
                goto out_peg_1084;
            }
            data = result_peg_1083.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributes(id, data); save(as<Ast::Attribute*>(value));
                result_peg_1083.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1083;
        stream.update(result_peg_1083.getPosition());
    }
    
    
    return result_peg_1083;
    out_peg_1084:
    
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
    tail_peg_1090:
    
    Result result_peg_1091(myposition);
    
    {
    
        result_peg_1091 = rule_name(stream, result_peg_1091.getPosition());
            if (result_peg_1091.error()){
                goto out_peg_1092;
            }
        
        Result result_peg_1093 = result_peg_1091;
        
        result_peg_1091.reset();
            do{
                Result result_peg_1096(result_peg_1091.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_1096.getPosition()))){
                                result_peg_1096.nextPosition();
                            } else {
                                goto loop_peg_1095;
                            }
                        }
                        result_peg_1096.setValue((void*) ".");
                    
                    
                    
                    result_peg_1096 = rule_name(stream, result_peg_1096.getPosition());
                        if (result_peg_1096.error()){
                            goto loop_peg_1095;
                        }
                    
                    
                }
                result_peg_1091.addResult(result_peg_1096);
            } while (true);
            loop_peg_1095:
            ;
        
        Result result_peg_1094 = result_peg_1091;
        
        {
                Value value((void*) 0);
                value = makeIdentifier(result_peg_1093.getValues(),result_peg_1094.getValues()); save(as<Ast::Identifier*>(value));
                result_peg_1091.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_identifier = result_peg_1091;
        stream.update(result_peg_1091.getPosition());
    }
    
    
    return result_peg_1091;
    out_peg_1092:
    
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
    tail_peg_1098:
    
    Result result_peg_1099(myposition);
    
    {
    
        result_peg_1099 = rule_identifier(stream, result_peg_1099.getPosition());
            if (result_peg_1099.error()){
                goto out_peg_1100;
            }
        
        
        
        result_peg_1099.reset();
            do{
                Result result_peg_1103(result_peg_1099.getPosition());
                {
                
                    result_peg_1103 = rule_s(stream, result_peg_1103.getPosition());
                        if (result_peg_1103.error()){
                            goto loop_peg_1102;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1103.getPosition()))){
                                result_peg_1103.nextPosition();
                            } else {
                                goto loop_peg_1102;
                            }
                        }
                        result_peg_1103.setValue((void*) ",");
                    
                    
                    
                    result_peg_1103 = rule_s(stream, result_peg_1103.getPosition());
                        if (result_peg_1103.error()){
                            goto loop_peg_1102;
                        }
                    
                    
                    
                    result_peg_1103 = rule_filename(stream, result_peg_1103.getPosition());
                        if (result_peg_1103.error()){
                            goto loop_peg_1102;
                        }
                    
                    
                }
                result_peg_1099.addResult(result_peg_1103);
            } while (true);
            loop_peg_1102:
            if (result_peg_1099.matches() == 0){
                goto out_peg_1100;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier_list = result_peg_1099;
        stream.update(result_peg_1099.getPosition());
    }
    
    
    return result_peg_1099;
    out_peg_1100:
    
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
    tail_peg_1107:
    
    Result result_peg_1108(myposition);
    
    {
    
        result_peg_1108 = rule_value(stream, result_peg_1108.getPosition());
            if (result_peg_1108.error()){
                goto out_peg_1109;
            }
        
        Result result_peg_1110 = result_peg_1108;
        
        result_peg_1108.reset();
            do{
                Result result_peg_1113(result_peg_1108.getPosition());
                {
                
                    result_peg_1113 = rule_s(stream, result_peg_1113.getPosition());
                        if (result_peg_1113.error()){
                            goto loop_peg_1112;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1113.getPosition()))){
                                result_peg_1113.nextPosition();
                            } else {
                                goto loop_peg_1112;
                            }
                        }
                        result_peg_1113.setValue((void*) ",");
                    
                    
                    
                    result_peg_1113 = rule_s(stream, result_peg_1113.getPosition());
                        if (result_peg_1113.error()){
                            goto loop_peg_1112;
                        }
                    
                    
                    
                    int save_peg_1117 = result_peg_1113.getPosition();
                        
                        result_peg_1113 = rule_value(stream, result_peg_1113.getPosition());
                        if (result_peg_1113.error()){
                            
                            result_peg_1113 = Result(save_peg_1117);
                            result_peg_1113.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_1108.addResult(result_peg_1113);
            } while (true);
            loop_peg_1112:
            ;
        
        Result result_peg_1111 = result_peg_1108;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_1110.getValues(),result_peg_1111.getValues()); save(as<Ast::Value*>(value));
                result_peg_1108.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = result_peg_1108;
        stream.update(result_peg_1108.getPosition());
    }
    
    
    return result_peg_1108;
    out_peg_1109:
    
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
    tail_peg_1118:
    
    Result result_peg_1119(myposition);
    
    result_peg_1119 = rule_string(stream, result_peg_1119.getPosition());
    if (result_peg_1119.error()){
        goto out_peg_1120;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1119;
        stream.update(result_peg_1119.getPosition());
    }
    
    
    return result_peg_1119;
    out_peg_1120:
    Result result_peg_1121(myposition);
    
    result_peg_1121 = rule_date(stream, result_peg_1121.getPosition());
    if (result_peg_1121.error()){
        goto out_peg_1122;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1121;
        stream.update(result_peg_1121.getPosition());
    }
    
    
    return result_peg_1121;
    out_peg_1122:
    Result result_peg_1123(myposition);
    
    result_peg_1123 = rule_number(stream, result_peg_1123.getPosition());
    if (result_peg_1123.error()){
        goto out_peg_1124;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1123;
        stream.update(result_peg_1123.getPosition());
    }
    
    
    return result_peg_1123;
    out_peg_1124:
    Result result_peg_1125(myposition);
    
    {
    
        result_peg_1125 = rule_keyword(stream, result_peg_1125.getPosition());
            if (result_peg_1125.error()){
                goto out_peg_1126;
            }
        
        Result result_peg_1127 = result_peg_1125;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1127.getValues()); save(as<Ast::Value*>(value));
                result_peg_1125.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1125;
        stream.update(result_peg_1125.getPosition());
    }
    
    
    return result_peg_1125;
    out_peg_1126:
    Result result_peg_1128(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_1128.getPosition()))){
                    result_peg_1128.nextPosition();
                } else {
                    goto out_peg_1129;
                }
            }
            result_peg_1128.setValue((void*) "s");
        
        Result result_peg_1130 = result_peg_1128;
        
        Result result_peg_1133(result_peg_1128);
            {
                
                char letter_peg_1138 = stream.get(result_peg_1133.getPosition());
                if (letter_peg_1138 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1138) != NULL){
                    result_peg_1133.nextPosition();
                    result_peg_1133.setValue((void*) letter_peg_1138);
                } else {
                    goto out_peg_1136;
                }
                
            }
            goto success_peg_1134;
            out_peg_1136:
            goto not_peg_1132;
            success_peg_1134:
            ;
            goto out_peg_1129;
            not_peg_1132:
            result_peg_1128.setValue((void*)0);
        
        
        
        Result result_peg_1141(result_peg_1128);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1141.getPosition()))){
                    result_peg_1141.nextPosition();
                } else {
                    goto not_peg_1140;
                }
            }
            result_peg_1141.setValue((void*) ".");
            goto out_peg_1129;
            not_peg_1140:
            result_peg_1128.setValue((void*)0);
        
        
        
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
    Result result_peg_1142(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_1142.getPosition()))){
                    result_peg_1142.nextPosition();
                } else {
                    goto out_peg_1143;
                }
            }
            result_peg_1142.setValue((void*) "h");
        
        Result result_peg_1144 = result_peg_1142;
        
        Result result_peg_1147(result_peg_1142);
            {
                
                char letter_peg_1152 = stream.get(result_peg_1147.getPosition());
                if (letter_peg_1152 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1152) != NULL){
                    result_peg_1147.nextPosition();
                    result_peg_1147.setValue((void*) letter_peg_1152);
                } else {
                    goto out_peg_1150;
                }
                
            }
            goto success_peg_1148;
            out_peg_1150:
            goto not_peg_1146;
            success_peg_1148:
            ;
            goto out_peg_1143;
            not_peg_1146:
            result_peg_1142.setValue((void*)0);
        
        
        
        Result result_peg_1155(result_peg_1142);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1155.getPosition()))){
                    result_peg_1155.nextPosition();
                } else {
                    goto not_peg_1154;
                }
            }
            result_peg_1155.setValue((void*) ".");
            goto out_peg_1143;
            not_peg_1154:
            result_peg_1142.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1144.getValues()); save(as<Ast::Value*>(value));
                result_peg_1142.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1142;
        stream.update(result_peg_1142.getPosition());
    }
    
    
    return result_peg_1142;
    out_peg_1143:
    Result result_peg_1156(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_1156.getPosition()))){
                    result_peg_1156.nextPosition();
                } else {
                    goto out_peg_1157;
                }
            }
            result_peg_1156.setValue((void*) "a");
        
        Result result_peg_1158 = result_peg_1156;
        
        int save_peg_1160 = result_peg_1156.getPosition();
            
            result_peg_1156 = rule_number(stream, result_peg_1156.getPosition());
            if (result_peg_1156.error()){
                
                result_peg_1156 = Result(save_peg_1160);
                result_peg_1156.setValue((void*) 0);
                
            }
        
        
        
        Result result_peg_1163(result_peg_1156);
            {
                
                char letter_peg_1168 = stream.get(result_peg_1163.getPosition());
                if (letter_peg_1168 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1168) != NULL){
                    result_peg_1163.nextPosition();
                    result_peg_1163.setValue((void*) letter_peg_1168);
                } else {
                    goto out_peg_1166;
                }
                
            }
            goto success_peg_1164;
            out_peg_1166:
            goto not_peg_1162;
            success_peg_1164:
            ;
            goto out_peg_1157;
            not_peg_1162:
            result_peg_1156.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1158.getValues()); save(as<Ast::Value*>(value));
                result_peg_1156.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1156;
        stream.update(result_peg_1156.getPosition());
    }
    
    
    return result_peg_1156;
    out_peg_1157:
    Result result_peg_1169(myposition);
    
    result_peg_1169 = rule_filename(stream, result_peg_1169.getPosition());
    if (result_peg_1169.error()){
        goto out_peg_1170;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1169;
        stream.update(result_peg_1169.getPosition());
    }
    
    
    return result_peg_1169;
    out_peg_1170:
    
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
    tail_peg_1171:
    
    Result result_peg_1172(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareChar("normal"[i], stream.get(result_peg_1172.getPosition()))){
            result_peg_1172.nextPosition();
        } else {
            goto out_peg_1173;
        }
    }
    result_peg_1172.setValue((void*) "normal");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1172;
        stream.update(result_peg_1172.getPosition());
    }
    
    
    return result_peg_1172;
    out_peg_1173:
    Result result_peg_1174(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("parallax"[i], stream.get(result_peg_1174.getPosition()))){
            result_peg_1174.nextPosition();
        } else {
            goto out_peg_1175;
        }
    }
    result_peg_1174.setValue((void*) "parallax");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1174;
        stream.update(result_peg_1174.getPosition());
    }
    
    
    return result_peg_1174;
    out_peg_1175:
    Result result_peg_1176(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("addalpha"[i], stream.get(result_peg_1176.getPosition()))){
            result_peg_1176.nextPosition();
        } else {
            goto out_peg_1177;
        }
    }
    result_peg_1176.setValue((void*) "addalpha");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1176;
        stream.update(result_peg_1176.getPosition());
    }
    
    
    return result_peg_1176;
    out_peg_1177:
    Result result_peg_1178(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareChar("add1"[i], stream.get(result_peg_1178.getPosition()))){
            result_peg_1178.nextPosition();
        } else {
            goto out_peg_1179;
        }
    }
    result_peg_1178.setValue((void*) "add1");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1178;
        stream.update(result_peg_1178.getPosition());
    }
    
    
    return result_peg_1178;
    out_peg_1179:
    Result result_peg_1180(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("add"[i], stream.get(result_peg_1180.getPosition()))){
            result_peg_1180.nextPosition();
        } else {
            goto out_peg_1181;
        }
    }
    result_peg_1180.setValue((void*) "add");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1180;
        stream.update(result_peg_1180.getPosition());
    }
    
    
    return result_peg_1180;
    out_peg_1181:
    Result result_peg_1182(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("sub"[i], stream.get(result_peg_1182.getPosition()))){
            result_peg_1182.nextPosition();
        } else {
            goto out_peg_1183;
        }
    }
    result_peg_1182.setValue((void*) "sub");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1182;
        stream.update(result_peg_1182.getPosition());
    }
    
    
    return result_peg_1182;
    out_peg_1183:
    
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
    tail_peg_1184:
    
    Result result_peg_1185(myposition);
    
    {
    
        result_peg_1185.reset();
            do{
                Result result_peg_1189(result_peg_1185.getPosition());
                {
                    
                    char letter_peg_1194 = stream.get(result_peg_1189.getPosition());
                    if (letter_peg_1194 != '\0' && strchr("0123456789", letter_peg_1194) != NULL){
                        result_peg_1189.nextPosition();
                        result_peg_1189.setValue((void*) letter_peg_1194);
                    } else {
                        goto out_peg_1192;
                    }
                    
                }
                goto success_peg_1190;
                out_peg_1192:
                goto loop_peg_1188;
                success_peg_1190:
                ;
                result_peg_1185.addResult(result_peg_1189);
            } while (true);
            loop_peg_1188:
            if (result_peg_1185.matches() == 0){
                goto out_peg_1186;
            }
        
        Result result_peg_1187 = result_peg_1185;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1185.getPosition()))){
                    result_peg_1185.nextPosition();
                } else {
                    goto out_peg_1186;
                }
            }
            result_peg_1185.setValue((void*) ".");
        
        
        
        result_peg_1185.reset();
            do{
                Result result_peg_1198(result_peg_1185.getPosition());
                {
                    
                    char letter_peg_1203 = stream.get(result_peg_1198.getPosition());
                    if (letter_peg_1203 != '\0' && strchr("0123456789", letter_peg_1203) != NULL){
                        result_peg_1198.nextPosition();
                        result_peg_1198.setValue((void*) letter_peg_1203);
                    } else {
                        goto out_peg_1201;
                    }
                    
                }
                goto success_peg_1199;
                out_peg_1201:
                goto loop_peg_1197;
                success_peg_1199:
                ;
                result_peg_1185.addResult(result_peg_1198);
            } while (true);
            loop_peg_1197:
            if (result_peg_1185.matches() == 0){
                goto out_peg_1186;
            }
        
        Result result_peg_1196 = result_peg_1185;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1185.getPosition()))){
                    result_peg_1185.nextPosition();
                } else {
                    goto out_peg_1186;
                }
            }
            result_peg_1185.setValue((void*) ".");
        
        
        
        result_peg_1185.reset();
            do{
                Result result_peg_1207(result_peg_1185.getPosition());
                {
                    
                    char letter_peg_1212 = stream.get(result_peg_1207.getPosition());
                    if (letter_peg_1212 != '\0' && strchr("0123456789", letter_peg_1212) != NULL){
                        result_peg_1207.nextPosition();
                        result_peg_1207.setValue((void*) letter_peg_1212);
                    } else {
                        goto out_peg_1210;
                    }
                    
                }
                goto success_peg_1208;
                out_peg_1210:
                goto loop_peg_1206;
                success_peg_1208:
                ;
                result_peg_1185.addResult(result_peg_1207);
            } while (true);
            loop_peg_1206:
            if (result_peg_1185.matches() == 0){
                goto out_peg_1186;
            }
        
        Result result_peg_1205 = result_peg_1185;
        
        {
                Value value((void*) 0);
                value = makeDate(result_peg_1187.getValues(),result_peg_1196.getValues(),result_peg_1205.getValues()); save(as<Ast::Value*>(value));
                result_peg_1185.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_date = result_peg_1185;
        stream.update(result_peg_1185.getPosition());
    }
    
    
    return result_peg_1185;
    out_peg_1186:
    
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
    tail_peg_1213:
    Value data;
    Result result_peg_1214(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1214.getPosition()))){
                    result_peg_1214.nextPosition();
                } else {
                    goto out_peg_1215;
                }
            }
            result_peg_1214.setValue((void*) "\"");
        
        
        
        result_peg_1214.reset();
            do{
                Result result_peg_1219(result_peg_1214.getPosition());
                {
                
                    Result result_peg_1222(result_peg_1219);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_1222.getPosition()))){
                                result_peg_1222.nextPosition();
                            } else {
                                goto not_peg_1221;
                            }
                        }
                        result_peg_1222.setValue((void*) "\"");
                        goto loop_peg_1218;
                        not_peg_1221:
                        result_peg_1219.setValue((void*)0);
                    
                    
                    
                    Result result_peg_1225(result_peg_1219);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_1225.getPosition()))){
                                result_peg_1225.nextPosition();
                            } else {
                                goto not_peg_1224;
                            }
                        }
                        result_peg_1225.setValue((void*) "\n");
                        goto loop_peg_1218;
                        not_peg_1224:
                        result_peg_1219.setValue((void*)0);
                    
                    
                    
                    char temp_peg_1226 = stream.get(result_peg_1219.getPosition());
                        if (temp_peg_1226 != '\0'){
                            result_peg_1219.setValue((void*) temp_peg_1226);
                            result_peg_1219.nextPosition();
                        } else {
                            goto loop_peg_1218;
                        }
                    
                    
                }
                result_peg_1214.addResult(result_peg_1219);
            } while (true);
            loop_peg_1218:
            ;
            data = result_peg_1214.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1214.getPosition()))){
                    result_peg_1214.nextPosition();
                } else {
                    goto out_peg_1215;
                }
            }
            result_peg_1214.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(data); save(as<Ast::String*>(value));
                result_peg_1214.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_string = result_peg_1214;
        stream.update(result_peg_1214.getPosition());
    }
    
    
    return result_peg_1214;
    out_peg_1215:
    
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
    tail_peg_1228:
    
    Result result_peg_1229(myposition);
    
    {
    
        int save_peg_1232 = result_peg_1229.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1229.getPosition()))){
                        result_peg_1229.nextPosition();
                    } else {
                        goto out_peg_1235;
                    }
                }
                result_peg_1229.setValue((void*) "+");
                    
            }
            goto success_peg_1233;
            out_peg_1235:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1229.getPosition()))){
                        result_peg_1229.nextPosition();
                    } else {
                        goto out_peg_1237;
                    }
                }
                result_peg_1229.setValue((void*) "-");
                    
            }
            goto success_peg_1233;
            out_peg_1237:
            
            result_peg_1229 = Result(save_peg_1232);
            result_peg_1229.setValue((void*) 0);
            
            success_peg_1233:
            ;
        
        Result result_peg_1231 = result_peg_1229;
        
        result_peg_1229 = rule_float_or_integer(stream, result_peg_1229.getPosition());
            if (result_peg_1229.error()){
                goto out_peg_1230;
            }
        
        Result result_peg_1238 = result_peg_1229;
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_1231.getValues(),result_peg_1238.getValues()); save(as<Ast::Value*>(value));
                result_peg_1229.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_number = result_peg_1229;
        stream.update(result_peg_1229.getPosition());
    }
    
    
    return result_peg_1229;
    out_peg_1230:
    
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
    tail_peg_1239:
    Value left;
    Value right;
    Result result_peg_1240(myposition);
    
    {
    
        result_peg_1240.reset();
            do{
                Result result_peg_1244(result_peg_1240.getPosition());
                {
                    
                    char letter_peg_1249 = stream.get(result_peg_1244.getPosition());
                    if (letter_peg_1249 != '\0' && strchr("0123456789", letter_peg_1249) != NULL){
                        result_peg_1244.nextPosition();
                        result_peg_1244.setValue((void*) letter_peg_1249);
                    } else {
                        goto out_peg_1247;
                    }
                    
                }
                goto success_peg_1245;
                out_peg_1247:
                goto loop_peg_1243;
                success_peg_1245:
                ;
                result_peg_1240.addResult(result_peg_1244);
            } while (true);
            loop_peg_1243:
            ;
            left = result_peg_1240.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1240.getPosition()))){
                    result_peg_1240.nextPosition();
                } else {
                    goto out_peg_1241;
                }
            }
            result_peg_1240.setValue((void*) ".");
        
        
        
        result_peg_1240.reset();
            do{
                Result result_peg_1253(result_peg_1240.getPosition());
                {
                    
                    char letter_peg_1258 = stream.get(result_peg_1253.getPosition());
                    if (letter_peg_1258 != '\0' && strchr("0123456789", letter_peg_1258) != NULL){
                        result_peg_1253.nextPosition();
                        result_peg_1253.setValue((void*) letter_peg_1258);
                    } else {
                        goto out_peg_1256;
                    }
                    
                }
                goto success_peg_1254;
                out_peg_1256:
                goto loop_peg_1252;
                success_peg_1254:
                ;
                result_peg_1240.addResult(result_peg_1253);
            } while (true);
            loop_peg_1252:
            if (result_peg_1240.matches() == 0){
                goto out_peg_1241;
            }
            right = result_peg_1240.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(left,right); save(as<double*>(value));
                result_peg_1240.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_1240;
        stream.update(result_peg_1240.getPosition());
    }
    
    
    return result_peg_1240;
    out_peg_1241:
    Result result_peg_1259(myposition);
    
    {
    
        result_peg_1259.reset();
            do{
                Result result_peg_1263(result_peg_1259.getPosition());
                {
                    
                    char letter_peg_1268 = stream.get(result_peg_1263.getPosition());
                    if (letter_peg_1268 != '\0' && strchr("0123456789", letter_peg_1268) != NULL){
                        result_peg_1263.nextPosition();
                        result_peg_1263.setValue((void*) letter_peg_1268);
                    } else {
                        goto out_peg_1266;
                    }
                    
                }
                goto success_peg_1264;
                out_peg_1266:
                goto loop_peg_1262;
                success_peg_1264:
                ;
                result_peg_1259.addResult(result_peg_1263);
            } while (true);
            loop_peg_1262:
            if (result_peg_1259.matches() == 0){
                goto out_peg_1260;
            }
        
        Result result_peg_1261 = result_peg_1259;
        
        Result result_peg_1271(result_peg_1259);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1271.getPosition()))){
                    result_peg_1271.nextPosition();
                } else {
                    goto not_peg_1270;
                }
            }
            result_peg_1271.setValue((void*) ".");
            goto out_peg_1260;
            not_peg_1270:
            result_peg_1259.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_1261.getValues()); save(as<double*>(value));
                result_peg_1259.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_1259;
        stream.update(result_peg_1259.getPosition());
    }
    
    
    return result_peg_1259;
    out_peg_1260:
    
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

        
