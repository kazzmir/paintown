

#include "../ast/all.h"
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
    Result chunk_whitespace;
    Result chunk_sw;
};

struct Chunk1{
    Result chunk_s;
    Result chunk_section;
    Result chunk_section_line;
    Result chunk_section_start;
    Result chunk_loopstart;
};

struct Chunk2{
    Result chunk_name;
    Result chunk_alpha_digit;
    Result chunk_line_end_or_comment;
    Result chunk_filename;
    Result chunk_filename_char;
};

struct Chunk3{
    Result chunk_attribute;
    Result chunk_identifier;
    Result chunk_identifier_list;
    Result chunk_valuelist;
    Result chunk_value;
};

struct Chunk4{
    Result chunk_keyword;
    Result chunk_date;
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
Result rule_whitespace(Stream &, const int);
Result rule_sw(Stream &, const int);
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
        values.push_back(value);
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

/* garbage collection */
void cleanup(const Value & value){
    SectionList * list = as<SectionList*>(value);
    for (std::list<Ast::Collectable>::iterator it = saved_pointers.begin(); it != saved_pointers.end(); it++){
        Ast::Collectable & collect = *it;
        if (! referenced(list, collect.pointer()) && ! referenced(saved_pointers, collect)){
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
        
        
        
        result_peg_2 = rule_whitespace(stream, result_peg_2.getPosition());
            if (result_peg_2.error()){
                goto out_peg_3;
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
    tail_peg_21:
    
    Result result_peg_22(myposition);
    
    {
    
        result_peg_22 = rule_s(stream, result_peg_22.getPosition());
            if (result_peg_22.error()){
                goto out_peg_23;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_22.getPosition()))){
                                result_peg_22.nextPosition();
                            } else {
                                goto out_peg_27;
                            }
                        }
                        result_peg_22.setValue((void*) ";");
                    
                    
                    
                    result_peg_22.reset();
                        do{
                            Result result_peg_37(result_peg_22.getPosition());
                            {
                            
                                Result result_peg_40(result_peg_37);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_40.getPosition()))){
                                            result_peg_40.nextPosition();
                                        } else {
                                            goto not_peg_39;
                                        }
                                    }
                                    result_peg_40.setValue((void*) "\n");
                                    goto loop_peg_36;
                                    not_peg_39:
                                    result_peg_37.setValue((void*)0);
                                
                                
                                
                                char temp_peg_41 = stream.get(result_peg_37.getPosition());
                                    if (temp_peg_41 != '\0'){
                                        result_peg_37.setValue((void*) temp_peg_41);
                                        result_peg_37.nextPosition();
                                    } else {
                                        goto loop_peg_36;
                                    }
                                
                                
                            }
                            result_peg_22.addResult(result_peg_37);
                        } while (true);
                        loop_peg_36:
                        ;
                    
                    
                }
            }
            goto success_peg_25;
            out_peg_27:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_22.getPosition()))){
                                result_peg_22.nextPosition();
                            } else {
                                goto out_peg_43;
                            }
                        }
                        result_peg_22.setValue((void*) "=");
                    
                    
                    
                    result_peg_22.reset();
                        do{
                            Result result_peg_53(result_peg_22.getPosition());
                            {
                            
                                Result result_peg_56(result_peg_53);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_56.getPosition()))){
                                            result_peg_56.nextPosition();
                                        } else {
                                            goto not_peg_55;
                                        }
                                    }
                                    result_peg_56.setValue((void*) "\n");
                                    goto loop_peg_52;
                                    not_peg_55:
                                    result_peg_53.setValue((void*)0);
                                
                                
                                
                                char temp_peg_57 = stream.get(result_peg_53.getPosition());
                                    if (temp_peg_57 != '\0'){
                                        result_peg_53.setValue((void*) temp_peg_57);
                                        result_peg_53.nextPosition();
                                    } else {
                                        goto loop_peg_52;
                                    }
                                
                                
                            }
                            result_peg_22.addResult(result_peg_53);
                        } while (true);
                        loop_peg_52:
                        ;
                    
                    
                }
            }
            goto success_peg_25;
            out_peg_43:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_22.getPosition()))){
                                result_peg_22.nextPosition();
                            } else {
                                goto out_peg_59;
                            }
                        }
                        result_peg_22.setValue((void*) "-");
                    
                    
                    
                    result_peg_22.reset();
                        do{
                            Result result_peg_65(result_peg_22.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_65.getPosition()))){
                                    result_peg_65.nextPosition();
                                } else {
                                    goto loop_peg_64;
                                }
                            }
                            result_peg_65.setValue((void*) "-");
                            result_peg_22.addResult(result_peg_65);
                        } while (true);
                        loop_peg_64:
                        if (result_peg_22.matches() == 0){
                            goto out_peg_59;
                        }
                    
                    
                }
            }
            goto success_peg_25;
            out_peg_59:
            goto out_peg_23;
            success_peg_25:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_22;
        stream.update(result_peg_22.getPosition());
    }
    
    
    return result_peg_22;
    out_peg_23:
    Result result_peg_66(myposition);
    
    {
    
        result_peg_66 = rule_s(stream, result_peg_66.getPosition());
            if (result_peg_66.error()){
                goto out_peg_67;
            }
        
        
        
        result_peg_66 = rule_section(stream, result_peg_66.getPosition());
            if (result_peg_66.error()){
                goto out_peg_67;
            }
        
        Result result_peg_69 = result_peg_66;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_69.getValues());
                result_peg_66.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_66;
        stream.update(result_peg_66.getPosition());
    }
    
    
    return result_peg_66;
    out_peg_67:
    Result result_peg_70(myposition);
    
    result_peg_70.reset();
    do{
        Result result_peg_73(result_peg_70.getPosition());
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
        result_peg_70.addResult(result_peg_73);
    } while (true);
    loop_peg_72:
    if (result_peg_70.matches() == 0){
        goto out_peg_71;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_70;
        stream.update(result_peg_70.getPosition());
    }
    
    
    return result_peg_70;
    out_peg_71:
    
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
    tail_peg_79:
    
    Result result_peg_80(myposition);
    
    result_peg_80.reset();
    do{
        Result result_peg_83(result_peg_80.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_83.getPosition()))){
                    result_peg_83.nextPosition();
                } else {
                    goto out_peg_86;
                }
            }
            result_peg_83.setValue((void*) "\n");
                
        }
        goto success_peg_84;
        out_peg_86:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_83.getPosition()))){
                    result_peg_83.nextPosition();
                } else {
                    goto out_peg_88;
                }
            }
            result_peg_83.setValue((void*) "\r");
                
        }
        goto success_peg_84;
        out_peg_88:
        goto loop_peg_82;
        success_peg_84:
        ;
        result_peg_80.addResult(result_peg_83);
    } while (true);
    loop_peg_82:
    if (result_peg_80.matches() == 0){
        goto out_peg_81;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_80;
        stream.update(result_peg_80.getPosition());
    }
    
    
    return result_peg_80;
    out_peg_81:
    Result result_peg_89(myposition);
    
    {
    
        Result result_peg_92(result_peg_89.getPosition());
            if ('\0' == stream.get(result_peg_92.getPosition())){
                result_peg_92.nextPosition();
                result_peg_92.setValue((void *) '\0');
            } else {
                goto out_peg_90;
            }
        
        
        
        
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_89;
        stream.update(result_peg_89.getPosition());
    }
    
    
    return result_peg_89;
    out_peg_90:
    
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
        

Result rule_whitespace(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_whitespace.calculated()){
            return column.chunk0->chunk_whitespace;
        }
    }
    
    int myposition = position;
    tail_peg_93:
    
    Result result_peg_94(myposition);
    
    result_peg_94.reset();
    do{
        Result result_peg_97(result_peg_94.getPosition());
        result_peg_97 = rule_sw(stream, result_peg_97.getPosition());
        if (result_peg_97.error()){
            goto loop_peg_96;
        }
        result_peg_94.addResult(result_peg_97);
    } while (true);
    loop_peg_96:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_whitespace = result_peg_94;
        stream.update(result_peg_94.getPosition());
    }
    
    
    return result_peg_94;
    out_peg_95:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_sw.calculated()){
            return column.chunk0->chunk_sw;
        }
    }
    
    int myposition = position;
    tail_peg_98:
    
    Result result_peg_99(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar(" "[i], stream.get(result_peg_99.getPosition()))){
            result_peg_99.nextPosition();
        } else {
            goto out_peg_100;
        }
    }
    result_peg_99.setValue((void*) " ");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_sw = result_peg_99;
        stream.update(result_peg_99.getPosition());
    }
    
    
    return result_peg_99;
    out_peg_100:
    Result result_peg_101(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\t"[i], stream.get(result_peg_101.getPosition()))){
            result_peg_101.nextPosition();
        } else {
            goto out_peg_102;
        }
    }
    result_peg_101.setValue((void*) "\t");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_sw = result_peg_101;
        stream.update(result_peg_101.getPosition());
    }
    
    
    return result_peg_101;
    out_peg_102:
    Result result_peg_103(myposition);
    
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_103.getPosition()))){
                        result_peg_103.nextPosition();
                    } else {
                        goto out_peg_107;
                    }
                }
                result_peg_103.setValue((void*) ";");
            
            
            
            result_peg_103.reset();
                do{
                    Result result_peg_117(result_peg_103.getPosition());
                    {
                    
                        Result result_peg_120(result_peg_117);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_120.getPosition()))){
                                    result_peg_120.nextPosition();
                                } else {
                                    goto not_peg_119;
                                }
                            }
                            result_peg_120.setValue((void*) "\n");
                            goto loop_peg_116;
                            not_peg_119:
                            result_peg_117.setValue((void*)0);
                        
                        
                        
                        char temp_peg_121 = stream.get(result_peg_117.getPosition());
                            if (temp_peg_121 != '\0'){
                                result_peg_117.setValue((void*) temp_peg_121);
                                result_peg_117.nextPosition();
                            } else {
                                goto loop_peg_116;
                            }
                        
                        
                    }
                    result_peg_103.addResult(result_peg_117);
                } while (true);
                loop_peg_116:
                ;
            
            
        }
    }
    goto success_peg_105;
    out_peg_107:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_103.getPosition()))){
                        result_peg_103.nextPosition();
                    } else {
                        goto out_peg_123;
                    }
                }
                result_peg_103.setValue((void*) "=");
            
            
            
            result_peg_103.reset();
                do{
                    Result result_peg_133(result_peg_103.getPosition());
                    {
                    
                        Result result_peg_136(result_peg_133);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_136.getPosition()))){
                                    result_peg_136.nextPosition();
                                } else {
                                    goto not_peg_135;
                                }
                            }
                            result_peg_136.setValue((void*) "\n");
                            goto loop_peg_132;
                            not_peg_135:
                            result_peg_133.setValue((void*)0);
                        
                        
                        
                        char temp_peg_137 = stream.get(result_peg_133.getPosition());
                            if (temp_peg_137 != '\0'){
                                result_peg_133.setValue((void*) temp_peg_137);
                                result_peg_133.nextPosition();
                            } else {
                                goto loop_peg_132;
                            }
                        
                        
                    }
                    result_peg_103.addResult(result_peg_133);
                } while (true);
                loop_peg_132:
                ;
            
            
        }
    }
    goto success_peg_105;
    out_peg_123:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_103.getPosition()))){
                        result_peg_103.nextPosition();
                    } else {
                        goto out_peg_139;
                    }
                }
                result_peg_103.setValue((void*) "-");
            
            
            
            result_peg_103.reset();
                do{
                    Result result_peg_145(result_peg_103.getPosition());
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_145.getPosition()))){
                            result_peg_145.nextPosition();
                        } else {
                            goto loop_peg_144;
                        }
                    }
                    result_peg_145.setValue((void*) "-");
                    result_peg_103.addResult(result_peg_145);
                } while (true);
                loop_peg_144:
                if (result_peg_103.matches() == 0){
                    goto out_peg_139;
                }
            
            
        }
    }
    goto success_peg_105;
    out_peg_139:
    goto out_peg_104;
    success_peg_105:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_sw = result_peg_103;
        stream.update(result_peg_103.getPosition());
    }
    
    
    return result_peg_103;
    out_peg_104:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_s.calculated()){
            return column.chunk1->chunk_s;
        }
    }
    
    int myposition = position;
    tail_peg_146:
    
    Result result_peg_147(myposition);
    
    result_peg_147.reset();
    do{
        Result result_peg_150(result_peg_147.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_150.getPosition()))){
                    result_peg_150.nextPosition();
                } else {
                    goto out_peg_153;
                }
            }
            result_peg_150.setValue((void*) " ");
                
        }
        goto success_peg_151;
        out_peg_153:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_150.getPosition()))){
                    result_peg_150.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_150.setValue((void*) "\t");
                
        }
        goto success_peg_151;
        out_peg_155:
        goto loop_peg_149;
        success_peg_151:
        ;
        result_peg_147.addResult(result_peg_150);
    } while (true);
    loop_peg_149:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_s = result_peg_147;
        stream.update(result_peg_147.getPosition());
    }
    
    
    return result_peg_147;
    out_peg_148:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_s = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_section.calculated()){
            return column.chunk1->chunk_section;
        }
    }
    
    int myposition = position;
    tail_peg_156:
    Value ast;
    Result result_peg_157(myposition);
    
    {
    
        result_peg_157 = rule_section_start(stream, result_peg_157.getPosition());
            if (result_peg_157.error()){
                goto out_peg_158;
            }
        
        Result result_peg_159 = result_peg_157;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_159.getValues()); save(as<Ast::Section*>(value));
                result_peg_157.setValue(value);
            }
            ast = result_peg_157.getValues();
        
        
        
        result_peg_157 = rule_whitespace(stream, result_peg_157.getPosition());
            if (result_peg_157.error()){
                goto out_peg_158;
            }
        
        
        
        result_peg_157.reset();
            do{
                Result result_peg_164(result_peg_157.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_164.getPosition()))){
                            result_peg_164.nextPosition();
                        } else {
                            goto out_peg_167;
                        }
                    }
                    result_peg_164.setValue((void*) "\n");
                        
                }
                goto success_peg_165;
                out_peg_167:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_164.getPosition()))){
                            result_peg_164.nextPosition();
                        } else {
                            goto out_peg_169;
                        }
                    }
                    result_peg_164.setValue((void*) "\r");
                        
                }
                goto success_peg_165;
                out_peg_169:
                goto loop_peg_163;
                success_peg_165:
                ;
                result_peg_157.addResult(result_peg_164);
            } while (true);
            loop_peg_163:
            if (result_peg_157.matches() == 0){
                goto out_peg_158;
            }
        
        
        
        result_peg_157.reset();
            do{
                Result result_peg_172(result_peg_157.getPosition());
                {
                
                    result_peg_172 = rule_section_line(stream, result_peg_172.getPosition(), ast);
                        if (result_peg_172.error()){
                            goto loop_peg_171;
                        }
                    
                    
                    
                    result_peg_172 = rule_whitespace(stream, result_peg_172.getPosition());
                        if (result_peg_172.error()){
                            goto loop_peg_171;
                        }
                    
                    
                    
                    result_peg_172 = rule_line_end(stream, result_peg_172.getPosition());
                        if (result_peg_172.error()){
                            goto loop_peg_171;
                        }
                    
                    
                }
                result_peg_157.addResult(result_peg_172);
            } while (true);
            loop_peg_171:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_157.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section = result_peg_157;
        stream.update(result_peg_157.getPosition());
    }
    
    
    return result_peg_157;
    out_peg_158:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section = errorResult;
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
    tail_peg_175:
    Value data;
    Result result_peg_176(myposition);
    
    {
    
        result_peg_176 = rule_s(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_177;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_176.getPosition()))){
                                result_peg_176.nextPosition();
                            } else {
                                goto out_peg_181;
                            }
                        }
                        result_peg_176.setValue((void*) ";");
                    
                    
                    
                    result_peg_176.reset();
                        do{
                            Result result_peg_191(result_peg_176.getPosition());
                            {
                            
                                Result result_peg_194(result_peg_191);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_194.getPosition()))){
                                            result_peg_194.nextPosition();
                                        } else {
                                            goto not_peg_193;
                                        }
                                    }
                                    result_peg_194.setValue((void*) "\n");
                                    goto loop_peg_190;
                                    not_peg_193:
                                    result_peg_191.setValue((void*)0);
                                
                                
                                
                                char temp_peg_195 = stream.get(result_peg_191.getPosition());
                                    if (temp_peg_195 != '\0'){
                                        result_peg_191.setValue((void*) temp_peg_195);
                                        result_peg_191.nextPosition();
                                    } else {
                                        goto loop_peg_190;
                                    }
                                
                                
                            }
                            result_peg_176.addResult(result_peg_191);
                        } while (true);
                        loop_peg_190:
                        ;
                    
                    
                }
            }
            goto success_peg_179;
            out_peg_181:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_176.getPosition()))){
                                result_peg_176.nextPosition();
                            } else {
                                goto out_peg_197;
                            }
                        }
                        result_peg_176.setValue((void*) "=");
                    
                    
                    
                    result_peg_176.reset();
                        do{
                            Result result_peg_207(result_peg_176.getPosition());
                            {
                            
                                Result result_peg_210(result_peg_207);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_210.getPosition()))){
                                            result_peg_210.nextPosition();
                                        } else {
                                            goto not_peg_209;
                                        }
                                    }
                                    result_peg_210.setValue((void*) "\n");
                                    goto loop_peg_206;
                                    not_peg_209:
                                    result_peg_207.setValue((void*)0);
                                
                                
                                
                                char temp_peg_211 = stream.get(result_peg_207.getPosition());
                                    if (temp_peg_211 != '\0'){
                                        result_peg_207.setValue((void*) temp_peg_211);
                                        result_peg_207.nextPosition();
                                    } else {
                                        goto loop_peg_206;
                                    }
                                
                                
                            }
                            result_peg_176.addResult(result_peg_207);
                        } while (true);
                        loop_peg_206:
                        ;
                    
                    
                }
            }
            goto success_peg_179;
            out_peg_197:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_176.getPosition()))){
                                result_peg_176.nextPosition();
                            } else {
                                goto out_peg_213;
                            }
                        }
                        result_peg_176.setValue((void*) "-");
                    
                    
                    
                    result_peg_176.reset();
                        do{
                            Result result_peg_219(result_peg_176.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_219.getPosition()))){
                                    result_peg_219.nextPosition();
                                } else {
                                    goto loop_peg_218;
                                }
                            }
                            result_peg_219.setValue((void*) "-");
                            result_peg_176.addResult(result_peg_219);
                        } while (true);
                        loop_peg_218:
                        if (result_peg_176.matches() == 0){
                            goto out_peg_213;
                        }
                    
                    
                }
            }
            goto success_peg_179;
            out_peg_213:
            goto out_peg_177;
            success_peg_179:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_176;
        stream.update(result_peg_176.getPosition());
    }
    
    
    return result_peg_176;
    out_peg_177:
    Result result_peg_220(myposition);
    
    {
    
        result_peg_220 = rule_s(stream, result_peg_220.getPosition());
            if (result_peg_220.error()){
                goto out_peg_221;
            }
        
        
        
        result_peg_220 = rule_attribute(stream, result_peg_220.getPosition());
            if (result_peg_220.error()){
                goto out_peg_221;
            }
            data = result_peg_220.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addAttribute(asAttribute(data));
                result_peg_220.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_220;
        stream.update(result_peg_220.getPosition());
    }
    
    
    return result_peg_220;
    out_peg_221:
    Result result_peg_224(myposition);
    
    {
    
        result_peg_224 = rule_s(stream, result_peg_224.getPosition());
            if (result_peg_224.error()){
                goto out_peg_225;
            }
        
        
        
        result_peg_224 = rule_valuelist(stream, result_peg_224.getPosition());
            if (result_peg_224.error()){
                goto out_peg_225;
            }
            data = result_peg_224.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_224.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_224;
        stream.update(result_peg_224.getPosition());
    }
    
    
    return result_peg_224;
    out_peg_225:
    Result result_peg_228(myposition);
    
    {
    
        result_peg_228 = rule_s(stream, result_peg_228.getPosition());
            if (result_peg_228.error()){
                goto out_peg_229;
            }
        
        
        
        result_peg_228 = rule_loopstart(stream, result_peg_228.getPosition());
            if (result_peg_228.error()){
                goto out_peg_229;
            }
            data = result_peg_228.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_228.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_228;
        stream.update(result_peg_228.getPosition());
    }
    
    
    return result_peg_228;
    out_peg_229:
    Result result_peg_232(myposition);
    
    {
    
        result_peg_232.reset();
            do{
                Result result_peg_236(result_peg_232.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_236.getPosition()))){
                            result_peg_236.nextPosition();
                        } else {
                            goto out_peg_239;
                        }
                    }
                    result_peg_236.setValue((void*) " ");
                        
                }
                goto success_peg_237;
                out_peg_239:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_236.getPosition()))){
                            result_peg_236.nextPosition();
                        } else {
                            goto out_peg_241;
                        }
                    }
                    result_peg_236.setValue((void*) "\t");
                        
                }
                goto success_peg_237;
                out_peg_241:
                goto loop_peg_235;
                success_peg_237:
                ;
                result_peg_232.addResult(result_peg_236);
            } while (true);
            loop_peg_235:
            if (result_peg_232.matches() == 0){
                goto out_peg_233;
            }
        
        
        
        Result result_peg_243(result_peg_232);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_243.getPosition()))){
                    result_peg_243.nextPosition();
                } else {
                    goto not_peg_242;
                }
            }
            result_peg_243.setValue((void*) "[");
            goto out_peg_233;
            not_peg_242:
            result_peg_232.setValue((void*)0);
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_232;
        stream.update(result_peg_232.getPosition());
    }
    
    
    return result_peg_232;
    out_peg_233:
    
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
    tail_peg_244:
    Value data;
    Result result_peg_245(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_245.getPosition()))){
                    result_peg_245.nextPosition();
                } else {
                    goto out_peg_246;
                }
            }
            result_peg_245.setValue((void*) "[");
        
        
        
        result_peg_245 = rule_s(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_246;
            }
        
        
        
        result_peg_245.reset();
            do{
                Result result_peg_251(result_peg_245.getPosition());
                {
                
                    Result result_peg_254(result_peg_251);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_254.getPosition()))){
                                result_peg_254.nextPosition();
                            } else {
                                goto not_peg_253;
                            }
                        }
                        result_peg_254.setValue((void*) "]");
                        goto loop_peg_250;
                        not_peg_253:
                        result_peg_251.setValue((void*)0);
                    
                    
                    
                    char temp_peg_255 = stream.get(result_peg_251.getPosition());
                        if (temp_peg_255 != '\0'){
                            result_peg_251.setValue((void*) temp_peg_255);
                            result_peg_251.nextPosition();
                        } else {
                            goto loop_peg_250;
                        }
                    
                    
                }
                result_peg_245.addResult(result_peg_251);
            } while (true);
            loop_peg_250:
            if (result_peg_245.matches() == 0){
                goto out_peg_246;
            }
            data = result_peg_245.getValues();
        
        
        
        result_peg_245 = rule_s(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_246;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_245.getPosition()))){
                    result_peg_245.nextPosition();
                } else {
                    goto out_peg_246;
                }
            }
            result_peg_245.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(data); save(as<std::string*>(value));
                result_peg_245.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_start = result_peg_245;
        stream.update(result_peg_245.getPosition());
    }
    
    
    return result_peg_245;
    out_peg_246:
    
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
    tail_peg_258:
    
    Result result_peg_259(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_259.getPosition()))){
                    result_peg_259.nextPosition();
                } else {
                    goto out_peg_260;
                }
            }
            result_peg_259.setValue((void*) "loopstart");
        
        
        
        {
                Value value((void*) 0);
                value = makeValue(); save(as<Ast::Value*>(value));
                result_peg_259.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_loopstart = result_peg_259;
        stream.update(result_peg_259.getPosition());
    }
    
    
    return result_peg_259;
    out_peg_260:
    
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
        if (column.chunk2 != 0 && column.chunk2->chunk_name.calculated()){
            return column.chunk2->chunk_name;
        }
    }
    
    int myposition = position;
    tail_peg_262:
    
    Result result_peg_263(myposition);
    
    {
    
        {
                
                char letter_peg_270 = stream.get(result_peg_263.getPosition());
                if (letter_peg_270 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_270) != NULL){
                    result_peg_263.nextPosition();
                    result_peg_263.setValue((void*) letter_peg_270);
                } else {
                    goto out_peg_268;
                }
                
            }
            goto success_peg_266;
            out_peg_268:
            goto out_peg_264;
            success_peg_266:
            ;
        
        Result result_peg_265 = result_peg_263;
        
        result_peg_263.reset();
            do{
                Result result_peg_273(result_peg_263.getPosition());
                result_peg_273 = rule_alpha_digit(stream, result_peg_273.getPosition());
                if (result_peg_273.error()){
                    goto loop_peg_272;
                }
                result_peg_263.addResult(result_peg_273);
            } while (true);
            loop_peg_272:
            ;
        
        Result result_peg_271 = result_peg_263;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_265.getValues().getValue(),result_peg_271.getValues()); save(as<std::string*>(value));
                result_peg_263.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_name = result_peg_263;
        stream.update(result_peg_263.getPosition());
    }
    
    
    return result_peg_263;
    out_peg_264:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_alpha_digit.calculated()){
            return column.chunk2->chunk_alpha_digit;
        }
    }
    
    int myposition = position;
    tail_peg_274:
    
    Result result_peg_275(myposition);
    
    {
        
        char letter_peg_281 = stream.get(result_peg_275.getPosition());
        if (letter_peg_281 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_281) != NULL){
            result_peg_275.nextPosition();
            result_peg_275.setValue((void*) letter_peg_281);
        } else {
            goto out_peg_279;
        }
        
    }
    goto success_peg_277;
    out_peg_279:
    goto out_peg_276;
    success_peg_277:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_alpha_digit = result_peg_275;
        stream.update(result_peg_275.getPosition());
    }
    
    
    return result_peg_275;
    out_peg_276:
    Result result_peg_282(myposition);
    
    {
        
        char letter_peg_288 = stream.get(result_peg_282.getPosition());
        if (letter_peg_288 != '\0' && strchr("0123456789", letter_peg_288) != NULL){
            result_peg_282.nextPosition();
            result_peg_282.setValue((void*) letter_peg_288);
        } else {
            goto out_peg_286;
        }
        
    }
    goto success_peg_284;
    out_peg_286:
    goto out_peg_283;
    success_peg_284:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_alpha_digit = result_peg_282;
        stream.update(result_peg_282.getPosition());
    }
    
    
    return result_peg_282;
    out_peg_283:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_alpha_digit = errorResult;
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
    tail_peg_289:
    
    Result result_peg_290(myposition);
    
    result_peg_290 = rule_line_end(stream, result_peg_290.getPosition());
    if (result_peg_290.error()){
        goto out_peg_291;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = result_peg_290;
        stream.update(result_peg_290.getPosition());
    }
    
    
    return result_peg_290;
    out_peg_291:
    Result result_peg_292(myposition);
    
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_292.getPosition()))){
                        result_peg_292.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
                result_peg_292.setValue((void*) ";");
            
            
            
            result_peg_292.reset();
                do{
                    Result result_peg_306(result_peg_292.getPosition());
                    {
                    
                        Result result_peg_309(result_peg_306);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_309.getPosition()))){
                                    result_peg_309.nextPosition();
                                } else {
                                    goto not_peg_308;
                                }
                            }
                            result_peg_309.setValue((void*) "\n");
                            goto loop_peg_305;
                            not_peg_308:
                            result_peg_306.setValue((void*)0);
                        
                        
                        
                        char temp_peg_310 = stream.get(result_peg_306.getPosition());
                            if (temp_peg_310 != '\0'){
                                result_peg_306.setValue((void*) temp_peg_310);
                                result_peg_306.nextPosition();
                            } else {
                                goto loop_peg_305;
                            }
                        
                        
                    }
                    result_peg_292.addResult(result_peg_306);
                } while (true);
                loop_peg_305:
                ;
            
            
        }
    }
    goto success_peg_294;
    out_peg_296:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_292.getPosition()))){
                        result_peg_292.nextPosition();
                    } else {
                        goto out_peg_312;
                    }
                }
                result_peg_292.setValue((void*) "=");
            
            
            
            result_peg_292.reset();
                do{
                    Result result_peg_322(result_peg_292.getPosition());
                    {
                    
                        Result result_peg_325(result_peg_322);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_325.getPosition()))){
                                    result_peg_325.nextPosition();
                                } else {
                                    goto not_peg_324;
                                }
                            }
                            result_peg_325.setValue((void*) "\n");
                            goto loop_peg_321;
                            not_peg_324:
                            result_peg_322.setValue((void*)0);
                        
                        
                        
                        char temp_peg_326 = stream.get(result_peg_322.getPosition());
                            if (temp_peg_326 != '\0'){
                                result_peg_322.setValue((void*) temp_peg_326);
                                result_peg_322.nextPosition();
                            } else {
                                goto loop_peg_321;
                            }
                        
                        
                    }
                    result_peg_292.addResult(result_peg_322);
                } while (true);
                loop_peg_321:
                ;
            
            
        }
    }
    goto success_peg_294;
    out_peg_312:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_292.getPosition()))){
                        result_peg_292.nextPosition();
                    } else {
                        goto out_peg_328;
                    }
                }
                result_peg_292.setValue((void*) "-");
            
            
            
            result_peg_292.reset();
                do{
                    Result result_peg_334(result_peg_292.getPosition());
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_334.getPosition()))){
                            result_peg_334.nextPosition();
                        } else {
                            goto loop_peg_333;
                        }
                    }
                    result_peg_334.setValue((void*) "-");
                    result_peg_292.addResult(result_peg_334);
                } while (true);
                loop_peg_333:
                if (result_peg_292.matches() == 0){
                    goto out_peg_328;
                }
            
            
        }
    }
    goto success_peg_294;
    out_peg_328:
    goto out_peg_293;
    success_peg_294:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = result_peg_292;
        stream.update(result_peg_292.getPosition());
    }
    
    
    return result_peg_292;
    out_peg_293:
    
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
    tail_peg_335:
    Value file;
    Result result_peg_336(myposition);
    
    {
    
        Result result_peg_340(result_peg_336);
            for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_340.getPosition()))){
                    result_peg_340.nextPosition();
                } else {
                    goto not_peg_339;
                }
            }
            result_peg_340.setValue((void*) "\"");
            goto out_peg_337;
            not_peg_339:
            result_peg_336.setValue((void*)0);
        
        
        
        result_peg_336.reset();
            do{
                Result result_peg_343(result_peg_336.getPosition());
                result_peg_343 = rule_filename_char(stream, result_peg_343.getPosition());
                if (result_peg_343.error()){
                    goto loop_peg_342;
                }
                result_peg_336.addResult(result_peg_343);
            } while (true);
            loop_peg_342:
            if (result_peg_336.matches() == 0){
                goto out_peg_337;
            }
            file = result_peg_336.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new Ast::Filename(toString(file)); save(as<Ast::Filename*>(value));
                result_peg_336.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename = result_peg_336;
        stream.update(result_peg_336.getPosition());
    }
    
    
    return result_peg_336;
    out_peg_337:
    
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
    tail_peg_344:
    
    Result result_peg_345(myposition);
    
    {
    
        Result result_peg_349(result_peg_345);
            for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_349.getPosition()))){
                    result_peg_349.nextPosition();
                } else {
                    goto not_peg_348;
                }
            }
            result_peg_349.setValue((void*) ",");
            goto out_peg_346;
            not_peg_348:
            result_peg_345.setValue((void*)0);
        
        
        
        Result result_peg_352(result_peg_345);
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_352.getPosition()))){
                    result_peg_352.nextPosition();
                } else {
                    goto not_peg_351;
                }
            }
            result_peg_352.setValue((void*) "\n");
            goto out_peg_346;
            not_peg_351:
            result_peg_345.setValue((void*)0);
        
        
        
        Result result_peg_355(result_peg_345);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_355.getPosition()))){
                    result_peg_355.nextPosition();
                } else {
                    goto not_peg_354;
                }
            }
            result_peg_355.setValue((void*) "[");
            goto out_peg_346;
            not_peg_354:
            result_peg_345.setValue((void*)0);
        
        
        
        Result result_peg_358(result_peg_345);
            if ((unsigned char) stream.get(result_peg_358.getPosition()) == (unsigned char) 13){
                result_peg_358.nextPosition();
            } else {
                goto not_peg_357;
            }
            result_peg_358.setValue((void*) 13);
            goto out_peg_346;
            not_peg_357:
            result_peg_345.setValue((void*)0);
        
        
        
        char temp_peg_359 = stream.get(result_peg_345.getPosition());
            if (temp_peg_359 != '\0'){
                result_peg_345.setValue((void*) temp_peg_359);
                result_peg_345.nextPosition();
            } else {
                goto out_peg_346;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename_char = result_peg_345;
        stream.update(result_peg_345.getPosition());
    }
    
    
    return result_peg_345;
    out_peg_346:
    
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
        if (column.chunk3 != 0 && column.chunk3->chunk_attribute.calculated()){
            return column.chunk3->chunk_attribute;
        }
    }
    
    int myposition = position;
    tail_peg_360:
    Value id;
    Value data;
    Value index;
    Result result_peg_361(myposition);
    
    {
    
        result_peg_361 = rule_identifier(stream, result_peg_361.getPosition());
            if (result_peg_361.error()){
                goto out_peg_362;
            }
            id = result_peg_361.getValues();
        
        
        
        result_peg_361 = rule_s(stream, result_peg_361.getPosition());
            if (result_peg_361.error()){
                goto out_peg_362;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_361.getPosition()))){
                    result_peg_361.nextPosition();
                } else {
                    goto out_peg_362;
                }
            }
            result_peg_361.setValue((void*) "=");
        
        
        
        result_peg_361 = rule_s(stream, result_peg_361.getPosition());
            if (result_peg_361.error()){
                goto out_peg_362;
            }
        
        
        
        Result result_peg_368(result_peg_361.getPosition());
            result_peg_368 = rule_line_end_or_comment(stream, result_peg_368.getPosition());
            if (result_peg_368.error()){
                goto out_peg_362;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id); save(as<Ast::Attribute*>(value));
                result_peg_361.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_attribute = result_peg_361;
        stream.update(result_peg_361.getPosition());
    }
    
    
    return result_peg_361;
    out_peg_362:
    Result result_peg_369(myposition);
    
    {
    
        result_peg_369 = rule_identifier(stream, result_peg_369.getPosition());
            if (result_peg_369.error()){
                goto out_peg_370;
            }
            id = result_peg_369.getValues();
        
        
        
        result_peg_369 = rule_s(stream, result_peg_369.getPosition());
            if (result_peg_369.error()){
                goto out_peg_370;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_369.getPosition()))){
                    result_peg_369.nextPosition();
                } else {
                    goto out_peg_370;
                }
            }
            result_peg_369.setValue((void*) "=");
        
        
        
        result_peg_369 = rule_s(stream, result_peg_369.getPosition());
            if (result_peg_369.error()){
                goto out_peg_370;
            }
        
        
        
        result_peg_369 = rule_valuelist(stream, result_peg_369.getPosition());
            if (result_peg_369.error()){
                goto out_peg_370;
            }
            data = result_peg_369.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id, data); save(as<Ast::Attribute*>(value));
                result_peg_369.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_attribute = result_peg_369;
        stream.update(result_peg_369.getPosition());
    }
    
    
    return result_peg_369;
    out_peg_370:
    Result result_peg_376(myposition);
    
    {
    
        result_peg_376 = rule_identifier(stream, result_peg_376.getPosition());
            if (result_peg_376.error()){
                goto out_peg_377;
            }
            id = result_peg_376.getValues();
        
        
        
        result_peg_376 = rule_s(stream, result_peg_376.getPosition());
            if (result_peg_376.error()){
                goto out_peg_377;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_376.getPosition()))){
                    result_peg_376.nextPosition();
                } else {
                    goto out_peg_377;
                }
            }
            result_peg_376.setValue((void*) "=");
        
        
        
        result_peg_376 = rule_s(stream, result_peg_376.getPosition());
            if (result_peg_376.error()){
                goto out_peg_377;
            }
        
        
        
        result_peg_376 = rule_filename(stream, result_peg_376.getPosition());
            if (result_peg_376.error()){
                goto out_peg_377;
            }
            data = result_peg_376.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeFilename(id, data); save(as<Ast::Attribute*>(value));
                result_peg_376.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_attribute = result_peg_376;
        stream.update(result_peg_376.getPosition());
    }
    
    
    return result_peg_376;
    out_peg_377:
    Result result_peg_383(myposition);
    
    {
    
        result_peg_383 = rule_identifier(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
            id = result_peg_383.getValues();
        
        
        
        result_peg_383 = rule_s(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_383.getPosition()))){
                    result_peg_383.nextPosition();
                } else {
                    goto out_peg_384;
                }
            }
            result_peg_383.setValue((void*) "(");
        
        
        
        result_peg_383 = rule_s(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
        
        
        
        result_peg_383 = rule_number(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
            index = result_peg_383.getValues();
        
        
        
        result_peg_383 = rule_s(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_383.getPosition()))){
                    result_peg_383.nextPosition();
                } else {
                    goto out_peg_384;
                }
            }
            result_peg_383.setValue((void*) ")");
        
        
        
        result_peg_383 = rule_s(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_383.getPosition()))){
                    result_peg_383.nextPosition();
                } else {
                    goto out_peg_384;
                }
            }
            result_peg_383.setValue((void*) "=");
        
        
        
        result_peg_383 = rule_s(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
        
        
        
        result_peg_383 = rule_valuelist(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_384;
            }
            data = result_peg_383.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(id, index, data); save(as<Ast::Value*>(value));
                result_peg_383.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_attribute = result_peg_383;
        stream.update(result_peg_383.getPosition());
    }
    
    
    return result_peg_383;
    out_peg_384:
    Result result_peg_396(myposition);
    
    {
    
        result_peg_396 = rule_identifier_list(stream, result_peg_396.getPosition());
            if (result_peg_396.error()){
                goto out_peg_397;
            }
            id = result_peg_396.getValues();
        
        
        
        result_peg_396 = rule_s(stream, result_peg_396.getPosition());
            if (result_peg_396.error()){
                goto out_peg_397;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_396.getPosition()))){
                    result_peg_396.nextPosition();
                } else {
                    goto out_peg_397;
                }
            }
            result_peg_396.setValue((void*) "=");
        
        
        
        result_peg_396 = rule_s(stream, result_peg_396.getPosition());
            if (result_peg_396.error()){
                goto out_peg_397;
            }
        
        
        
        result_peg_396 = rule_valuelist(stream, result_peg_396.getPosition());
            if (result_peg_396.error()){
                goto out_peg_397;
            }
            data = result_peg_396.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributes(id, data); save(as<Ast::Attribute*>(value));
                result_peg_396.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_attribute = result_peg_396;
        stream.update(result_peg_396.getPosition());
    }
    
    
    return result_peg_396;
    out_peg_397:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_attribute = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_identifier.calculated()){
            return column.chunk3->chunk_identifier;
        }
    }
    
    int myposition = position;
    tail_peg_403:
    
    Result result_peg_404(myposition);
    
    {
    
        result_peg_404 = rule_name(stream, result_peg_404.getPosition());
            if (result_peg_404.error()){
                goto out_peg_405;
            }
        
        Result result_peg_406 = result_peg_404;
        
        result_peg_404.reset();
            do{
                Result result_peg_409(result_peg_404.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_409.getPosition()))){
                                result_peg_409.nextPosition();
                            } else {
                                goto loop_peg_408;
                            }
                        }
                        result_peg_409.setValue((void*) ".");
                    
                    
                    
                    result_peg_409 = rule_name(stream, result_peg_409.getPosition());
                        if (result_peg_409.error()){
                            goto loop_peg_408;
                        }
                    
                    
                }
                result_peg_404.addResult(result_peg_409);
            } while (true);
            loop_peg_408:
            ;
        
        Result result_peg_407 = result_peg_404;
        
        {
                Value value((void*) 0);
                value = makeIdentifier(result_peg_406.getValues(),result_peg_407.getValues()); save(as<Ast::Identifier*>(value));
                result_peg_404.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier = result_peg_404;
        stream.update(result_peg_404.getPosition());
    }
    
    
    return result_peg_404;
    out_peg_405:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier = errorResult;
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
    tail_peg_411:
    
    Result result_peg_412(myposition);
    
    {
    
        result_peg_412 = rule_identifier(stream, result_peg_412.getPosition());
            if (result_peg_412.error()){
                goto out_peg_413;
            }
        
        
        
        result_peg_412.reset();
            do{
                Result result_peg_416(result_peg_412.getPosition());
                {
                
                    result_peg_416 = rule_s(stream, result_peg_416.getPosition());
                        if (result_peg_416.error()){
                            goto loop_peg_415;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_416.getPosition()))){
                                result_peg_416.nextPosition();
                            } else {
                                goto loop_peg_415;
                            }
                        }
                        result_peg_416.setValue((void*) ",");
                    
                    
                    
                    result_peg_416 = rule_s(stream, result_peg_416.getPosition());
                        if (result_peg_416.error()){
                            goto loop_peg_415;
                        }
                    
                    
                    
                    result_peg_416 = rule_filename(stream, result_peg_416.getPosition());
                        if (result_peg_416.error()){
                            goto loop_peg_415;
                        }
                    
                    
                }
                result_peg_412.addResult(result_peg_416);
            } while (true);
            loop_peg_415:
            if (result_peg_412.matches() == 0){
                goto out_peg_413;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier_list = result_peg_412;
        stream.update(result_peg_412.getPosition());
    }
    
    
    return result_peg_412;
    out_peg_413:
    
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
    tail_peg_420:
    
    Result result_peg_421(myposition);
    
    {
    
        result_peg_421 = rule_value(stream, result_peg_421.getPosition());
            if (result_peg_421.error()){
                goto out_peg_422;
            }
        
        Result result_peg_423 = result_peg_421;
        
        result_peg_421.reset();
            do{
                Result result_peg_426(result_peg_421.getPosition());
                {
                
                    result_peg_426 = rule_s(stream, result_peg_426.getPosition());
                        if (result_peg_426.error()){
                            goto loop_peg_425;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_426.getPosition()))){
                                result_peg_426.nextPosition();
                            } else {
                                goto loop_peg_425;
                            }
                        }
                        result_peg_426.setValue((void*) ",");
                    
                    
                    
                    result_peg_426 = rule_s(stream, result_peg_426.getPosition());
                        if (result_peg_426.error()){
                            goto loop_peg_425;
                        }
                    
                    
                    
                    int save_peg_430 = result_peg_426.getPosition();
                        
                        result_peg_426 = rule_value(stream, result_peg_426.getPosition());
                        if (result_peg_426.error()){
                            
                            result_peg_426 = Result(save_peg_430);
                            result_peg_426.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_421.addResult(result_peg_426);
            } while (true);
            loop_peg_425:
            ;
        
        Result result_peg_424 = result_peg_421;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_423.getValues(),result_peg_424.getValues()); save(as<Ast::Value*>(value));
                result_peg_421.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = result_peg_421;
        stream.update(result_peg_421.getPosition());
    }
    
    
    return result_peg_421;
    out_peg_422:
    
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
    tail_peg_431:
    
    Result result_peg_432(myposition);
    
    result_peg_432 = rule_string(stream, result_peg_432.getPosition());
    if (result_peg_432.error()){
        goto out_peg_433;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_432;
        stream.update(result_peg_432.getPosition());
    }
    
    
    return result_peg_432;
    out_peg_433:
    Result result_peg_434(myposition);
    
    {
    
        result_peg_434 = rule_date(stream, result_peg_434.getPosition());
            if (result_peg_434.error()){
                goto out_peg_435;
            }
        
        
        
        {
                Value value((void*) 0);
                value = 0;
                result_peg_434.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_434;
        stream.update(result_peg_434.getPosition());
    }
    
    
    return result_peg_434;
    out_peg_435:
    Result result_peg_437(myposition);
    
    result_peg_437 = rule_number(stream, result_peg_437.getPosition());
    if (result_peg_437.error()){
        goto out_peg_438;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_437;
        stream.update(result_peg_437.getPosition());
    }
    
    
    return result_peg_437;
    out_peg_438:
    Result result_peg_439(myposition);
    
    {
    
        result_peg_439 = rule_keyword(stream, result_peg_439.getPosition());
            if (result_peg_439.error()){
                goto out_peg_440;
            }
        
        Result result_peg_441 = result_peg_439;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_441.getValues()); save(as<Ast::Value*>(value));
                result_peg_439.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_439;
        stream.update(result_peg_439.getPosition());
    }
    
    
    return result_peg_439;
    out_peg_440:
    Result result_peg_442(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_442.getPosition()))){
                    result_peg_442.nextPosition();
                } else {
                    goto out_peg_443;
                }
            }
            result_peg_442.setValue((void*) "s");
        
        
        
        Result result_peg_447(result_peg_442);
            {
                
                char letter_peg_452 = stream.get(result_peg_447.getPosition());
                if (letter_peg_452 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_452) != NULL){
                    result_peg_447.nextPosition();
                    result_peg_447.setValue((void*) letter_peg_452);
                } else {
                    goto out_peg_450;
                }
                
            }
            goto success_peg_448;
            out_peg_450:
            goto not_peg_446;
            success_peg_448:
            ;
            goto out_peg_443;
            not_peg_446:
            result_peg_442.setValue((void*)0);
        
        
        
        Result result_peg_455(result_peg_442);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_455.getPosition()))){
                    result_peg_455.nextPosition();
                } else {
                    goto not_peg_454;
                }
            }
            result_peg_455.setValue((void*) ".");
            goto out_peg_443;
            not_peg_454:
            result_peg_442.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = 0;
                result_peg_442.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_442;
        stream.update(result_peg_442.getPosition());
    }
    
    
    return result_peg_442;
    out_peg_443:
    Result result_peg_456(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_456.getPosition()))){
                    result_peg_456.nextPosition();
                } else {
                    goto out_peg_457;
                }
            }
            result_peg_456.setValue((void*) "h");
        
        
        
        Result result_peg_461(result_peg_456);
            {
                
                char letter_peg_466 = stream.get(result_peg_461.getPosition());
                if (letter_peg_466 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_466) != NULL){
                    result_peg_461.nextPosition();
                    result_peg_461.setValue((void*) letter_peg_466);
                } else {
                    goto out_peg_464;
                }
                
            }
            goto success_peg_462;
            out_peg_464:
            goto not_peg_460;
            success_peg_462:
            ;
            goto out_peg_457;
            not_peg_460:
            result_peg_456.setValue((void*)0);
        
        
        
        Result result_peg_469(result_peg_456);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_469.getPosition()))){
                    result_peg_469.nextPosition();
                } else {
                    goto not_peg_468;
                }
            }
            result_peg_469.setValue((void*) ".");
            goto out_peg_457;
            not_peg_468:
            result_peg_456.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = 0;
                result_peg_456.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_456;
        stream.update(result_peg_456.getPosition());
    }
    
    
    return result_peg_456;
    out_peg_457:
    Result result_peg_470(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_470.getPosition()))){
                    result_peg_470.nextPosition();
                } else {
                    goto out_peg_471;
                }
            }
            result_peg_470.setValue((void*) "a");
        
        
        
        int save_peg_474 = result_peg_470.getPosition();
            
            result_peg_470 = rule_number(stream, result_peg_470.getPosition());
            if (result_peg_470.error()){
                
                result_peg_470 = Result(save_peg_474);
                result_peg_470.setValue((void*) 0);
                
            }
        
        
        
        Result result_peg_477(result_peg_470);
            {
                
                char letter_peg_482 = stream.get(result_peg_477.getPosition());
                if (letter_peg_482 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_482) != NULL){
                    result_peg_477.nextPosition();
                    result_peg_477.setValue((void*) letter_peg_482);
                } else {
                    goto out_peg_480;
                }
                
            }
            goto success_peg_478;
            out_peg_480:
            goto not_peg_476;
            success_peg_478:
            ;
            goto out_peg_471;
            not_peg_476:
            result_peg_470.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = 0;
                result_peg_470.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_470;
        stream.update(result_peg_470.getPosition());
    }
    
    
    return result_peg_470;
    out_peg_471:
    Result result_peg_483(myposition);
    
    result_peg_483 = rule_filename(stream, result_peg_483.getPosition());
    if (result_peg_483.error()){
        goto out_peg_484;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_483;
        stream.update(result_peg_483.getPosition());
    }
    
    
    return result_peg_483;
    out_peg_484:
    
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
        if (column.chunk4 != 0 && column.chunk4->chunk_keyword.calculated()){
            return column.chunk4->chunk_keyword;
        }
    }
    
    int myposition = position;
    tail_peg_485:
    
    Result result_peg_486(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareChar("normal"[i], stream.get(result_peg_486.getPosition()))){
            result_peg_486.nextPosition();
        } else {
            goto out_peg_487;
        }
    }
    result_peg_486.setValue((void*) "normal");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_keyword = result_peg_486;
        stream.update(result_peg_486.getPosition());
    }
    
    
    return result_peg_486;
    out_peg_487:
    Result result_peg_488(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("parallax"[i], stream.get(result_peg_488.getPosition()))){
            result_peg_488.nextPosition();
        } else {
            goto out_peg_489;
        }
    }
    result_peg_488.setValue((void*) "parallax");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_keyword = result_peg_488;
        stream.update(result_peg_488.getPosition());
    }
    
    
    return result_peg_488;
    out_peg_489:
    Result result_peg_490(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("addalpha"[i], stream.get(result_peg_490.getPosition()))){
            result_peg_490.nextPosition();
        } else {
            goto out_peg_491;
        }
    }
    result_peg_490.setValue((void*) "addalpha");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_keyword = result_peg_490;
        stream.update(result_peg_490.getPosition());
    }
    
    
    return result_peg_490;
    out_peg_491:
    Result result_peg_492(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareChar("add1"[i], stream.get(result_peg_492.getPosition()))){
            result_peg_492.nextPosition();
        } else {
            goto out_peg_493;
        }
    }
    result_peg_492.setValue((void*) "add1");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_keyword = result_peg_492;
        stream.update(result_peg_492.getPosition());
    }
    
    
    return result_peg_492;
    out_peg_493:
    Result result_peg_494(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("add"[i], stream.get(result_peg_494.getPosition()))){
            result_peg_494.nextPosition();
        } else {
            goto out_peg_495;
        }
    }
    result_peg_494.setValue((void*) "add");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_keyword = result_peg_494;
        stream.update(result_peg_494.getPosition());
    }
    
    
    return result_peg_494;
    out_peg_495:
    Result result_peg_496(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("sub"[i], stream.get(result_peg_496.getPosition()))){
            result_peg_496.nextPosition();
        } else {
            goto out_peg_497;
        }
    }
    result_peg_496.setValue((void*) "sub");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_keyword = result_peg_496;
        stream.update(result_peg_496.getPosition());
    }
    
    
    return result_peg_496;
    out_peg_497:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_date.calculated()){
            return column.chunk4->chunk_date;
        }
    }
    
    int myposition = position;
    tail_peg_498:
    
    Result result_peg_499(myposition);
    
    {
    
        result_peg_499.reset();
            do{
                Result result_peg_503(result_peg_499.getPosition());
                {
                    
                    char letter_peg_508 = stream.get(result_peg_503.getPosition());
                    if (letter_peg_508 != '\0' && strchr("0123456789", letter_peg_508) != NULL){
                        result_peg_503.nextPosition();
                        result_peg_503.setValue((void*) letter_peg_508);
                    } else {
                        goto out_peg_506;
                    }
                    
                }
                goto success_peg_504;
                out_peg_506:
                goto loop_peg_502;
                success_peg_504:
                ;
                result_peg_499.addResult(result_peg_503);
            } while (true);
            loop_peg_502:
            if (result_peg_499.matches() == 0){
                goto out_peg_500;
            }
        
        Result result_peg_501 = result_peg_499;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_499.getPosition()))){
                    result_peg_499.nextPosition();
                } else {
                    goto out_peg_500;
                }
            }
            result_peg_499.setValue((void*) ".");
        
        
        
        result_peg_499.reset();
            do{
                Result result_peg_512(result_peg_499.getPosition());
                {
                    
                    char letter_peg_517 = stream.get(result_peg_512.getPosition());
                    if (letter_peg_517 != '\0' && strchr("0123456789", letter_peg_517) != NULL){
                        result_peg_512.nextPosition();
                        result_peg_512.setValue((void*) letter_peg_517);
                    } else {
                        goto out_peg_515;
                    }
                    
                }
                goto success_peg_513;
                out_peg_515:
                goto loop_peg_511;
                success_peg_513:
                ;
                result_peg_499.addResult(result_peg_512);
            } while (true);
            loop_peg_511:
            if (result_peg_499.matches() == 0){
                goto out_peg_500;
            }
        
        Result result_peg_510 = result_peg_499;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_499.getPosition()))){
                    result_peg_499.nextPosition();
                } else {
                    goto out_peg_500;
                }
            }
            result_peg_499.setValue((void*) ".");
        
        
        
        result_peg_499.reset();
            do{
                Result result_peg_521(result_peg_499.getPosition());
                {
                    
                    char letter_peg_526 = stream.get(result_peg_521.getPosition());
                    if (letter_peg_526 != '\0' && strchr("0123456789", letter_peg_526) != NULL){
                        result_peg_521.nextPosition();
                        result_peg_521.setValue((void*) letter_peg_526);
                    } else {
                        goto out_peg_524;
                    }
                    
                }
                goto success_peg_522;
                out_peg_524:
                goto loop_peg_520;
                success_peg_522:
                ;
                result_peg_499.addResult(result_peg_521);
            } while (true);
            loop_peg_520:
            if (result_peg_499.matches() == 0){
                goto out_peg_500;
            }
        
        Result result_peg_519 = result_peg_499;
        
        {
                Value value((void*) 0);
                value = makeDate(result_peg_501.getValues(),result_peg_510.getValues(),result_peg_519.getValues()); save(as<Ast::Value*>(value));
                result_peg_499.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_date = result_peg_499;
        stream.update(result_peg_499.getPosition());
    }
    
    
    return result_peg_499;
    out_peg_500:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_date = errorResult;
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
    tail_peg_527:
    Value data;
    Result result_peg_528(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_528.getPosition()))){
                    result_peg_528.nextPosition();
                } else {
                    goto out_peg_529;
                }
            }
            result_peg_528.setValue((void*) "\"");
        
        
        
        result_peg_528.reset();
            do{
                Result result_peg_533(result_peg_528.getPosition());
                {
                
                    Result result_peg_536(result_peg_533);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_536.getPosition()))){
                                result_peg_536.nextPosition();
                            } else {
                                goto not_peg_535;
                            }
                        }
                        result_peg_536.setValue((void*) "\"");
                        goto loop_peg_532;
                        not_peg_535:
                        result_peg_533.setValue((void*)0);
                    
                    
                    
                    Result result_peg_539(result_peg_533);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_539.getPosition()))){
                                result_peg_539.nextPosition();
                            } else {
                                goto not_peg_538;
                            }
                        }
                        result_peg_539.setValue((void*) "\n");
                        goto loop_peg_532;
                        not_peg_538:
                        result_peg_533.setValue((void*)0);
                    
                    
                    
                    char temp_peg_540 = stream.get(result_peg_533.getPosition());
                        if (temp_peg_540 != '\0'){
                            result_peg_533.setValue((void*) temp_peg_540);
                            result_peg_533.nextPosition();
                        } else {
                            goto loop_peg_532;
                        }
                    
                    
                }
                result_peg_528.addResult(result_peg_533);
            } while (true);
            loop_peg_532:
            ;
            data = result_peg_528.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_528.getPosition()))){
                    result_peg_528.nextPosition();
                } else {
                    goto out_peg_529;
                }
            }
            result_peg_528.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(data); save(as<Ast::String*>(value));
                result_peg_528.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_string = result_peg_528;
        stream.update(result_peg_528.getPosition());
    }
    
    
    return result_peg_528;
    out_peg_529:
    
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
    tail_peg_542:
    
    Result result_peg_543(myposition);
    
    {
    
        int save_peg_546 = result_peg_543.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_543.getPosition()))){
                        result_peg_543.nextPosition();
                    } else {
                        goto out_peg_549;
                    }
                }
                result_peg_543.setValue((void*) "+");
                    
            }
            goto success_peg_547;
            out_peg_549:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_543.getPosition()))){
                        result_peg_543.nextPosition();
                    } else {
                        goto out_peg_551;
                    }
                }
                result_peg_543.setValue((void*) "-");
                    
            }
            goto success_peg_547;
            out_peg_551:
            
            result_peg_543 = Result(save_peg_546);
            result_peg_543.setValue((void*) 0);
            
            success_peg_547:
            ;
        
        Result result_peg_545 = result_peg_543;
        
        result_peg_543 = rule_float_or_integer(stream, result_peg_543.getPosition());
            if (result_peg_543.error()){
                goto out_peg_544;
            }
        
        Result result_peg_552 = result_peg_543;
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_545.getValues(),result_peg_552.getValues()); save(as<Ast::Value*>(value));
                result_peg_543.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_number = result_peg_543;
        stream.update(result_peg_543.getPosition());
    }
    
    
    return result_peg_543;
    out_peg_544:
    
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
    tail_peg_553:
    Value left;
    Value right;
    Result result_peg_554(myposition);
    
    {
    
        result_peg_554.reset();
            do{
                Result result_peg_558(result_peg_554.getPosition());
                {
                    
                    char letter_peg_563 = stream.get(result_peg_558.getPosition());
                    if (letter_peg_563 != '\0' && strchr("0123456789", letter_peg_563) != NULL){
                        result_peg_558.nextPosition();
                        result_peg_558.setValue((void*) letter_peg_563);
                    } else {
                        goto out_peg_561;
                    }
                    
                }
                goto success_peg_559;
                out_peg_561:
                goto loop_peg_557;
                success_peg_559:
                ;
                result_peg_554.addResult(result_peg_558);
            } while (true);
            loop_peg_557:
            ;
            left = result_peg_554.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_554.getPosition()))){
                    result_peg_554.nextPosition();
                } else {
                    goto out_peg_555;
                }
            }
            result_peg_554.setValue((void*) ".");
        
        
        
        result_peg_554.reset();
            do{
                Result result_peg_567(result_peg_554.getPosition());
                {
                    
                    char letter_peg_572 = stream.get(result_peg_567.getPosition());
                    if (letter_peg_572 != '\0' && strchr("0123456789", letter_peg_572) != NULL){
                        result_peg_567.nextPosition();
                        result_peg_567.setValue((void*) letter_peg_572);
                    } else {
                        goto out_peg_570;
                    }
                    
                }
                goto success_peg_568;
                out_peg_570:
                goto loop_peg_566;
                success_peg_568:
                ;
                result_peg_554.addResult(result_peg_567);
            } while (true);
            loop_peg_566:
            if (result_peg_554.matches() == 0){
                goto out_peg_555;
            }
            right = result_peg_554.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(left,right); save(as<double*>(value));
                result_peg_554.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_554;
        stream.update(result_peg_554.getPosition());
    }
    
    
    return result_peg_554;
    out_peg_555:
    Result result_peg_573(myposition);
    
    {
    
        result_peg_573.reset();
            do{
                Result result_peg_577(result_peg_573.getPosition());
                {
                    
                    char letter_peg_582 = stream.get(result_peg_577.getPosition());
                    if (letter_peg_582 != '\0' && strchr("0123456789", letter_peg_582) != NULL){
                        result_peg_577.nextPosition();
                        result_peg_577.setValue((void*) letter_peg_582);
                    } else {
                        goto out_peg_580;
                    }
                    
                }
                goto success_peg_578;
                out_peg_580:
                goto loop_peg_576;
                success_peg_578:
                ;
                result_peg_573.addResult(result_peg_577);
            } while (true);
            loop_peg_576:
            if (result_peg_573.matches() == 0){
                goto out_peg_574;
            }
        
        Result result_peg_575 = result_peg_573;
        
        Result result_peg_585(result_peg_573);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_585.getPosition()))){
                    result_peg_585.nextPosition();
                } else {
                    goto not_peg_584;
                }
            }
            result_peg_585.setValue((void*) ".");
            goto out_peg_574;
            not_peg_584:
            result_peg_573.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_575.getValues()); save(as<double*>(value));
                result_peg_573.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_573;
        stream.update(result_peg_573.getPosition());
    }
    
    
    return result_peg_573;
    out_peg_574:
    
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

        
