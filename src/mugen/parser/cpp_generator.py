import core
from core import newResult, indent, gensym, special_char, newOut, Accessor

result_code = """
class Result{
public:
    Result():
    position(-2)%(initialize-state)s{
    }

    Result(const int position):
    position(position)%(initialize-state)s{
    }

    Result(const Result & r):
    position(r.position),
    value(r.value)%(copy-state-r)s{
    }

    Result & operator=(const Result & r){
        position = r.position;
        value = r.value;
        %(assign-state-r)s
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

    %(get/set-state)s
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
    %(state-id)s
};"""

start_cpp_code = """
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

%(state-class)s
%(result-class)s
%(chunks)s

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
        %(initialize-state-counter)s
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
            return '\\0';
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
            if (buffer[i] == '\\n'){
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
            if (buffer[i] == '\\n'){
                line += 1;
                column = 1;
            } else {
                column += 1;
            }
        }
        int context = %(error-size)d;
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
                case '\\n' : {
                    show << '\\\\';
                    show << 'n';
                    break;
                }
                case '\\r' : {
                    show << '\\\\';
                    show << 'r';
                    break;
                }
                case '\\t' : {
                    show << '\\\\';
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
        throw ParseException(out.str(), line, column);
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

    void push_rule(const char * name){
        rule_backtrace.push_back(name);
    }

    void pop_rule(){
        rule_backtrace.pop_back();
    }

    %(state-functions)s

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
    %(current-state)s
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
"""

state_stuff = """
/* Holds arbitrary global state as defined by the user. The user
 * should create a subclass of State and make createState() return
 * an instance of that subclass.
 */
class State{
public:
    /* Each state gets a unique id */
    State(unsigned int id, State * parent):
    parent(parent),
    child(NULL),
    id(id){
    }

    bool operator==(const State & state) const {
        return getId() == state.getId();
    }

    void setNextState(State * state){
        child = state;
    }

    unsigned int getId() const {
        return id;
    }

    /* must be defined by the user */
    static State * createState(State * parent);

    virtual ~State(){
    }
private:
    State * parent;
    State * child;

    /* id for this state */
    unsigned int id;
};"""

# all the self parameters are named 'me' because the code was originally
# copied from another class and to ensure that copy/paste errors don't
# occur I have changed the name from 'self' to 'me'
# that is, 'self' in the original code is now the parameter 'pattern'
class CppGenerator(core.CodeGenerator):
    def fixup_cpp(self, code, args):
        import re
        fix = re.compile("\$(\d+)")
        # return re.sub(fix, r"values.getValues()[\1-1]", code)
        return re.sub(fix, lambda obj: args(int(obj.group(1))) + ".getValues()", code)

    def generate_not(me, pattern, peg, result, stream, failure, tail, peg_args):
        not_label = gensym("not")
        my_result = newResult()
        my_fail = lambda : "goto %s;" % not_label
        data = """
Result %(new-result)s(%(result)s);
%(code)s
%(fail)s
%(not)s:
%(result)s.setValue(Value((void*)0));
        """ % {'new-result': my_result,
               'result': result,
               'code': pattern.next.generate_cpp(peg, my_result, stream, my_fail, None, peg_args).strip(),
               'fail': failure(),
               'not': not_label,
               'result': result}

        return data

    def generate_ensure(me, pattern, peg, result, stream, failure, tail, peg_args):
        my_result = newResult()
        data = """
Result %(new-result)s(%(result)s.getPosition());
%(code)s
""" % {'new-result': my_result,
       'result': result,
       'code': pattern.next.generate_cpp(peg, my_result, stream, failure, None, peg_args).strip()}
        return data

    def generate_call_rule(me, pattern, peg, result, stream, failure, tail, peg_args):
        def change(arg):
            if arg.startswith('@'):
                return arg[1:]
            return 'rule_%s' % arg
        rule_parameters = ""
        if pattern.rules != None:
            rule_parameters = ", %s" % ", ".join([change(f) for f in pattern.rules])

        parameters = ""
        if pattern.values != None:
            parameters = ", %s" % ", ".join([me.fixup_cpp(p, peg_args) for p in pattern.values])
            # parameters = ", %s" % fix_param(pattern.parameters)

        def argify(name, many):
            if many == None or len(many) == 0:
                return ""
            return ", " + ",".join([name] * len(many))

        cast = "Result (*)(Stream &, const int%s%s)" % (argify('void *', pattern.rules), argify('Value', pattern.values))

        data = """
%(result)s = ((%(cast)s) %(name)s)(%(stream)s, %(result)s.getPosition()%(rule-parameters)s%(parameters)s);
if (%(result)s.error()){
    %(fail)s
}
""" % {'result': result,
       'cast': cast,
       'name': pattern.name,
       'stream': stream,
       'rule-parameters': rule_parameters,
       'parameters': parameters,
       'result': result,
       'fail': indent(failure())}

        return data

    def generate_predicate(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = """
{
    bool %(variable)s = true;
    %(code)s
    if (!%(variable)s){
        %(fail)s
    }
}
""" % {'variable': pattern.variable,
       'code': me.fixup_cpp(indent(pattern.code.strip()), peg_args),
       'fail': failure()}
        return data

    def generate_rule(me, pattern, peg, result, stream, failure, tail, peg_args):
        rule = peg.getRule(pattern.rule)
        if rule != None and rule.isInline():
            # TODO: add rule parameters and regular parameters for inlined rules
            if tail != None:
                raise Exception("Do not combine inlined rules that use tail recursion")
            def newPattern(pattern, stream, result, success):
                # my_result = newResult()
                previous_position = gensym('position')
                out = [False]
                def label(n):
                    if n != False:
                        return "%s:" % n
                    return ""

                def fail():
                    if out[0] == False:
                        out[0] = newOut()
                    return "%s.setPosition(%s);\ngoto %s;" % (result, previous_position, out[0])
                # pattern_result = pattern.generate_cpp(peg, my_result, stream, fail, tail, peg_args).strip()

                data = """
{
    int %(previous)s = %(result)s.getPosition();
    %(code)s
}
%(success)s
%(label)s
""" % {'previous': previous_position,
       'result': result,
       'code': indent(pattern.generate_cpp(peg, result, stream, fail, tail, peg_args)),
       'success': success,
       'label': label(out[0])}
                return data

            success_out = gensym('success')
            data = """
%(code)s
%(fail)s
%(success)s:
;
""" % {'code': '\n'.join([newPattern(pattern, stream, result, "goto %s;" % success_out).strip() for pattern in rule.patterns]),
       'fail': failure(),
       'success': success_out}
            return data
        else:
            # TODO: add rule parameters here
            if tail != None:
                if len(tail) == 0:
                    return ""
                else:
                    if pattern.parameters == None or len(tail) != len(pattern.parameters):
                        raise Exception("Expected parameters %s but got %s while calling rule '%s'" % (tail, pattern.parameters, pattern.rule))
                    return '\n'.join(["%s = %s;" % (q[0], me.fixup_cpp(q[1], peg_args)) for q in zip(tail, pattern.parameters)])
            else:
                def change(arg):
                    if arg.startswith('@'):
                        return arg[1:]
                    if peg.getRule(arg) == None:
                        raise Exception("Cannot find rule '%s' while trying to call rule '%s'" % (arg, pattern.rule))
                    return '(void*) rule_%s' % arg
                rule_parameters = ""
                if pattern.rules != None:
                    rule_parameters = ", %s" % ", ".join([change(f) for f in pattern.rules])
                parameters = ""
                if pattern.parameters != None:
                    parameters = ", %s" % ", ".join([me.fixup_cpp(p, peg_args) for p in pattern.parameters])
                    # parameters = ", %s" % fix_param(pattern.parameters)
                data = """
%(result)s = rule_%(rule)s(%(stream)s, %(result)s.getPosition()%(rule-parameters)s%(parameters)s);
if (%(result)s.error()){
    %(fail)s
}
""" % {'result': result,
       'rule': pattern.rule,
       'stream': stream,
       'rule-parameters': rule_parameters,
       'parameters': parameters,
       'fail': indent(failure())}

                return data

    def generate_void(me, pattern, peg, result, stream, failure, tail, peg_args):
        return ""

    def generate_eof(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = """
if ('\\0' == %(stream)s.get(%(result)s.getPosition())){
    %(result)s.nextPosition();
    %(result)s.setValue(Value((void *) '\\0'));
} else {
    %(fail)s
}
""" % {'stream': stream,
       'result': result,
       'fail': indent(failure())}
        return data

    def generate_sequence(me, pattern, peg, result, stream, failure, tail, peg_args):
        if len(pattern.patterns) == 1:
            return pattern.patterns[0].generate_cpp(peg, result, stream, failure, tail, peg_args)
        else:
            # for each pattern, save the result in a temporary variable. only create
            # temporaries if the result is used. looking up a variable through the
            # 'args' accessor tells the code generator to generate the variable
            data = []
            def invalid(d):
                raise Exception("Invalid result %s" % d)
            args = invalid
            use_args = []
            arg_num = 0

            fail = False
            for apattern in pattern.patterns:
                use_args.append("")
                do_tail = None
                if apattern == pattern.patterns[-1]:
                    do_tail = tail
                else:
                    # lexical scope is broken so we need another function here
                    def make(n, old_arg, my_result):
                        def get(d):
                            # print "Looking for %s arg_num is %d result is %s. previous is %s" % (d, n, my_result, old_arg)
                            if d == n:
                                use_args[n-1] = "Result %s = %s;" % (my_result, result)
                                return my_result
                            return old_arg(d)
                        return get
                    arg_num += 1
                    args = make(arg_num, args, newResult())

                data.append("""
%s
""" % (indent(apattern.generate_cpp(peg, result, stream, failure, do_tail, args).strip())))

            return "{\n%s\n}" % indent('\n'.join(["%s\n%s" % (x[0], x[1]) for x in zip(data, use_args)]))

    def generate_repeat_once(me, pattern, peg, result, stream, failure, tail, peg_args):
        loop_done = gensym("loop")
        my_fail = lambda : "goto %s;" % loop_done
        my_result = newResult()
        data = """
%(result)s.reset();
do{
    Result %(new-result)s(%(result)s.getPosition());
    %(code)s
    %(result)s.addResult(%(new-result)s);
} while (true);
%(loop-done)s:
if (%(result)s.matches() == 0){
    %(fail)s
}
""" % {'result': result,
       'new-result': my_result,
       'code': indent(pattern.next.generate_cpp(peg, my_result, stream, my_fail, tail, peg_args).strip()),
       'loop-done': loop_done,
       'fail': indent(failure())}

        return data

    def generate_code(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = """
{
    Value value((void*) 0);
    %(code)s
    %(result)s.setValue(value);
}
        """ % {'code': me.fixup_cpp(indent(pattern.code.strip()), peg_args),
               'result': result}

        return data

    def generate_repeat_many(me, pattern, peg, result, stream, failure, tail, peg_args):
        loop_done = gensym("loop")
        my_fail = lambda: "goto %s;" % loop_done
        my_result = newResult()
        data = """
%(result)s.reset();
do{
    Result %(new-result)s(%(result)s.getPosition());
    %(code)s
    %(result)s.addResult(%(new-result)s);
} while (true);
%(loop-done)s:
;
        """ % {'result': result,
               'new-result': my_result,
               'code': indent(pattern.next.generate_cpp(peg, my_result, stream, my_fail, tail, peg_args).strip()),
               'loop-done': loop_done}
        return data

    def generate_any(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = """
char %(temp)s = %(stream)s.get(%(result)s.getPosition());
if (%(temp)s != '\\0'){
    %(result)s.setValue(Value((void*) (long) %(temp)s));
    %(result)s.nextPosition();
} else {
    %(fail)s
}
""" % {'temp': gensym(),
       'stream': stream,
       'result': result,
       'fail': indent(failure())}
        return data

    def generate_maybe(me, pattern, peg, result, stream, failure, tail, peg_args):
        save = gensym("save")
        fail = lambda: """
%(result)s = Result(%(save)s);
%(result)s.setValue(Value((void*) 0));
""" % {'result': result,
       'save': save}

        data = """
int %(save)s = %(result)s.getPosition();
%(code)s
""" % {'save': save,
       'result': result,
       'code': pattern.pattern.generate_cpp(peg, result, stream, fail, tail, peg_args)}
        return data

    def generate_or(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = ""
        success = gensym("success")
        for pattern in pattern.patterns:
            # TODO: lazily create this
            out = gensym("or")
            my_result = newResult()
            fail = lambda : "goto %s;" % out
            if pattern == pattern.patterns[-1]:
                fail = failure
            data += """
{
Result %(new-result)s(%(result)s.getPosition());
%(code)s
%(result)s = %(new-result)s;
}
goto %(success)s;
%(out)s:
""" % {'new-result': my_result,
       'result': result,
       'code': pattern.generate_cpp(peg, my_result, stream, fail, tail, peg_args).strip(),
       'success': success,
       'out': out}
        data += "%s:\n" % success
        return data

    def generate_bind(me, pattern, peg, result, stream, failure, tail, peg_args):
        if pattern.pattern.isLineInfo():
            name = gensym("line_info");
            data = """
Stream::LineInfo %(name)s = %(stream)s.getLineInfo(%(result)s.getPosition());
%(variable)s = &%(name)s;
""" % {'name': name,
       'stream': stream,
       'result': result,
       'variable': pattern.variable}
        else:
            data = """
%(code)s
%(variable)s = %(result)s.getValues();
""" % {'code': pattern.pattern.generate_cpp(peg, result, stream, failure, tail, peg_args).strip(),
       'variable': pattern.variable,
       'result': result}
        return data

    def generate_range(me, pattern, peg, result, stream, failure, tail, peg_args):
        letter = gensym("letter")
        data = """
char %(letter)s = %(stream)s.get(%(result)s.getPosition());
if (%(letter)s != '\\0' && strchr("%(range)s", %(letter)s) != NULL){
    %(result)s.nextPosition();
    %(result)s.setValue(Value((void*) (long) %(letter)s));
} else {
    %(failure)s
}
""" % {'letter': letter,
       'stream': stream,
       'result': result,
       'range': pattern.range,
       'failure': indent(failure())}
        return data

    def generate_verbatim(me, pattern, peg, result, stream, failure, tail, peg_args):
        def doString():
            length = len(pattern.letters)
            if special_char(pattern.letters):
                length = 1
            comparison = "compareChar"
            if pattern.options == "{case}":
                comparison = "compareCharCase"
            data = """
%(result)s.setValue(Value((void*) "%(string)s"));
for (int i = 0; i < %(length)d; i++){
    if (%(compare)s("%(string)s"[i], %(stream)s.get(%(result)s.getPosition()))){
        %(result)s.nextPosition();
    } else {
        %(failure)s
    }
}
    """ % {'result': result,
           'string': pattern.letters.replace('"', '\\"'),
           'length': length,
           'compare': comparison,
           'stream': stream,
           'failure': indent(indent(failure()))}

            return data
        def doAscii():
            data = """
%(result)s.setValue(Value((void*) %(string)s));
if ((unsigned char) %(stream)s.get(%(result)s.getPosition()) == (unsigned char) %(string)s){
    %(result)s.nextPosition();
} else {
    %(failure)s
}
"""
            return data % {'result': result,
                           'string': pattern.letters,
                           'stream': stream,
                           'failure': indent(failure())}

        if type(pattern.letters) == type('x'):
            return doString()
        elif type(pattern.letters) == type(0):
            return doAscii()
        else:
            raise Exception("unknown verbatim value %s" % pattern.letters)

# self is the peg
def generate(self, parallel = False, separate = None, directory = '.', main = False):
    def prototype(rule):
        rule_parameters = ""
        if rule.rules != None:
            # rule_parameters = ", " + ", ".join(['Result (*%s)(Stream &, const int, ...)' % name for name in rule.rules])
            rule_parameters = ", " + ", ".join(['void *%s' % name for name in rule.rules])
        parameters = ""
        if rule.parameters != None:
            parameters = ", " + ", ".join(["Value %s" % p for p in rule.parameters])
        return "Result rule_%s(Stream &, const int%s%s);" % (rule.name, rule_parameters, parameters)

    # r = 0
    use_rules = [rule for rule in self.rules if not rule.isInline()]
    # rule_numbers = '\n'.join(["const int RULE_%s = %d;" % (x[0].name, x[1]) for x in zip(use_rules, range(0, len(use_rules)))])


    chunk_accessors = []
    def findAccessor(rule):
        for accessor in chunk_accessors:
            if accessor.rule == rule:
                return accessor
        raise Exception("Cannot find accessor for " + rule.name)

    def makeChunks(rules):
        import math

        values_per_chunk = self.chunks
        #values_per_chunk = int(math.sqrt(len(rules)))
        #if values_per_chunk < 5:
        #    values_per_chunk = 5
        all = []
        pre = ""
        chunk_to_rules = {}
        for i in xrange(0,int(math.ceil(float(len(rules)) / values_per_chunk))):
            values = rules[i*values_per_chunk:(i+1)*values_per_chunk]
            name = "Chunk%d" % i
            chunk_to_rules[name.lower()] = values
            chunk_accessors.extend([Accessor(".%s" % name.lower(), "->chunk_%s" % rule.name, name, rule) for rule in values])

            value_data = """
struct %(name)s{
%(items)s
};
""" % {'name': name,
       'items': indent("\n".join(["Result chunk_%s;" % rule.name for rule in values]))}

            all.append(name)
            pre += value_data

        def sumChunk(chunk):
            data = """
(%s != NULL ? (%s) : 0)
""" % (chunk, '\n+ '.join(["(%s->chunk_%s.calculated() ? 1 : 0)" % (chunk, rule.name) for rule in chunk_to_rules[chunk]]))
            return data

        hit_count = '+'.join([sumChunk(chunk) for chunk in chunk_to_rules.keys()])
        # Disable for now
        hit_count = "0"

        data = """
%(pre)s
struct Column{
    Column():
    %(initializers)s{
    }

    %(members)s

    int hitCount(){
        return %(hit-count)s;
    }

    int maxHits(){
        return %(rules)s;
    }

    ~Column(){
        %(deletes)s
    }
};
""" % {'pre': pre,
       'initializers': indent(indent("\n,".join(["%s(0)" % x.lower() for x in all]))),
       'members': indent("\n".join(["%s * %s;" % (x, x.lower()) for x in all])),
       'hit-count': hit_count,
       'rules': len(rules),
       'deletes': indent(indent("\n".join(["delete %s;" % x.lower() for x in all])))}

        return data

    chunks = makeChunks(use_rules)

    top_code = ""
    if self.include_code != None:
        top_code = self.include_code
    more_code = ""
    if self.more_code != None:
        more_code = self.more_code

    namespace_start = self.cppNamespaceStart()
    namespace_end = self.cppNamespaceEnd()

    maybe_state_stuff = ""
    if self.transactions:
        maybe_state_stuff = state_stuff

    maybe_state_id = ""
    if self.transactions:
        maybe_state_id = 'State * state;';

    def singleFile():
        result_strings = {'initialize-state': '',
                          'copy-state-r': '',
                          'assign-state-r': '',
                          'get/set-state': '',
                          'state-id': ''}
        if self.transactions:
            result_strings = {'initialize-state': ',\n    stateId(0)',
                              'copy-state-r': ',\n    stateId(r.stateId)',
                              'assign-state-r': 'stateId = r.stateId;',
                              'get/set-state': indent("""State * getState() const {
    return state;
}

void setState(State * state){
    this->state = state;
}
                              """),
                              'state-id': maybe_state_id
                             }

        result_class = result_code % result_strings

        current_state = ''
        if self.transactions:
            current_state = indent("""State * currentState;
unsigned int stateId;""")

        state_functions = ''
        if self.transactions:
            state_functions = indent("""
State * getCurrentState(){
    return currentState;
}

State * startTransaction(){
    State * newState = State::createState(current);
    current->setNextState(newState);
    return newState;
}

void abortTransaction(State * state){
    State * parent = state->getParent();
    /* The state should always have a parent because the top most state
     * will never be aborted.
     */
    if (parent != NULL){
        parent->setNextState(NULL);
        currentState = parent;
    }
}

void commitTransaction(State * state){
}

unsigned int newId(){
    unsigned int use = counter;
    counter += 1;
    return use;
}""")
                                                   
        maybe_initialize_state_counter = ''
        if self.transactions:
            maybe_initialize_state_counter = indent("""stateId = 0;
    currentState = State::createState(NULL);""")

        strings = {'top-code': top_code,
                   'namespace-start': namespace_start,
                   'start-code': start_cpp_code % {'result-class': result_class,
                                                   'chunks': chunks,
                                                   'initialize-state-counter': maybe_initialize_state_counter,
                                                   'state-class': maybe_state_stuff,
                                                   'state-functions': state_functions,
                                                   'current-state': current_state,
                                                   'error-size': self.error_size},
                   'rules': '\n'.join([prototype(rule) for rule in use_rules]),
                   'more-code': more_code,
                   'generated': '\n'.join([rule.generate_cpp(self, findAccessor(rule)) for rule in use_rules]),
                   'start': self.start,
                   'namespace-end': namespace_end}

        data = """
%(top-code)s

#include <list>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>

%(namespace-start)s
%(start-code)s

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

%(rules)s

%(more-code)s

%(generated)s

static const void * doParse(Stream & stream, bool stats, const std::string & context){
    errorResult.setError();
    Result done = rule_%(start)s(stream, 0);
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

%(namespace-end)s
    """ % strings
        return data

    def multipleFiles(name):
        prototypes = '\n'.join([prototype(rule) for rule in use_rules])
        if not main:
            for rule in use_rules:
                rule_data = rule.generate_cpp(self, findAccessor(rule))
                out = """
%s
#include "%s.h"
%s
%s
%s
""" % (top_code, name, namespace_start, rule_data, namespace_end)
                file = open('%s/%s-%s.cpp' % (directory, name, rule.name), 'w')
                file.write(out)
                file.close()

        header_guard = "_peg_%s_h_" % name
        header_data = """
#ifndef %s
#define %s

#include <list>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>

%s
%s
%s

extern Result errorResult;
%s
#endif
""" % (header_guard, header_guard, namespace_start, start_cpp_code % (chunks, self.error_size), prototypes, namespace_end)
        if not main:
            header_file = open('%s/%s.h' % (directory, name), 'w')
            header_file.write(header_data)
            header_file.close()

        data = """
%s

#include <list>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>
#include "%s.h"

%s
%s

std::string ParseException::getReason() const {
return message;
}

Result errorResult(-1);

static const void * doParse(Stream & stream, bool stats, const std::string & context){
    errorResult.setError();
    Result done = rule_%s(stream, 0);
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

%s
"""

        return data % (top_code, name, namespace_start, more_code, self.start, namespace_end)

    if separate == None:
        return singleFile()
    else:
        return multipleFiles(separate)

