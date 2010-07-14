



#include <list>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>


namespace Bor{




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
    Result chunk_comment;
    Result chunk_empty__space;
    Result chunk_data;
};

struct Chunk1{
    Result chunk_line_end;
    Result chunk_item;
    Result chunk_valid_letter;
    Result chunk_sw;
};

struct Column{
    Column():
        chunk0(0)
        ,chunk1(0){
    }

    Chunk0 * chunk0;
    Chunk1 * chunk1;

    int hitCount(){
        return 
(chunk1 != NULL ? ((chunk1->chunk_line_end.calculated() ? 1 : 0)
+ (chunk1->chunk_item.calculated() ? 1 : 0)
+ (chunk1->chunk_valid_letter.calculated() ? 1 : 0)
+ (chunk1->chunk_sw.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_comment.calculated() ? 1 : 0)
+ (chunk0->chunk_empty__space.calculated() ? 1 : 0)
+ (chunk0->chunk_data.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 9;
    }

    ~Column(){
        delete chunk0;
        delete chunk1;
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
Result rule_line(Stream &, const int);
Result rule_comment(Stream &, const int);
Result rule_empty__space(Stream &, const int);
Result rule_data(Stream &, const int);
Result rule_line_end(Stream &, const int);
Result rule_item(Stream &, const int);
Result rule_valid_letter(Stream &, const int);
Result rule_sw(Stream &, const int);





Result rule_start(Stream & stream, const int position){
    
    RuleTrace trace_peg_7(stream, "start");
    int myposition = position;
    
    
    Result result_peg_2(myposition);
    
    {
    
        result_peg_2.reset();
            do{
                Result result_peg_5(result_peg_2.getPosition());
                result_peg_5 = rule_line(stream, result_peg_5.getPosition());
                if (result_peg_5.error()){
                    goto loop_peg_4;
                }
                result_peg_2.addResult(result_peg_5);
            } while (true);
            loop_peg_4:
            ;
        
        
        
        if ('\0' == stream.get(result_peg_2.getPosition())){
                result_peg_2.nextPosition();
                result_peg_2.setValue((void *) '\0');
            } else {
                goto out_peg_6;
            }
        
        
    }
    stream.update(result_peg_2.getPosition());
    
    return result_peg_2;
    out_peg_6:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position){
    
    RuleTrace trace_peg_15(stream, "line");
    int myposition = position;
    
    
    Result result_peg_9(myposition);
    
    result_peg_9 = rule_comment(stream, result_peg_9.getPosition());
    if (result_peg_9.error()){
        goto out_peg_10;
    }
    stream.update(result_peg_9.getPosition());
    
    return result_peg_9;
    out_peg_10:
    Result result_peg_11(myposition);
    
    result_peg_11 = rule_empty__space(stream, result_peg_11.getPosition());
    if (result_peg_11.error()){
        goto out_peg_12;
    }
    stream.update(result_peg_11.getPosition());
    
    return result_peg_11;
    out_peg_12:
    Result result_peg_13(myposition);
    
    result_peg_13 = rule_data(stream, result_peg_13.getPosition());
    if (result_peg_13.error()){
        goto out_peg_14;
    }
    stream.update(result_peg_13.getPosition());
    
    return result_peg_13;
    out_peg_14:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    RuleTrace trace_peg_28(stream, "comment");
    int myposition = position;
    
    
    Result result_peg_17(myposition);
    
    {
    
        result_peg_17 = rule_sw(stream, result_peg_17.getPosition());
            if (result_peg_17.error()){
                goto out_peg_19;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("#"[i], stream.get(result_peg_17.getPosition()))){
                    result_peg_17.nextPosition();
                } else {
                    goto out_peg_19;
                }
            }
            result_peg_17.setValue((void*) "#");
        
        
        
        result_peg_17.reset();
            do{
                Result result_peg_23(result_peg_17.getPosition());
                {
                
                    Result result_peg_26(result_peg_23);
                        result_peg_26 = rule_line_end(stream, result_peg_26.getPosition());
                        if (result_peg_26.error()){
                            goto not_peg_25;
                        }
                        goto loop_peg_22;
                        not_peg_25:
                        result_peg_23.setValue((void*)0);
                    
                    
                    
                    char temp_peg_27 = stream.get(result_peg_23.getPosition());
                        if (temp_peg_27 != '\0'){
                            result_peg_23.setValue((void*) (long) temp_peg_27);
                            result_peg_23.nextPosition();
                        } else {
                            goto loop_peg_22;
                        }
                    
                    
                }
                result_peg_17.addResult(result_peg_23);
            } while (true);
            loop_peg_22:
            ;
        
        
        
        result_peg_17 = rule_line_end(stream, result_peg_17.getPosition());
            if (result_peg_17.error()){
                goto out_peg_19;
            }
        
        
    }
    stream.update(result_peg_17.getPosition());
    
    return result_peg_17;
    out_peg_19:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_empty__space(Stream & stream, const int position){
    
    RuleTrace trace_peg_33(stream, "empty__space");
    int myposition = position;
    
    
    Result result_peg_30(myposition);
    
    {
    
        result_peg_30 = rule_sw(stream, result_peg_30.getPosition());
            if (result_peg_30.error()){
                goto out_peg_32;
            }
        
        
        
        result_peg_30 = rule_line_end(stream, result_peg_30.getPosition());
            if (result_peg_30.error()){
                goto out_peg_32;
            }
        
        
    }
    stream.update(result_peg_30.getPosition());
    
    return result_peg_30;
    out_peg_32:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_data(Stream & stream, const int position){
    
    RuleTrace trace_peg_42(stream, "data");
    int myposition = position;
    
    
    Result result_peg_35(myposition);
    
    {
    
        result_peg_35 = rule_sw(stream, result_peg_35.getPosition());
            if (result_peg_35.error()){
                goto out_peg_37;
            }
        
        
        
        result_peg_35.reset();
            do{
                Result result_peg_40(result_peg_35.getPosition());
                {
                
                    result_peg_40 = rule_item(stream, result_peg_40.getPosition());
                        if (result_peg_40.error()){
                            goto loop_peg_39;
                        }
                    
                    
                    
                    result_peg_40 = rule_sw(stream, result_peg_40.getPosition());
                        if (result_peg_40.error()){
                            goto loop_peg_39;
                        }
                    
                    
                }
                result_peg_35.addResult(result_peg_40);
            } while (true);
            loop_peg_39:
            if (result_peg_35.matches() == 0){
                goto out_peg_37;
            }
        
        
        
        result_peg_35 = rule_line_end(stream, result_peg_35.getPosition());
            if (result_peg_35.error()){
                goto out_peg_37;
            }
        
        
    }
    stream.update(result_peg_35.getPosition());
    
    return result_peg_35;
    out_peg_37:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    RuleTrace trace_peg_53(stream, "line_end");
    int myposition = position;
    
    
    Result result_peg_44(myposition);
    
    result_peg_44.reset();
    do{
        Result result_peg_46(result_peg_44.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_46.getPosition()))){
                    result_peg_46.nextPosition();
                } else {
                    goto out_peg_49;
                }
            }
            result_peg_46.setValue((void*) "\n");
                
        }
        goto success_peg_47;
        out_peg_49:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_46.getPosition()))){
                    result_peg_46.nextPosition();
                } else {
                    goto out_peg_51;
                }
            }
            result_peg_46.setValue((void*) "\r");
                
        }
        goto success_peg_47;
        out_peg_51:
        goto loop_peg_45;
        success_peg_47:
        ;
        result_peg_44.addResult(result_peg_46);
    } while (true);
    loop_peg_45:
    if (result_peg_44.matches() == 0){
        goto out_peg_52;
    }
    stream.update(result_peg_44.getPosition());
    
    return result_peg_44;
    out_peg_52:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_item(Stream & stream, const int position){
    
    RuleTrace trace_peg_59(stream, "item");
    int myposition = position;
    
    
    Result result_peg_55(myposition);
    
    result_peg_55.reset();
    do{
        Result result_peg_57(result_peg_55.getPosition());
        result_peg_57 = rule_valid_letter(stream, result_peg_57.getPosition());
        if (result_peg_57.error()){
            goto loop_peg_56;
        }
        result_peg_55.addResult(result_peg_57);
    } while (true);
    loop_peg_56:
    if (result_peg_55.matches() == 0){
        goto out_peg_58;
    }
    stream.update(result_peg_55.getPosition());
    
    return result_peg_55;
    out_peg_58:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_valid_letter(Stream & stream, const int position){
    
    RuleTrace trace_peg_78(stream, "valid_letter");
    int myposition = position;
    
    
    Result result_peg_61(myposition);
    
    {
    
        Result result_peg_64(result_peg_61);
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_64.getPosition()))){
                        result_peg_64.nextPosition();
                    } else {
                        goto out_peg_67;
                    }
                }
                result_peg_64.setValue((void*) "\n");
                    
            }
            goto success_peg_65;
            out_peg_67:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_64.getPosition()))){
                        result_peg_64.nextPosition();
                    } else {
                        goto out_peg_69;
                    }
                }
                result_peg_64.setValue((void*) "\r");
                    
            }
            goto success_peg_65;
            out_peg_69:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar(" "[i], stream.get(result_peg_64.getPosition()))){
                        result_peg_64.nextPosition();
                    } else {
                        goto out_peg_71;
                    }
                }
                result_peg_64.setValue((void*) " ");
                    
            }
            goto success_peg_65;
            out_peg_71:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\t"[i], stream.get(result_peg_64.getPosition()))){
                        result_peg_64.nextPosition();
                    } else {
                        goto out_peg_73;
                    }
                }
                result_peg_64.setValue((void*) "\t");
                    
            }
            goto success_peg_65;
            out_peg_73:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("#"[i], stream.get(result_peg_64.getPosition()))){
                        result_peg_64.nextPosition();
                    } else {
                        goto out_peg_75;
                    }
                }
                result_peg_64.setValue((void*) "#");
                    
            }
            goto success_peg_65;
            out_peg_75:
            goto not_peg_63;
            success_peg_65:
            ;
            goto out_peg_76;
            not_peg_63:
            result_peg_61.setValue((void*)0);
        
        
        
        char temp_peg_77 = stream.get(result_peg_61.getPosition());
            if (temp_peg_77 != '\0'){
                result_peg_61.setValue((void*) (long) temp_peg_77);
                result_peg_61.nextPosition();
            } else {
                goto out_peg_76;
            }
        
        
    }
    stream.update(result_peg_61.getPosition());
    
    return result_peg_61;
    out_peg_76:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    RuleTrace trace_peg_88(stream, "sw");
    int myposition = position;
    
    
    Result result_peg_80(myposition);
    
    result_peg_80.reset();
    do{
        Result result_peg_82(result_peg_80.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_82.getPosition()))){
                    result_peg_82.nextPosition();
                } else {
                    goto out_peg_85;
                }
            }
            result_peg_82.setValue((void*) " ");
                
        }
        goto success_peg_83;
        out_peg_85:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_82.getPosition()))){
                    result_peg_82.nextPosition();
                } else {
                    goto out_peg_87;
                }
            }
            result_peg_82.setValue((void*) "\t");
                
        }
        goto success_peg_83;
        out_peg_87:
        goto loop_peg_81;
        success_peg_83:
        ;
        result_peg_80.addResult(result_peg_82);
    } while (true);
    loop_peg_81:
    ;
    stream.update(result_peg_80.getPosition());
    
    return result_peg_80;
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



} /* Bor */

        
