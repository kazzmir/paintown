#!/usr/bin/env python

# Packrat PEG (parsing expression grammar) generator
#   http://pdos.csail.mit.edu/~baford/packrat/
# Optimizations (like chunks) and other inspiration: Rats!
#   http://cs.nyu.edu/rgrimm/xtc/rats.html
# By Jon Rafkind
# License: GPL 2

# Python BNF parser:
# 1. 171397b / 45.216s = 3790.62721160651 b/s
# 2. 171397b / 36.751s = 4663.73704116895 b/s
# 3. 171397b / 8.630s = 19860.6025492468 b/s
# 4. 171397b / 10.539s = 16263.1179428788 b/s

# Todo (finished items at bottom)
# inline rules + semantic actions are broken (in C++ at least)
# add header (.h) generator for C/C++
# add generator for scheme, haskell, java, scala, ocaml, erlang, javascript, php, pascal, perl, C
# fix error message reporting (whats wrong with it?)
# Don't memoize if a rule accepts parameters (ruby, python)
# Robert Grimm suggested "His Elkhound-generated C++ parsers do not free memory nor do they integrate GC. Instead, you just allocate from a dedicated region, copy out the AST after parsing, and kill the entire region in one operation."
#

# create a variable name
next_var = 0
def nextVar():
    global next_var;
    next_var += 1;
    return "peg_%d" % next_var

def resetGensym():
    global next_var
    next_var = 0

# substitute variables in a string named by $foo
# "$foo + $bar - $foo" with {foo:1, bar:2} => "1 + 2 - 1"
# this is orders of magnitude slower than normal python string
# interpolation like "%s %s %s" % (str1, str2, str3)
# so only use it if it makes life much easier (and speed isn't an issue)
def template(now, dict):
    import re
    for key in dict:
        now = re.sub(r'\$%s' % key, str(dict[key]), now)
    return now

# create a variable using the argument as a prefix
def gensym(what = "temp"):
    return "%s_%s" % (what, nextVar())

def newResult():
    return gensym("result")

def newOut():
    return gensym("out")

def flatten(lst):
    if isinstance(lst, list):
        out = []
        for x in lst:
            out.extend(flatten(x))
        return out
    return [lst]

def reverse(lst):
    # reversed only seems to be in python 2.4+, but I need to support 2.3
    # list(reversed(lst))
    return lst[::-1]

def special_escape(s):
    return s.replace("\\n", "\\\\n").replace("\\t", "\\\\t").replace("\"", "\\\"").replace("\\r", "\\\\r")

def special_char(s):
    return s in ["\\n", "\\t", "\\r"]

# unique elements of a list
def unique(lst):
    x = []
    for item in lst:
        if not item in x:
            x.append(item)
    return x

def indent(s):
    space = '    '
    return s.replace('\n', '\n%s' % space)

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

%s

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
        /* dont create column objects before they are needed because transient
         * productions will never call for them so we can save some space by
         * not allocating columns at all.
         */
        memset(memo, 0, sizeof(Column*) * memo_size);
        /*
        for (int i = 0; i < memo_size; i++){
            memo[i] = new Column();
        }
        */
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
        memset(&newMemo[memo_size], 0, sizeof(Column*) * (newSize - memo_size));
        /*
        for (int i = memo_size; i < newSize; i++){
            newMemo[i] = new Column();
        }
        */
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
        int context = %d;
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
"""

start_python = """

def special_escape(s):
    return s.replace("\\\\n", "\\\\\\\\n").replace("\\\\t", "\\\\\\\\t").replace("\\\"", '\\\\\\\"').replace("\\\\r", "\\\\\\\\r")

class PegError(Exception):
    def __init__(self):
        Exception.__init__(self)

class NotError(Exception):
    def __init__(self):
        Exception.__init__(self)

class Result:
    def __init__(self, position):
        self.position = position
        self.values = []

    def getPosition(self):
        return self.position

    def nextPosition(self, amount = 1):
        self.position += amount

    def setValue(self, value):
        self.values = value

    def getLastValue(self):
        if type(self.values) is list:
            if len(self.values) > 0:
                return self.values[-1]
            else:
                return None
        return self.values
    
    def matches(self):
        return len(self.values)

    def getValues(self):
        return self.values

    def addResult(self, him):
        self.values.append(him.values)
        self.position = him.position
    
    #def extendResult(self, him):
    #    self.values.extend(him.values)
    #    self.position = him.position

class Stream:
    def __init__(self, filename = None, input = None):
        def read():
            file = open(filename, 'r')
            out = file.read()
            file.close()
            return out
        self.position = 0
        self.limit = 100
        self.furthest = 0
        self.memo = {}
        if filename != None:
            self.all = read()
        elif input != None:
            self.all = input
        else:
            raise PegError("Pass a filename or input")
        # print "Read " + str(len(self.all))

    def get(self, position, number = 1):
        if position + number > self.limit:
            # print (position + number)
            self.limit += 5000
        if position + number > len(self.all):
            return chr(0)
        # print "stream: %s" % self.all[position:position+number]
        return self.all[position:position+number]

    def get2(self, position):
        if position != self.position:
            self.file.seek(position)
        self.position = position + 1
        if position > self.limit:
            print position
            self.limit += 5000
        return self.file.read(1)

    def reportError(self):
        line = 1
        column = 1
        for i in xrange(0, self.furthest):
            if self.all[i] == '\\n':
                line += 1
                column = 1
            else:
                column += 1
        context = 10
        left = self.furthest - context
        right = self.furthest + context
        if left < 0:
            left = 0
        if right > len(self.all):
            right = len(self.all)
        print "Read up till line %d, column %d" % (line, column)
        print "'%s'" % special_escape(self.all[left:right])
        print "%s^" % (' ' * (self.furthest - left))

    def update(self, rule, position, result):
        if result != None and result.getPosition() > self.furthest:
            self.furthest = result.getPosition()

        for_rule = None
        try:
            for_rule = self.memo[rule]
        except KeyError:
            self.memo[rule] = {}
            for_rule = self.memo[rule]
        
        for_position = None
        try:
            for_position = for_rule[position]
        except KeyError:
            for_rule[position] = None

        for_rule[position] = result

    def hasResult(self, rule, position):
        try:
            x = self.memo[rule][position]
            return True
        except KeyError:
            return False

    def result(self, rule, position):
        return self.memo[rule][position]

"""

start_ruby = """

def special_escape(s)
    return s.replace("\\\\n", "\\\\\\\\n").replace("\\\\t", "\\\\\\\\t").replace("\\\"", '\\\\\\\"').replace("\\\\r", "\\\\\\\\r")
end

class PegError < Exception
end

class NotError < Exception
end

class Result
    attr_reader :values, :position

    def initialize(position)
        @position = position
        @values = []
    end

    def getPosition()
        return @position
    end

    def nextPosition(amount = 1)
        @position += amount
    end

    def setValue(value)
        @values = value
    end

    def getLastValue()
        if @values.is_a?(Array)
            if @values.size() > 0
                return @values[-1]
            else
                return nil
            end
        end
        return @values
    end
    
    def matches()
        return @values.size
    end

    def getValues()
        return @values
    end

    def addResult(him)
        @values << him.values
        @position = him.position
    end
    
    #def extendResult(self, him):
    #    self.values.extend(him.values)
    #    self.position = him.position
end

class Stream
    def initialize(filename)
        @file = File.new(filename, 'r')
        @position = 0
        @limit = 100
        @furthest = 0
        @all = @file.read()
        @memo = {}
        # print "Read " + str(len(self.all))
    end

    def close()
        @file.close()
    end

    def get(position, number = 1)
        if position + number > @limit
            # print (position + number)
            @limit += 5000
        end
        if position + number > @all.size
            return 0.chr()
        end
        # print "stream: %s" % self.all[position:position+number]
        return @all[position...position+number]
    end

    def reportError()
        line = 1
        column = 1
        for i in 0..@furthest
            if @all[i] == '\\n'
                line += 1
                column = 1
            else
                column += 1
            end
        end
        context = 10
        left = @furthest - context
        right = @furthest + context
        if left < 0
            left = 0
        end
        if right > @all.size
            right = @all.size
        end
        puts "Read up till line #{line}, column #{column}"
        puts special_escape(@all[left...right])
        puts (' ' * (@furthest - left)) + "^"
    end

    def update(rule, position, result)
        if result != nil and result.getPosition() > @furthest
            @furthest = result.getPosition()
        end

        for_rule = nil
        if @memo.has_key? rule
            for_rule = @memo[rule]
        else
            @memo[rule] = {}
            for_rule = @memo[rule]
        end
        
        for_position = nil
        if for_rule.has_key? position
            for_position = for_rule[position]
        else
            for_rule[position] = nil
        end
        for_rule[position] = result
    end

    def hasResult(rule, position)
        @memo.has_key?(rule) and @memo[rule].has_key?(position)
        # return @memo.has_key?(rule) and @memo[rule].has_key?(position)
    end

    def result(rule, position)
        return @memo[rule][position]
    end
end
"""

# This function exists to fool vim into a coloring syntax properly. Otherwise
# it might get stuck in string-mode due to the above code
def _not_used_by_anything_or_anyone_please_ignore_me():
    pass

# Default class that generates code given some Peg description. Sub-classes
# should implement every generate_* method
class CodeGenerator:
    def __init__(self):
        pass

    def fail(self):
        raise Exception("this method has not been implemented yet")

    def generate_not(self, *args):
        self.fail()

    def generate_ensure(self, *args):
        self.fail()

    def generate_rule(self, *args):
        self.fail()

    def generate_void(self, *args):
        self.fail()
    
    def generate_predicate(me, *args):
        self.fail()

    def generate_eof(self, *args):
        self.fail()

    def generate_sequence(self, *args):
        self.fail()

    def generate_repeat_once(self, *args):
        self.fail()

    def generate_code(self, *args):
        self.fail()

    def generate_repeat_many(self, *args):
        self.fail()

    def generate_any(self, *args):
        self.fail()

    def generate_maybe(self, *args):
        self.fail()

    def generate_or(self, *args):
        self.fail()

    def generate_bind(self, *args):
        self.fail()

    def generate_range(self, *args):
        self.fail()

    def generate_verbatim(self, *args):
        self.fail()

    def generate_line(self, *args):
        self.fail()
    
    def generate_call_rule(self, *args):
        self.fail()

class RubyGenerator(CodeGenerator):
    def fixup_ruby(self, code, how):
        import re
        fix = re.compile("\$(\d+)")
        return re.sub(fix, how, code)

    def generate_sequence(me, pattern, result, previous_result, stream, failure):
        data = ""
        for apattern in pattern.patterns:
            my_result = newResult()
            data += """
%s = Result.new(%s.getPosition())
%s
%s.addResult(%s)
""" % (my_result, result, apattern.generate_v1(me, my_result, result, stream, failure), result, my_result)

        return data + """
%s.setValue(%s.getLastValue())
""" % (result, result)

    # this breaks when the sub-pattern is a PatternSequence, todo: fix it
    def generate_maybe(me, pattern, result, previous_result, stream, failure):
        save = gensym("save")
        fail = lambda : """
%s = Result.new(%s)
%s.setValue(nil)
""" % (result, save, result)

        data = """
%s = %s.getPosition()
%s
""" % (save, result, pattern.pattern.generate_v1(me, result, previous_result, stream, fail))
        return data

    def generate_repeat_many(me, pattern, result, previous_result, stream, failure):
        my_fail = lambda : "raise PegError"
        my_result = newResult()
        data = """
begin
    while true
        %s = Result.new(%s.getPosition())
        %s
        %s.addResult(%s)
    end
rescue PegError
end
        """ % (my_result, result, indent(indent(pattern.next.generate_v1(me, my_result, result, stream, my_fail).strip())), result, my_result)

        return data

    def generate_rule(me, pattern, result, previous_result, stream, failure):
        def fix(v):
            return "%s.getValues()[%s]" % (previous_result, int(v.group(1)) - 1)
        def change(arg):
            if arg.startswith('@'):
                return arg[1:]
            return 'lambda{|*args| rule_%s(*args)}' % arg
        rule_parameters = ""
        if pattern.rules != None:
            rule_parameters = ", %s" % ", ".join([change(f) for f in pattern.rules])

        parameters = ""
        if pattern.parameters != None:
            parameters = ", %s" % ",".join([me.fixup_ruby(p, fix) for p in pattern.parameters])
        data = """
# puts "Trying rule '%s'"
%s = rule_%s(%s, %s.getPosition()%s%s)
if %s == nil
    %s
end
""" % (pattern.rule, result, pattern.rule, stream, result, rule_parameters, parameters, result, indent(failure()))

        return data

    def generate_repeat_once(me, pattern, result, previous_result, stream, failure):
        my_fail = lambda : "raise PegError"
        my_result = newResult()
        my_result2 = newResult()
        data = """
begin
    while (true)
        %s = Result.new(%s.getPosition())
        %s
        %s.addResult(%s)
    end
rescue PegError
    if %s.matches() == 0
        %s
    end
end
        """ % (my_result, result, indent(indent(pattern.next.generate_v1(me, my_result, result, stream, my_fail).strip())), result, my_result, result, failure())

        return data

    def generate_void(me, pattern, result, previous_result, stream, failure):
        return ""

    def generate_verbatim(me, pattern, result, previous_result, stream, failure):
        def doString():
            length = len(pattern.letters)
            if special_char(pattern.letters):
                length = 1
            data = """
if '%s' == %s.get(%s.getPosition(), %s) then
    %s.nextPosition(%s)
    %s.setValue('%s')
else
    %s
end
""" % (pattern.letters, stream, result, length, result, length, result, pattern.letters, indent(failure()))
            return data
        def doAscii():
            data = """
if %s.get(%s.getPosition()).ord() == %s then
    %s.nextPosition()
    %s.setValue(%s)
else
    %s
end
"""
            return data % (stream, result, pattern.letters, result, result, pattern.letters, indent(failure()))
        if type(pattern.letters) == type('x'):
            return doString()
        elif type(pattern.letters) == type(0):
            return doAscii()
        else:
            raise Exception("unknown verbatim value %s" % pattern.letters)

    def generate_ensure(me, pattern, result, previous_result, stream, failure):
        my_result = newResult()
        data = """
%s = Result.new(%s.getPosition())
%s
""" % (my_result, result, pattern.next.generate_v1(me, my_result, result, stream, failure).strip())
        return data

    def generate_not(me, pattern, result, previous_result, stream, failure):
        my_result = newResult()
        my_fail = lambda : "raise NotError"
        data = """
%s = Result.new(%s.getPosition())
begin
    %s
    %s
rescue NotError
    %s.setValue(nil)
end
        """ % (my_result, result, indent(pattern.next.generate_v1(my_result, result, stream, my_fail).strip()), failure(), result)

        return data

    def generate_any(me, pattern, result, previous_result, stream, failure):
        temp = gensym()
        data = """
%s = %s.get(%s.getPosition())
if %s != 0.chr() then
    %s.setValue(%s)
    %s.nextPosition()
else
    %s
end
""" % (temp, stream, result, temp, result, temp, result, indent(failure()))
        return data

    def generate_range(me, pattern, result, previous_result, stream, failure):
        letter = gensym("letter")
        data = """
%s = %s.get(%s.getPosition())
if '%s'.index(%s) != nil then
    %s.nextPosition()
    %s.setValue(%s)
else
    %s
end
""" % (letter, stream, result, pattern.range, letter, result, result, letter, indent(failure()))
        return data

    def generate_eof(me, pattern, result, previous_result, stream, failure):
        data = """
if 0.chr() == %s.get(%s.getPosition()) then
    %s.nextPosition()
    %s.setValue(0.chr())
else
    %s
end
""" % (stream, result, result, result, indent(failure()))
        return data

    def generate_code(me, pattern, result, previous_result, stream, failure):
        data = """
value = nil
values = %s.getValues()
%s
%s.setValue(value)
""" % (previous_result, me.fixup_ruby(pattern.code.strip(), lambda v: "values[%s]" % (int(v.group(1)) - 1)), result)

        return data


    def generate_bind(me, pattern, result, previous_result, stream, failure):
        data = """
%s
%s = %s.getValues()
""" % (pattern.pattern.generate_v1(me, result, previous_result, stream, failure).strip(), pattern.variable, result)
        return data

    def generate_call_rule(me, pattern, result, previous_result, stream, failure):
        def fix(v):
            return "%s.getValues()[%s]" % (previous_result, int(v.group(1)) - 1)
        def change(arg):
            if arg.startswith('@'):
                return arg[1:]
            return 'lambda{|*args| rule_%s(*args)}' % arg
        rule_parameters = ""
        if pattern.rules != None:
            rule_parameters = ", %s" % ", ".join([change(f) for f in pattern.rules])

        parameters = ""
        if pattern.values != None:
            parameters = ", %s" % ",".join([me.fixup_ruby(p, fix) for p in pattern.values])
        data = """
# print "Trying rule " + '%s'
%s = %s.call(%s, %s.getPosition()%s%s)
if %s == nil
    %s
end
""" % (pattern.name, result, pattern.name, stream, result, rule_parameters, parameters, result, indent(failure()))

        return data

class PythonGenerator(CodeGenerator):
    def fixup_python(self, code, how):
        import re
        fix = re.compile("\$(\d+)")
        # return re.sub(fix, r"values[\1-1]", code)
        # return re.sub(fix, r"(\1-1)", code)
        return re.sub(fix, how, code)

    def generate_ensure(me, pattern, result, previous_result, stream, failure):
        my_result = newResult()
        data = """
%s = Result(%s.getPosition())
%s
""" % (my_result, result, pattern.next.generate_python(my_result, result, stream, failure).strip())
        return data

    def generate_not(me, pattern, result, previous_result, stream, failure):
        my_result = newResult()
        my_fail = lambda : "raise NotError"
        data = """
%s = Result(%s.getPosition());
try:
    %s
    %s
except NotError:
    %s.setValue(None)
        """ % (my_result, result, indent(pattern.next.generate_python(my_result, result, stream, my_fail).strip()), failure(), result)

        return data

    def generate_rule(me, pattern, result, previous_result, stream, failure):
        def fix(v):
            return "%s.getValues()[%s]" % (previous_result, int(v.group(1)) - 1)
        def change(arg):
            if arg.startswith('@'):
                return arg[1:]
            return 'rule_%s' % arg
        rule_parameters = ""
        if pattern.rules != None:
            rule_parameters = ", %s" % ", ".join([change(f) for f in pattern.rules])
        parameters = ""
        if pattern.parameters != None:
            parameters = ", %s" % ", ".join([me.fixup_python(p, fix) for p in pattern.parameters])
        data = """
# print "Trying rule " + '%s'
%s = rule_%s(%s, %s.getPosition()%s%s)
if %s == None:
    %s
""" % (pattern.rule, result, pattern.rule, stream, result, rule_parameters, parameters, result, indent(failure()))

        return data

    def generate_eof(me, pattern, result, previous_result, stream, failure):
        data = """
if chr(0) == %s.get(%s.getPosition()):
    %s.nextPosition()
    %s.setValue(chr(0))
else:
    %s
""" % (stream, result, result, result, indent(failure()))
        return data
    
    def generate_call_rule(me, pattern, result, previous_result, stream, failure):
        def fix(v):
            return "%s.getValues()[%s]" % (previous_result, int(v.group(1)) - 1)
        def change(arg):
            if arg.startswith('@'):
                return arg[1:]
            return 'rule_%s' % arg
        rule_parameters = ""
        if pattern.rules != None:
            rule_parameters = ", %s" % ", ".join([change(f) for f in pattern.rules])

        parameters = ""
        if pattern.values != None:
            parameters = ", %s" % ",".join([me.fixup_python(p, fix) for p in pattern.values])
        data = """
# print "Trying rule " + '%s'
%s = %s(%s, %s.getPosition()%s%s)
if %s == None:
    %s
""" % (pattern.name, result, pattern.name, stream, result, rule_parameters, parameters, result, indent(failure()))

        return data


    def generate_sequence(me, pattern, result, previous_result, stream, failure):
        data = ""
        for apattern in pattern.patterns:
            my_result = newResult()
            data += """
%s = Result(%s.getPosition())
%s
%s.addResult(%s);
""" % (my_result, result, apattern.generate_python(my_result, result, stream, failure), result, my_result)

        return data + """
%s.setValue(%s.getLastValue())
""" % (result, result)

    def generate_repeat_once(me, pattern, result, previous_result, stream, failure):
        my_fail = lambda : "raise PegError"
        my_result = newResult()
        my_result2 = newResult()
        data = """
try:
    while True:
        %s = Result(%s.getPosition());
        %s
        %s.addResult(%s);
except PegError:
    if %s.matches() == 0:
        %s
        """ % (my_result, result, indent(indent(pattern.next.generate_python(my_result, result, stream, my_fail).strip())), result, my_result, result, failure())

        return data

    def generate_code(me, pattern, result, previous_result, stream, failure):
        data = """
value = None
values = %s.getValues()
%s
%s.setValue(value)
""" % (previous_result, me.fixup_python(pattern.code.strip(), lambda v: "values[%s]" % (int(v.group(1)) - 1)), result)

        return data

    def generate_repeat_many(me, pattern, result, previous_result, stream, failure):
        my_fail = lambda : "raise PegError"
        my_result = newResult()
        data = """
try:
    while True:
        %s = Result(%s.getPosition());
        %s
        %s.addResult(%s);
except PegError:
    pass
        """ % (my_result, result, indent(indent(pattern.next.generate_python(my_result, result, stream, my_fail).strip())), result, my_result)

        return data

    def generate_any(me, pattern, result, previous_result, stream, failure):
        temp = gensym()
        data = """
%s = %s.get(%s.getPosition())
if %s != chr(0):
    %s.setValue(%s)
    %s.nextPosition()
else:
    %s
""" % (temp, stream, result, temp, result, temp, result, indent(failure()))
        return data

    # this breaks when the sub-pattern is a PatternSequence, todo: fix it
    def generate_maybe(me, pattern, result, previous_result, stream, failure):
        save = gensym("save")
        fail = lambda : """
%s = Result(%s)
%s.setValue(None)
""" % (result, save, result)

        data = """
%s = %s.getPosition()
%s
""" % (save, result, pattern.pattern.generate_python(result, previous_result, stream, fail))
        return data

    def generate_or(me, pattern, result, previous_result, stream, failure):
        data = ""
        fail = failure
        save = gensym("save")
        for next_pattern in pattern.patterns[::-1]:
            my_result = newResult()
            data = """
%s = Result(%s)
%s
""" % (result, save, next_pattern.generate_python(result, previous_result, stream, fail).strip())
            fail = lambda : data
        return """
%s = %s.getPosition()
%s
""" % (save, result, data)

    def generate_bind(me, pattern, result, previous_result, stream, failure):
        data = """
%s
%s = %s.getValues()
""" % (pattern.pattern.generate_python(result, previous_result, stream, failure).strip(), pattern.variable, result)
        return data

    def generate_range(me, pattern, result, previous_result, stream, failure):
        letter = gensym("letter")
        data = """
%s = %s.get(%s.getPosition())
if %s in '%s':
    %s.nextPosition()
    %s.setValue(%s)
else:
    %s
""" % (letter, stream, result, letter, pattern.range, result, result, letter, indent(failure()))

        return data

    def generate_verbatim(me, pattern, result, previous_result, stream, failure):
        def doString():
            length = len(pattern.letters)
            if special_char(pattern.letters):
                length = 1
            import re
            letters = re.sub(r"'", "\\'", pattern.letters)
            data = """
if '%s' == %s.get(%s.getPosition(), %s):
    %s.nextPosition(%s)
    %s.setValue('%s')
else:
    %s
""" % (letters, stream, result, length, result, length, result, letters, indent(failure()))
            return data
        def doAscii():
            data = """
if ord(%s.get(%s.getPosition())) == %s:
    %s.nextPosition()
    %s.setValue(%s);
else:
    %s
"""
            return data % (stream, result, pattern.letters, result, result, pattern.letters, indent(failure()))
        if type(pattern.letters) == type('x'):
            return doString()
        elif type(pattern.letters) == type(0):
            return doAscii()
        else:
            raise Exception("unknown verbatim value %s" % pattern.letters)

# all the self parameters are named me because the code was originally
# copied from another class and to ensure that copy/paste errors don't
# occur I have changed the name from 'self' to 'me'
# that is, 'self' in the original code is now the parameter 'pattern'
class CppGenerator(CodeGenerator):
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
Result %s(%s);
%s
%s
%s:
%s.setValue(Value((void*)0));
        """ % (my_result, result, pattern.next.generate_cpp(peg, my_result, stream, my_fail, None, peg_args).strip(), failure(), not_label, result)

        return data

    def generate_ensure(me, pattern, peg, result, stream, failure, tail, peg_args):
        my_result = newResult()
        data = """
Result %s(%s.getPosition());
%s
""" % (my_result, result, pattern.next.generate_cpp(peg, my_result, stream, failure, None, peg_args).strip())
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
%s = ((%s) %s)(%s, %s.getPosition()%s%s);
if (%s.error()){
    %s
}
""" % (result, cast, pattern.name, stream, result, rule_parameters, parameters, result, indent(failure()))

        return data

    def generate_predicate(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = """
{
    bool %s = true;
    %s
    if (!%s){
        %s
    }
}
""" % (pattern.variable, me.fixup_cpp(indent(pattern.code.strip()), peg_args), pattern.variable, failure())
        return data

    def generate_rule(me, pattern, peg, result, stream, failure, tail, peg_args):
        rule = peg.getRule(pattern.rule)
        if rule != None and rule.isInline():
            # TODO: add rule parameters and regular parameters for inlined rules
            if tail != None:
                raise Exception("Do not combine inlined rules that use tail recursion")
            def newPattern(pattern, stream, result, success):
                my_result = newResult()
                out = [False]
                def label(n):
                    if n != False:
                        return "%s:" % n
                    return ""

                def fail():
                    if out[0] == False:
                        out[0] = newOut()
                    return "goto %s;" % out[0]
                pattern_result = pattern.generate_cpp(peg, my_result, stream, fail, tail, peg_args).strip()
                data = """
{
Result %s(%s.getPosition());
%s
%s = %s;
}
%s
%s
                """ % (my_result, result, pattern_result, result, my_result, success, label(out[0]))

                data = """
{
    %s
}
%s
%s
""" % (indent(pattern.generate_cpp(peg, result, stream, fail, tail, peg_args)), success, label(out[0]))
                return data

            success_out = gensym('success')
            data = """
%s
%s
%s:
;
""" % ('\n'.join([newPattern(pattern, stream, result, "goto %s;" % success_out).strip() for pattern in rule.patterns]), failure(), success_out)
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
%s = rule_%s(%s, %s.getPosition()%s%s);
if (%s.error()){
    %s
}
""" % (result, pattern.rule, stream, result, rule_parameters, parameters, result, indent(failure()))

                return data

    def generate_void(me, pattern, peg, result, stream, failure, tail, peg_args):
        return ""

    def generate_eof(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = """
if ('\\0' == %s.get(%s.getPosition())){
    %s.nextPosition();
    %s.setValue(Value((void *) '\\0'));
} else {
    %s
}
""" % (stream, result, result, result, indent(failure()))
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
%s.reset();
do{
    Result %s(%s.getPosition());
    %s
    %s.addResult(%s);
} while (true);
%s:
if (%s.matches() == 0){
    %s
}
""" % (result, my_result, result, indent(pattern.next.generate_cpp(peg, my_result, stream, my_fail, tail, peg_args).strip()), result, my_result, loop_done, result, indent(failure()))

        return data

    def generate_code(me, pattern, peg, result, stream, failure, tail, peg_args):
        data = """
{
    Value value((void*) 0);
    %s
    %s.setValue(value);
}
        """ % (me.fixup_cpp(indent(pattern.code.strip()), peg_args), result)

        return data

    def generate_repeat_many(me, pattern, peg, result, stream, failure, tail, peg_args):
        loop_done = gensym("loop")
        my_fail = lambda : "goto %s;" % loop_done
        my_result = newResult()
        data = """
%s.reset();
do{
    Result %s(%s.getPosition());
    %s
    %s.addResult(%s);
} while (true);
%s:
;
        """ % (result, my_result, result, indent(pattern.next.generate_cpp(peg, my_result, stream, my_fail, tail, peg_args).strip()), result, my_result, loop_done)
        return data

    def generate_any(me, pattern, peg, result, stream, failure, tail, peg_args):
        temp = gensym()
        data = """
char %s = %s.get(%s.getPosition());
if (%s != '\\0'){
    %s.setValue(Value((void*) (long) %s));
    %s.nextPosition();
} else {
    %s
}
""" % (temp, stream, result, temp, result, temp, result, indent(failure()))
        return data

    def generate_maybe(me, pattern, peg, result, stream, failure, tail, peg_args):
        save = gensym("save")
        fail = lambda : """
%s = Result(%s);
%s.setValue(Value((void*) 0));
""" % (result, save, result)
        data = """
int %s = %s.getPosition();
%s
""" % (save, result, pattern.pattern.generate_cpp(peg, result, stream, fail, tail, peg_args))
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
Result %s(%s.getPosition());
%s
%s = %s;
}
goto %s;
%s:
""" % (my_result, result, pattern.generate_cpp(peg, my_result, stream, fail, tail, peg_args).strip(), result, my_result, success, out)
        data += "%s:\n" % success
        return data

    def generate_bind(me, pattern, peg, result, stream, failure, tail, peg_args):
        if isinstance(pattern.pattern, PatternLine):
            name = gensym("line_info");
            data = """
Stream::LineInfo %s = %s.getLineInfo(%s.getPosition());
%s = &%s;
""" % (name, stream, result, pattern.variable, name)
        else:
            data = """
%s
%s = %s.getValues();
""" % (pattern.pattern.generate_cpp(peg, result, stream, failure, tail, peg_args).strip(), pattern.variable, result)
        return data

    def generate_range(me, pattern, peg, result, stream, failure, tail, peg_args):
        letter = gensym("letter")
        data = """
char %s = %s.get(%s.getPosition());
if (%s != '\\0' && strchr("%s", %s) != NULL){
    %s.nextPosition();
    %s.setValue(Value((void*) (long) %s));
} else {
    %s
}
""" % (letter, stream, result, letter, pattern.range, letter, result, result, letter, indent(failure()))
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
%s.setValue(Value((void*) "%s"));
for (int i = 0; i < %d; i++){
    if (%s("%s"[i], %s.get(%s.getPosition()))){
        %s.nextPosition();
    } else {
        %s
    }
}
    """ % (result, pattern.letters.replace('"', '\\"'), length, comparison, pattern.letters.replace('"', '\\"'), stream, result, result, indent(indent(failure())))
            return data
        def doAscii():
            data = """
%s.setValue(Value((void*) %s));
if ((unsigned char) %s.get(%s.getPosition()) == (unsigned char) %s){
    %s.nextPosition();
} else {
    %s
}
"""
            return data % (result, pattern.letters, stream, result, pattern.letters, result, indent(failure()))

        if type(pattern.letters) == type('x'):
            return doString()
        elif type(pattern.letters) == type(0):
            return doAscii()
        else:
            raise Exception("unknown verbatim value %s" % pattern.letters)

class CppInterpreterGenerator(CodeGenerator):
    start_code = """
namespace Peg{

struct Value{
    typedef std::list<Value>::const_iterator iterator;

    Value():
        which(0),
        value(0),
        values(NULL){
    }

    Value(const Value & him):
    which(him.which),
    value(0),
    values(NULL){
        if (him.isData()){
            value = him.value;
        }

        if (him.isList()){
            values = him.values;
        }
    }

    Value(const void * value):
        which(0),
        value(value),
        values(NULL){
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

    inline std::list<Value*> & getValues(){
        which = 1;
        return values;
    }

    inline const std::list<Value> & getValues() const {
        throw 1;
        // return values;
    }

    ~Value(){
    }

    /*
    inline void setValues(std::list<Value> values){
        which = 1;
        values = values;
    }
    */

    const void * value;
    std::list<Value*> values;
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

    /*
    inline int matches() const {
        if (value.isList()){
            return this->value.values->size();
        } else {
            return 1;
        }
    }
    */

    inline const Value & getValues() const {
        return this->value;
    }

    void addResult(const Result & result){
        std::list<Value*> & mine = this->value.getValues();
        mine.push_back(new Value(result.getValues()));
        this->position = result.getPosition();
        this->value.which = 1;
    }

private:
    int position;
    Value value;
};

%s

class Stream;

enum RuleType{
    %s
};

class Expression{
public:
    virtual Result parse(Stream & stream, int position, Value ** arguments) = 0;
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
            return '\\0';
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

    std::vector<Expression*> * getRule(RuleType rule){
        return rules[rule];
    }

    void addRule(RuleType rule, std::vector<Expression*> * expressions){
        rules[rule] = expressions;
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

    std::string reportError(){
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
        int context = %d;
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

    ~Stream(){
        delete[] temp;
        for (int i = 0; i < memo_size; i++){
            delete memo[i];
        }
        delete[] memo;

        for (std::map<RuleType, std::vector<Expression*>*>::iterator it = rules.begin(); it != rules.end(); it++){
            std::vector<Expression*> * expressions = (*it).second;
            for (std::vector<Expression*>::iterator expression_it = expressions->begin(); expression_it != expressions->end(); expression_it++){
                Expression * expression = *expression_it;
                delete expression;
            }
            delete expressions;
        }
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
    std::map<RuleType, std::vector<Expression*>*> rules;
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

class Failure: public std::exception {
public:
    Failure(){
    }

    virtual ~Failure() throw () {
    }
};

class Or: public Expression {
public:
    Or(int elements, ...){
        va_list arguments;
        va_start(arguments, elements);
        for (int i = 0; i < elements; i++){
            all.push_back(va_arg(arguments, Expression*));
        }
        va_end(arguments);
    }

    std::vector<Expression*> all;

    virtual ~Or(){
        for (std::vector<Expression*>::iterator it = all.begin(); it != all.end(); it++){
            delete (*it);
        }
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        for (std::vector<Expression*>::iterator it = all.begin(); it != all.end(); it++){
            try{
                Expression * expression = *it;
                return expression->parse(stream, position, arguments);
            } catch (const Failure & ignore){
            }
        }
        throw Failure();
    }
};

class Sequence: public Expression {
public:
    Sequence(int elements, ...){
        va_list arguments;
        va_start(arguments, elements);
        for (int i = 0; i < elements; i++){
            all.push_back(va_arg(arguments, Expression*));
        }
        va_end(arguments);
    }

    std::vector<Expression*> all;

    virtual ~Sequence(){
        for (std::vector<Expression*>::iterator it = all.begin(); it != all.end(); it++){
            delete (*it);
        }
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        Result out(position);
        for (std::vector<Expression*>::iterator it = all.begin(); it != all.end(); it++){
            Expression * expression = *it;
            out.addResult(expression->parse(stream, out.getPosition(), arguments));
        }
        return out;
    }
};

class Bind: public Expression {
public:
    Bind(int index, Expression * next):
    index(index),
    next(next){
    }

    int index;
    Expression * next;

    virtual ~Bind(){
        delete next;
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        Result out = next->parse(stream, position, arguments);
        // *(arguments[index]) = out.getValues();
        return out;
    }
};

class Verbatim: public Expression {
public:
    Verbatim(const std::string & data):
    data(data),
    length(data.size()){
    }

    std::string data;
    int length;

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        for (int i = 0; i < length; i++){
            if (!compareChar(data[i], stream.get(position + i))){
                throw Failure();
            }
        }
        Result out(position + length);
        out.setValue((void*) data.c_str());
        return out;
    }
};

class RepeatOnce: public Expression {
public:
    RepeatOnce(Expression * next):
    next(next){
    }

    Expression * next;

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        Result out(position);
        out.addResult(next->parse(stream, position, arguments));
        try{
            while (true){
                out.addResult(next->parse(stream, out.getPosition(), arguments));
            }
        } catch (const Failure & ignore){
        }
        return out;
    }

    virtual ~RepeatOnce(){
        delete next;
    }
};

class RepeatMany: public Expression {
public:
    RepeatMany(Expression * next):
    next(next){
    }

    Expression * next;

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        Result out(position);
        try{
            while (true){
                out.addResult(next->parse(stream, out.getPosition(), arguments));
            }
        } catch (const Failure & ignore){
        }
        return out;
    }

    virtual ~RepeatMany(){
        delete next;
    }
};

class Maybe: public Expression {
public:
    Maybe(Expression * next):
    next(next){
    }

    Expression * next;

    virtual ~Maybe(){
        delete next;
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        try{
            return next->parse(stream, position, arguments);
        } catch (const Failure & ignore){
            return Result(position);
        }
    }
};

class Eof: public Expression {
public:
    Eof(){
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        if (stream.get(position) != 0){
            throw Failure();
        }
        return Result(position + 1);
    }
};

class Not: public Expression {
public:
    Not(Expression * next):
    next(next){
    }

    Expression * next;

    virtual ~Not(){
        delete next;
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        try{
            next->parse(stream, position, arguments);
            throw Failure();
        } catch (const Failure & fail){
            return Result(position);
        }
    }
};

class Void: public Expression {
public:
    Void(){
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        return Result(position);
    }
};

class Any: public Expression {
public:
    Any(){
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        if (stream.get(position) != 0){
            Result out(position + 1);
            out.setValue((void*) stream.get(position));
            return out;
        }

        throw Failure();
    }
};

class Ensure: public Expression {
public:
    Ensure(Expression * next):
    next(next){
    }

    Expression * next;

    virtual ~Ensure(){
        delete next;
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        next->parse(stream, position, arguments);
        return Result(position);
    }
};

class Line: public Expression {
public:
    Line(){
    }

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        /* FIXME */
        return Result(position);
    }
};

class Range: public Expression {
public:
    Range(const char * letters):
    letters(letters){
    }

    const char * letters;

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        char get = stream.get(position);
        if (strchr(letters, get) != NULL){
            Result out(position + 1);
            out.setValue((void*) get);
            return out;
        }

        throw Failure();
    }
};

class Code: public Expression {
public:
    typedef Value (*function)(Value ** arguments);

    Code(function call):
    call(call){
    }

    function call;

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        // Value value = call(arguments);
        Result out(position);
        // out.setValue(value);
        return out;
    }
};

class Rule: public Expression {
public:
    typedef Result (*rule_function)(Stream & stream, int position, Value ** arguments);
    Rule(rule_function function, ...):
    function(function){
    }

    rule_function function;

    virtual Result parse(Stream & stream, int position, Value ** arguments){
        return function(stream, position, arguments);
    }
};

} /* Peg */

typedef Peg::Result Result;
typedef Peg::Stream Stream;
typedef Peg::Value Value;
typedef Peg::ParseException ParseException;
typedef Peg::Column Column;
"""

    def __init__(self):
        self.extra_codes = []

    def addCode(self, name, code):
        data = """
Value %s(Value ** arguments){
    Value value;
    %s
    return value;
}
""" % (name,
       ""
       #code
       )
        self.extra_codes.append(data)

    def maybe_inline(self, pattern, rule, peg):
        if isinstance(pattern, PatternRule) and peg.getRule(pattern.rule).inline and pattern.rule != rule.name:
            return PatternOr(peg.getRule(pattern.rule).patterns)
        return pattern

    def generate_sequence(self, pattern, rule, peg):
        real_patterns = []
        for subpattern in pattern.patterns:
            real_patterns.append(self.maybe_inline(subpattern, rule, peg))
        if len(real_patterns) == 1:
            return real_patterns[0].generate_v3(self, rule, peg)
        data = "new Peg::Sequence(%d, %s)" % (len(pattern.patterns), ", ".join([subpattern.generate_v3(self, rule, peg) for subpattern in real_patterns]))
        return data

    def generate_bind(self, pattern, rule, peg):
        variable_index = 0
        data = "new Peg::Bind(%s, %s)" % (variable_index, pattern.pattern.generate_v3(self, rule, peg))
        return data

    def generate_repeat_once(self, pattern, rule, peg):
        data = "new Peg::RepeatOnce(%s)" % self.maybe_inline(pattern.next, rule, peg).generate_v3(self, rule, peg)
        return data

    def generate_repeat_many(self, pattern, rule, peg):
        data = "new Peg::RepeatMany(%s)" % self.maybe_inline(pattern.next, rule, peg).generate_v3(self, rule, peg)
        return data

    def generate_maybe(self, pattern, rule, peg):
        data = "new Peg::Maybe(%s)" % pattern.pattern.generate_v3(self, rule, peg)
        return data

    def generate_eof(self, pattern, rule, peg):
        data = "new Peg::Eof()"
        return data
    
    def generate_void(self, pattern, rule, peg):
        data = "new Peg::Void()"
        return data

    def generate_ensure(self, pattern, rule, peg):
        data = "new Peg::Ensure(%s)" % pattern.next.generate_v3(self, rule, peg)
        return data
    
    def generate_not(self, pattern, rule, peg):
        data = "new Peg::Not(%s)" % self.maybe_inline(pattern.next, rule, peg).generate_v3(self, rule, peg)
        return data
    
    def generate_any(self, pattern, rule, peg):
        data = "new Peg::Any()"
        return data

    def generate_line(self, pattern, rule, peg):
        data = "new Peg::Line()"
        return data

    def generate_range(self, pattern, rule, peg):
        data = 'new Peg::Range("%s")' % pattern.range
        return data

    def generate_verbatim(self, pattern, rule, peg):
        data = 'new Peg::Verbatim("%s")' % pattern.letters.replace('"', '\\"')
        return data

    def generate_code(self, pattern, rule, peg):
        function = gensym('code')
        self.addCode(function, pattern.code)
        data = 'new Peg::Code(%s)' % function
        return data

    def generate_rule(self, pattern, rule, peg):
        data = 'new Peg::Rule(rule_%s)' % pattern.rule
        return data

    def generate_or(self, pattern, rule, peg):
        data = 'new Peg::Or(%d, %s)' % (len(pattern.patterns), ", ".join([p.generate_v3(self, rule, peg) for p in pattern.patterns]))
        return data

# Thrown when an eof rule is encountered
class DoneGenerating(Exception):
    pass

# Generates random input based on the Peg
class TestGenerator(CodeGenerator):
    def generate_sequence(self, pattern, peg):
        def make(p):
            def doit(work):
                return p.generate_v2(self, peg)
            return doit
        return [make(p) for p in pattern.patterns]
        # return "".join([p.generate_v2(self, peg) for p in pattern.patterns])

    def generate_verbatim(self, pattern, peg):
        def make2(work):
            if pattern.letters in ["\\r"]:
                return "\n"
            if type(pattern.letters) == type(0):
                return chr(pattern.letters)
            else:
                return pattern.letters
        return [make2]

    def generate_eof(self, pattern, peg):
        def blah(work):
            raise DoneGenerating()
        return [blah]
        # return ""

    def generate_any(self, pattern, peg):
        return [lambda work: 'x']
        # return 'x'

    def generate_ensure(self, pattern, peg):
        def blah(work):
            # Throw the result out, but generate it for eof
            pattern.generate_v2(self, peg)
            return ""
        return [blah]
        #return ""

    def generate_rule(self, pattern, peg):
        # print "Generating rule %s" % pattern.rule
        def make3(work):
            rule = peg.getRule(pattern.rule)
            if work > 10 and rule.hasEmptyRule(peg):
                # print "Skipping rule %s" % rule.name
                return ""
            else:
                return rule.generate_test(self, peg)
        return [make3]
        # return peg.getRule(pattern.rule).generate_test(self, peg)

    def generate_bind(self, pattern, peg):
        def make4(work):
            return pattern.pattern.generate_v2(self, peg)
        return [make4]
        # return pattern.pattern.generate_v2(self, peg)

    def generate_range(self, pattern, peg):
        def make5(work):
            import random
            return random.choice(pattern.range)
        return [make5]

    def generate_line(self, pattern, peg):
        return [lambda work: ""]

    def generate_void(self, pattern, peg):
        return [lambda work: ""]

    def generate_maybe(self, pattern, peg):
        def make(work):
            import random
            if random.randint(0, 1) == 0:
                return pattern.pattern.generate_v2(self, peg)
            else:
                return ""
        return [make]

    def generate_repeat_many(self, pattern, peg):
        import random
        def make(p):
            def doit(work):
                return p.generate_v2(self, peg)
            return doit
        return [make(pattern.next) for x in [1] * random.randint(0, 4)]

        # return "".join([pattern.next.generate_v2(self, peg) for x in [1] * random.randint(0, 4)])

    def generate_repeat_once(self, pattern, peg):
        import random
        def make(p):
            def doit(work):
                return p.generate_v2(self, peg)
            return doit
        return [make(pattern.next) for x in [1] * random.randint(1, 4)]
        # return "".join([pattern.next.generate_v2(self, peg) for x in [1] * random.randint(1, 4)])

    def generate_code(self, pattern, peg):
        return [lambda work: ""]

    def generate_not(self, pattern, peg):
        return [lambda work: ""]

class Pattern:
    def __init__(self):
        pass

    # only true for the failure class
    def isFail(self):
        return False
    
    # true if this pattern is at the end of a sequence and calls a rule
    def tailRecursive(self, rule):
        return False

    def generate_v1(self, generator, result, previous_result, stream, failure):
        raise Exception("%s must override the `generate_v1' method to generate code" % (self.__class__))

    # generic code generation method. visitor should be a subclass of
    # CodeGenerator
    def generate(self, visitor, peg, result, stream, failure, tail, peg_args):
        raise Exception("Sub-classes must override the `generate' method to generate code")
    # utility method, probably move it elsewhere
    def parens(self, pattern, str):
        if pattern.contains() > 1:
            return "(%s)" % str
        else:
            return str

# Continues to parse only if the sub-pattern can be parsed, but no input is consumed
class PatternEnsure(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def ensureRules(self, find):
        self.next.ensureRules(find)

    def find(self, proc):
        def me():
            if proc(self):
                return [self]
            return []
        return me() + self.next.find(proc)

    def generate_bnf(self):
        return "&" + self.next.generate_bnf()

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_ensure(self, result, previous_result, stream, failure)

    # Takes some arguments
    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_ensure(self, result, previous_result, stream, failure)

    # Takes no arguments other than the generator
    def generate_v2(self, generator, peg):
        return generator.generate_ensure(self, peg)

    def generate_v3(self, generator, rule, peg):
        return generator.generate_ensure(self, rule, peg)

    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_ensure(self, peg, result, stream, failure, tail, peg_args)
        
class PatternNot(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def ensureRules(self, find):
        self.next.ensureRules(find)

    def find(self, proc):
        def me():
            if proc(self):
                return [self]
            return []
        return me() + self.next.find(proc)

    def canBeEmpty(self, peg):
        return True

    def generate_bnf(self):
        return "!" + self.next.generate_bnf()

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_not(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_not(self, peg, result, stream, failure, tail, peg_args)

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_not(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_not(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_not(self, rule, peg)
        
class PatternRule(Pattern):
    def __init__(self, rule, rules = None, parameters = None):
        Pattern.__init__(self)
        self.rule = rule
        self.rules = rules
        self.parameters = parameters

    def contains(self):
        return 1

    def tailRecursive(self, rule):
        return self.rule == rule.name

    def canBeEmpty(self, peg):
        return peg.getRule(self.rule).hasEmptyRule(peg)

    def find(self, proc):
        if proc(self):
            return [self]
        return []

    def generate_v2(self, generator, peg):
        return generator.generate_rule(self, peg)
        # return peg.getRule(self.rule).generate_test(generator, peg)

    def generate_v3(self, generator, rule, peg):
        return generator.generate_rule(self, rule, peg)

    def ensureRules(self, find):
        if not find(self.rule):
            print "*warning* could not find rule " + self.rule

    def generate_bnf(self):
        rules = ""
        values = ""
        if self.rules != None:
            rules = '[%s]' % ', '.join(self.rules)
        if self.parameters != None:
            values = '(%s)' % ', '.join(self.parameters)
        return '%s%s%s' % (self.rule, rules, values)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_rule(self, result, previous_result, stream, failure)
    
    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_rule(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_rule(self, peg, result, stream, failure, tail, peg_args)
    
class PatternVoid(Pattern):
    def __init__(self):
        Pattern.__init__(self)

    def ensureRules(self, find):
        pass

    def find(self, proc):
        if proc(self):
            return [self]
        return []
    
    def canBeEmpty(self, peg):
        return True

    def generate_bnf(self):
        return "<void>"

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_void(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_void(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_void(self, rule, peg)

    def generate_python(self, result, previous_result, stream, failure):
        return ""
    
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_void(self, peg, result, stream, failure, tail, peg_args)

class PatternEof(Pattern):
    def __init__(self):
        Pattern.__init__(self)

    def ensureRules(self, find):
        pass

    def find(self, proc):
        if proc(self):
            return [self]
        return []
    
    def canBeEmpty(self, peg):
        return True

    def generate_bnf(self):
        return "<eof>"

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_eof(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_eof(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_eof(self, rule, peg)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_eof(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_eof(self, peg, result, stream, failure, tail, peg_args)
        
class PatternSequence(Pattern):
    def __init__(self, patterns):
        Pattern.__init__(self)
        self.patterns = patterns

    def contains(self):
        return len(self.patterns)

    def tailRecursive(self, rule):
        return self.patterns[-1].tailRecursive(rule)

    def canBeEmpty(self, peg):
        for pattern in self.patterns:
            if not pattern.canBeEmpty(peg):
                return False
        return True

    def find(self, proc):
        def me():
            if proc(self):
                return [self]
            return []
        return flatten([p.find(proc) for p in self.patterns]) + me()

    def ensureRules(self, find):
        for pattern in self.patterns:
            pattern.ensureRules(find)

    def generate_bnf(self):
        return "%s" % " ".join([p.generate_bnf() for p in self.patterns])

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_sequence(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_sequence(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_sequence(self, rule, peg)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_sequence(self, result, previous_result, stream, failure)

    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_sequence(self, peg, result, stream, failure, tail, peg_args)

class PatternCallRule(Pattern):
    def __init__(self, name, rules, values):
        Pattern.__init__(self)
        self.name = name
        self.rules = rules
        self.values = values
    
    def ensureRules(self, find):
        pass

    def find(self, proc):
        if proc(self):
            return [self]
        return []
    
    def generate_bnf(self):
        rules = ""
        values = ""
        if self.rules != None:
            rules = '[%s]' % ', '.join(self.rules)
        if self.values != None:
            values = '(%s)' % ', '.join(self.values)
        return '@%s%s%s' % (self.name, rules, values)
    
    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_call_rule(self, result, previous_result, stream, failure)
    
    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_call_rule(self, result, previous_result, stream, failure)
    
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_call_rule(self, peg, result, stream, failure, tail, peg_args)
        
class PatternRepeatOnce(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def ensureRules(self, find):
        self.next.ensureRules(find)

    def find(self, proc):
        def me():
            if proc(self):
                return [self]
            return []
        return me() + self.next.find(proc)

    def canBeEmpty(self, peg):
        return False

    def generate_bnf(self):
        return self.parens(self.next, self.next.generate_bnf()) + "+"

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_repeat_once(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_repeat_once(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_repeat_once(self, rule, peg)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_repeat_once(self, result, previous_result, stream, failure)

    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_repeat_once(self, peg, result, stream, failure, tail, peg_args)
        
class PatternCode(Pattern):
    def __init__(self, code):
        Pattern.__init__(self)
        self.code = code

    def contains(self):
        return 1

    def find(self, proc):
        if proc(self):
            return [self]
        return []

    def canBeEmpty(self, peg):
        return True

    def ensureRules(self, find):
        pass

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_code(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_code(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_code(self, rule, peg)

    def generate_bnf(self):
        return """{{%s}}""" % (self.code)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_code(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_code(self, peg, result, stream, failure, tail, peg_args)

class PatternRepeatMany(Pattern):
    def __init__(self, next):
        Pattern.__init__(self)
        self.next = next

    def ensureRules(self, find):
        self.next.ensureRules(find)

    def find(self, proc):
        def me():
            if proc(self):
                return [self]
            return []
        return me() + self.next.find(proc)

    def canBeEmpty(self, peg):
        return True
    
    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_repeat_many(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_repeat_many(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_repeat_many(self, rule, peg)

    def generate_bnf(self):
        return self.parens(self.next, self.next.generate_bnf()) + "*"

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_repeat_many(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_repeat_many(self, peg, result, stream, failure, tail, peg_args)
        
class PatternAny(Pattern):
    def __init__(self):
        Pattern.__init__(self)

    def find(self, proc):
        if proc(self):
            return [self]
        return []

    def generate_bnf(self):
        return "."

    def ensureRules(self, find):
        pass

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_any(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_any(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_any(self, rule, peg)

    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_any(self, peg, result, stream, failure, tail, peg_args)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_any(self, result, previous_result, stream, failure)
        
class PatternMaybe(Pattern):
    def __init__(self, pattern):
        Pattern.__init__(self)
        self.pattern = pattern

    def ensureRules(self, find):
        self.pattern.ensureRules(find)

    def find(self, proc):
        def me():
            if proc(self):
                return [self]
            return []
        return me() + self.pattern.find(proc)

    def canBeEmpty(self, peg):
        return True
    
    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_maybe(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_maybe(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_maybe(self, rule, peg)

    def generate_bnf(self):
        return self.parens(self.pattern, self.pattern.generate_bnf()) + "?"

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_maybe(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_maybe(self, peg, result, stream, failure, tail, peg_args)
        
class PatternOr(Pattern):
    def __init__(self, patterns):
        Pattern.__init__(self)
        self.patterns = patterns

    def contains(self):
        return 1

    def ensureRules(self, find):
        for pattern in self.patterns:
            pattern.ensureRules(find)

    def generate_bnf(self):
        return "or"

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_or(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_or(self, peg, result, stream, failure, tail, peg_args)

    def generate_v3(self, generator, rule, peg):
        return generator.generate_or(self, rule, peg)
        
class PatternBind(Pattern):
    def __init__(self, variable, pattern):
        Pattern.__init__(self)
        self.variable = variable
        self.pattern = pattern

    def ensureRules(self, find):
        self.pattern.ensureRules(find)

    def find(self, proc):
        def me():
            if proc(self):
                return [self]
            return []
        return me() + self.pattern.find(proc)

    def canBeEmpty(self, peg):
        return self.pattern.canBeEmpty(peg)

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_bind(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_bind(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_bind(self, rule, peg)

    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_bind(self, peg, result, stream, failure, tail, peg_args)
        
    def generate_bnf(self):
        return "%s:%s" % (self.variable, self.pattern.generate_bnf())

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_bind(self, result, previous_result, stream, failure)
        
def PatternUntil(pattern):
    return PatternRepeatMany(PatternSequence([
        PatternNot(pattern),
        PatternAny()
        ]))

class PatternRange(Pattern):
    def __init__(self, range):
        Pattern.__init__(self)
        self.range = range

    def find(self, proc):
        if proc(self):
            return [self]
        return []

    def ensureRules(self, find):
        pass

    def canBeEmpty(self, peg):
        return False

    def generate_bnf(self):
        return "[%s]" % self.range

    def generate_v2(self, generator, peg):
        return generator.generate_range(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_range(self, rule, peg)

    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_range(self, peg, result, stream, failure, tail, peg_args)
        
    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_range(self, result, previous_result, stream, failure)

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_range(self, result, previous_result, stream, failure)

class PatternLine(Pattern):
    def __init__(self):
        Pattern.__init__(self)

    def ensureRules(self, find):
        pass

    def find(self, proc):
        if proc(self):
            return [self]
        return []
    
    def contains(self):
        return 1

    def canBeEmpty(self, peg):
        return True

    def generate_bnf(self):
        return '<item>'

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_line(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_line(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_line(self, rule, peg)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_line(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_line(self, peg, result, stream, failure, tail, peg_args)

class PatternPredicate(Pattern):
    def __init__(self, variable, code):
        Pattern.__init__(self)
        self.variable = variable
        self.code = code
    
    def ensureRules(self, find):
        pass

    def find(self, proc):
        if proc(self):
            return [self]
        return []

    def canBeEmpty(self, peg):
        return True

    def contains(self):
        return 1

    def generate_bnf(self):
        return '<predicate %s> {{%s}}"' % (self.variable, self.code)

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_predicate(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_predicate(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_predicate(self, rule, peg)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_predicate(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_predicate(self, peg, result, stream, failure, tail, peg_args)


class PatternVerbatim(Pattern):
    def __init__(self, letters, options = None):
        Pattern.__init__(self)
        self.letters = letters
        self.options = options

    def ensureRules(self, find):
        pass

    def find(self, proc):
        if proc(self):
            return [self]
        return []

    def canBeEmpty(self, peg):
        return False

    def contains(self):
        return 1

    def generate_bnf(self):
        return '"%s"' % self.letters

    def generate_v1(self, generator, result, previous_result, stream, failure):
        return generator.generate_verbatim(self, result, previous_result, stream, failure)

    def generate_v2(self, generator, peg):
        return generator.generate_verbatim(self, peg)
    
    def generate_v3(self, generator, rule, peg):
        return generator.generate_verbatim(self, rule, peg)

    def generate_python(self, result, previous_result, stream, failure):
        return PythonGenerator().generate_verbatim(self, result, previous_result, stream, failure)
        
    def generate_cpp(self, peg, result, stream, failure, tail, peg_args):
        return CppGenerator().generate_verbatim(self, peg, result, stream, failure, tail, peg_args)
        
class Rule:
    def __init__(self, name, patterns, rules = None, inline = False, parameters = None, fail = None):
        self.name = name
        self.patterns = patterns
        self.inline = inline
        self.fail = fail
        self.rules = rules
        self.parameters = parameters

    def isInline(self):
        return self.inline

    def generate_bnf(self):
        data = """
%s = %s
""" % (self.name, (('\n%s | ') % (' ' * len(self.name))).join([p.generate_bnf() for p in self.patterns]))
        if self.inline:
            return "inline " + data.strip() + "\n"
        return data.strip() + "\n"

    def ensureRules(self, find):
        for pattern in self.patterns:
            pattern.ensureRules(find)

    def generate_ruby(self):
        def newPattern(pattern, stream, position):
            result = newResult()
            rule_id = "RULE_%s" % self.name

            def fail():
                return "raise PegError"
            data = """
begin
    %s = Result.new(%s)
    %s
    %s.update(%s, %s, %s)
    return %s
rescue PegError
end
            """ % (result, position, indent(pattern.generate_v1(RubyGenerator(), result, None, stream, fail).strip()), stream, rule_id, position, result, result)
            return data


        stream = "stream"
        position = "position"
        rule_id = "RULE_%s" % self.name
        rule_parameters = ""
        if self.rules != None:
            rule_parameters = ", " + ", ".join(["%s" % p for p in self.rules])
        parameters = ""
        if self.parameters != None:
            parameters = ", " + ", ".join(["%s" % p for p in self.parameters])

        data = """
def rule_%s(%s, %s%s%s)
    if %s.hasResult(%s, %s)
        return %s.result(%s, %s)
    end
    %s
    %s.update(%s, %s, nil)
    return nil
end
""" % (self.name, stream, position, rule_parameters, parameters, stream, rule_id, position, stream, rule_id, position, indent('\n'.join([newPattern(pattern, stream, position).strip() for pattern in self.patterns])), stream, rule_id, position)
        return data

    def choosePattern(self):
        import random
        return random.choice(self.patterns)

    def hasEmptyRule(self, peg):
        for pattern in self.patterns:
            if pattern.canBeEmpty(peg):
                return True
        return False

    def generate_test(self, generator, peg):
        return self.choosePattern().generate_v2(generator, peg)

    def generate_python(self):
        def newPattern(pattern, stream, position):
            result = newResult()

            def fail():
                return "raise PegError"
            data = """
try:
    %s = Result(%s)
    %s
    %s.update(%s, %s, %s)
    return %s
except PegError:
    pass
            """ % (result, position, indent(pattern.generate_python(result, None, stream, fail).strip()), stream, "RULE_%s" % self.name, position, result, result)
            return data

        stream = "stream"
        position = "position"
        rule_parameters = ""
        if self.rules != None:
            rule_parameters = ", " + ", ".join(["%s" % p for p in self.rules])
        parameters = ""
        if self.parameters != None:
            parameters = ", " + ", ".join(["%s" % p for p in self.parameters])
        data = """
def rule_%s(%s, %s%s%s):
    if %s.hasResult(%s, %s):
        return %s.result(%s, %s)
    %s
    %s.update(%s, %s, %s)
    return None
""" % (self.name, stream, position, rule_parameters, parameters, stream, "RULE_%s" % self.name, position, stream, "RULE_%s" % self.name, position, indent('\n'.join([newPattern(pattern, stream, position).strip() for pattern in self.patterns])), stream, "RULE_%s" % self.name, position, "None")

        return data

    def generate_cpp_interpreter(self, peg, chunk_accessor):
        def updateChunk(new, columnVar, memo):
            if not memo:
                return "%s.update(%s.getPosition());" % (stream, new)
            chunk = chunk_accessor.getChunk(columnVar)
            data = """
if (%s == 0){
    %s = new Peg::%s();
}
%s = %s;
stream.update(%s.getPosition());
""" % (chunk, chunk, chunk_accessor.getType(), chunk_accessor.getValue(chunk), new, new)
            return data
            
        columnVar = gensym("column")

        def hasChunk(memo):
            if memo:
                return """
Column & %s = stream.getColumn(position);
if (%s != 0 && %s.calculated()){
    if (%s.error()){
        throw Peg::Failure();
    }
    return %s;
}
""" % (columnVar, chunk_accessor.getChunk(columnVar), chunk_accessor.getValue(chunk_accessor.getChunk(columnVar)), chunk_accessor.getValue(chunk_accessor.getChunk(columnVar)), chunk_accessor.getValue(chunk_accessor.getChunk(columnVar)))
            else:
                return ""

        generator = CppInterpreterGenerator()
        patterns = "\n".join(["expressions->push_back(%s);" % pattern.generate_v3(generator, self, peg) for pattern in self.patterns])
        extra = '\n'.join(generator.extra_codes)
        data = """
%s
std::vector<Peg::Expression*> * create_rule_%s(){
    std::vector<Peg::Expression*> * expressions = new std::vector<Peg::Expression*>();
    %s
    return expressions;
}

Result rule_%s(Stream & stream, int position, Value ** arguments){
    %s
    std::vector<Peg::Expression*> * expressions = stream.getRule(Peg::Rule_%s);
    for (std::vector<Peg::Expression*>::const_iterator it = expressions->begin(); it != expressions->end(); it++){
        try{
            Peg::Expression * expression = *it;
            Result out = expression->parse(stream, position, arguments);
            %s
            return out;
        } catch (const Peg::Failure & failure){
            /* try next rule.. */
        }
    }
    throw Peg::Failure();
    // return Peg::errorResult;
}
""" % (extra, self.name, indent(patterns), self.name, indent(hasChunk(True)), self.name, indent(indent(indent(updateChunk('out', columnVar, True)))))
        return data

    def generate_cpp(self, peg, chunk_accessor):
        resetGensym()
        rule_number = "RULE_%s" % self.name
        stream = "stream"
        position = "position"
        # tail_loop = [gensym("tail")]
        tail_loop = [False]
        debug = "debug1" in peg.options
        
        def updateChunk(new, columnVar, memo):
            if not memo:
                return "%s.update(%s.getPosition());" % (stream, new)
            chunk = chunk_accessor.getChunk(columnVar)
            data = """
if (%s == 0){
    %s = new %s();
}
%s = %s;
%s.update(%s.getPosition());
""" % (chunk, chunk, chunk_accessor.getType(), chunk_accessor.getValue(chunk), new, stream, new)
            return data
            
        columnVar = gensym("column")

        def hasChunk(memo):
            if memo:
                return """
Column & %s = %s.getColumn(%s);
if (%s != 0 && %s.calculated()){
    return %s;
}
""" % (columnVar, stream, position, chunk_accessor.getChunk(columnVar), chunk_accessor.getValue(chunk_accessor.getChunk(columnVar)), chunk_accessor.getValue(chunk_accessor.getChunk(columnVar)))
            else:
                return ""
        
        def newPattern(pattern, stream, position):
            result = newResult()
            out = [False]

            def label(n):
                if n != False:
                    return "%s:" % n
                return ""

            def failure():
                if out[0] == False:
                    out[0] = newOut()
                return "goto %s;" % out[0]
            
            def invalid_arg(d):
                raise Exception("No results available")

            if pattern.tailRecursive(self):
                tail_vars = self.parameters
                if tail_vars == None:
                    tail_vars = []
                if tail_loop[0] == False:
                    tail_loop[0] = gensym("tail")
                data = """
Result %s(%s);
%s
%s = %s.getPosition();
goto %s;
%s
    """ % (result, position, pattern.generate_cpp(peg, result, stream, failure, tail_vars, invalid_arg).strip(), position, result, tail_loop[0], label(out[0]))
            else:
                # non-tail so dont make the tail label
                debugging = ""
                debug_result = ""
                if debug:
                    debugging = """std::cout << "Trying rule %s at " << %s << " '" << %s.get(%s.getPosition()) << "' alternative: %s" << std::endl;""" % (self.name, position, stream, result, special_escape(pattern.generate_bnf()).replace("\n", "\\n"))
                if 'debug2' in peg.options:
                    debug_result = """std::cout << "Succeeded rule %s at position " << %s.getPosition() << " alternative: %s" << std::endl;""" % (self.name, result, special_escape(pattern.generate_bnf()).replace("\n", "\\n"))
                do_memo = peg.memo and self.rules == None and self.parameters == None
                data = """
Result %s(%s);
%s
%s
%s
%s
return %s;
%s
            """ % (result, position, debugging, pattern.generate_cpp(peg, result, stream, failure, None, invalid_arg).strip(), updateChunk(result, columnVar, do_memo), debug_result, result, label(out[0]))

            return data

        rule_parameters = ""
        if self.rules != None:
            # rule_parameters = ", " + ", ".join(["Result (*%s)(Stream &, const int, ...)" % p for p in self.rules])
            rule_parameters = ", " + ", ".join(["void * %s" % p for p in self.rules])

        parameters = ""
        if self.parameters != None:
            parameters = ", " + ", ".join(["Value %s" % p for p in self.parameters])

        def findVars():
            def isBind(pattern):
                return isinstance(pattern, PatternBind)
            bind_patterns = flatten([p.find(isBind) for p in self.patterns])
            return unique([p.variable for p in bind_patterns])

        def declareVar(var):
            return "Value %s;" % var

        vars = "\n".join([declareVar(v) for v in findVars()])
        my_position = "myposition"

        fail_code = ""
        if self.fail != None:
            fail_code = self.fail

        def label(n):
            if n != False:
                return "%s:" % n
            return ""
        
        pattern_results = indent('\n'.join([newPattern(pattern, stream, my_position).strip() for pattern in self.patterns]))

        # Don't memoize if the rule accepts parameters
        do_memo = peg.memo and self.rules == None and self.parameters == None
        body = """
%s
%s
%s
%s
""" % (label(tail_loop[0]), indent(vars), pattern_results, indent(updateChunk("errorResult", columnVar, do_memo)))

        if self.fail != None:
            body = """
try{
    %s
} catch (...){
    %s
    throw;
}
""" % (body, self.fail)

        data = """
Result rule_%s(Stream & %s, const int %s%s%s){
    %s
    RuleTrace %s(%s, "%s");
    int %s = %s;
    %s
    return errorResult;
}
        """ % (self.name, stream, position, rule_parameters, parameters, indent(hasChunk(do_memo)), gensym("trace"), stream, self.name, my_position, position, indent(body))

        return data

# Getting values from chunks
class Accessor:
    def __init__(self, chunk, value, type, rule):
        self.chunk = chunk
        self.value = value
        self.rule = rule
        self.type = type

    def getChunk(self, code):
        return code + self.chunk

    def getType(self):
        return self.type

    def getValue(self, code):
        return code + self.value
    
class Peg:
    def __init__(self, start, include_code, more_code, module, rules, options):
        self.start = start
        self.rules = rules
        self.include_code = include_code
        self.more_code = more_code
        self.module = module
        self.options = options
        # Whether to memoize or not
        self.memo = True
        if options == None:
            self.options = []
        if self.module == None:
            self.module = ['Parser']
        if 'no-memo' in self.options:
            self.memo = False
        # Default error length
        self.error_size = 15
        for option in self.options:
            import re
            length = re.compile('error-length (\d+)')
            match = length.match(option)
            if match:
                self.error_size = int(match.group(1))

        for rule in self.rules:
            rule.ensureRules(lambda r: r in [r2.name for r2 in self.rules])

    def getRule(self, name):
        for rule in self.rules:
            if rule.name == name:
                return rule
        return None

    def generate_ruby(self):

        use_rules = self.rules
        rule_numbers = '\n'.join(["RULE_%s = %d" % (x[0].name, x[1]) for x in zip(use_rules, range(0, len(use_rules)))])

        data = """
%s

%s

%s

def parse(file)
    stream = Stream.new(file)
    out = rule_%s(stream, 0)
    stream.close()
    return out.getValues()
end
""" % (start_ruby, rule_numbers, '\n'.join([rule.generate_ruby() for rule in self.rules]), self.start)
        return data

    def generate_test(self):
        # return self.getRule(self.start).generate_test(TestGenerator(), self)
        work = self.getRule(self.start).generate_test(TestGenerator(), self)
        data = ""
        length = 0
        while work:
            head = work.pop()
            length -= 1
            try:
                more = head(length)
            except DoneGenerating:
                more = ""
                data = ""
            # print "More is %s" % more
            if type(more) == type([]):
                work.extend(more)
                length += len(more)
            else:
                data = more + data
        return data

    def generate_python(self):
        # use_rules = [rule for rule in self.rules if not rule.isInline()]
        use_rules = self.rules
        rule_numbers = '\n'.join(["RULE_%s = %d" % (x[0].name, x[1]) for x in zip(use_rules, range(0, len(use_rules)))])

        top_code = ""
        if self.include_code != None:
            top_code = self.include_code

        more_code = ""
        if self.more_code != None:
            more_code = self.more_code

        data = """
import peg

%s

%s

%s

%s

%s

def doParse(stream):
    done = rule_%s(stream, 0)
    if done == None:
        # print "Error parsing " + file
        stream.reportError()
        return None
    else:
        return done.getValues()

def parseFile(file):
    # print "Parsing " + file
    return doParse(Stream(filename = file))

def parseString(value):
    return doParse(Stream(input = value))

    """ % (top_code, start_python, rule_numbers, more_code, '\n'.join([rule.generate_python() for rule in self.rules]), self.start)

        return data

    def generate_bnf(self):
        more = ""
        if self.include_code != None:
            more = """include: {{%s}}""" % self.include_code
        code = ""
        if self.more_code != None:
            code = """code: {{%s}}""" % self.more_code
        data = """
start-symbol: %s
%s
%s
rules:
    %s
""" % (self.start, more, code, indent('\n'.join([rule.generate_bnf() for rule in self.rules]).strip()))
        return data

    def cppNamespaceStart(self):
        start = ""
        for module in reverse(self.module):
            start = """
namespace %s{
%s
""" % (module, indent(start))
        return start

    def cppNamespaceEnd(self):
        end = ""
        for module in self.module:
            end = """
%s
} /* %s */
""" % (indent(end), module)
        return end

    def generate_cpp_interpreter(self):
        chunk_accessors = []
        def findAccessor(rule):
            for accessor in chunk_accessors:
                if accessor.rule == rule:
                    return accessor
            raise Exception("Cannot find accessor for " + rule.name)

        def makeChunks(rules):
            import math

            values_per_chunk = 5
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
struct %s{
    %s
};
""" % (name, indent("\n".join(["Result chunk_%s;" % rule.name for rule in values])))
                all.append(name)
                pre += value_data

            def sumChunk(chunk):
                data = """
(%s != NULL ? (%s) : 0)
""" % (chunk, '\n+ '.join(["(%s->chunk_%s.calculated() ? 1 : 0)" % (chunk, rule.name) for rule in chunk_to_rules[chunk]]))
                return data

            hit_count = '+'.join([sumChunk(chunk) for chunk in chunk_to_rules.keys()])

            data = """
%s
struct Column{
    Column():
        %s{
    }

    %s

    int hitCount(){
        return %s;
    }

    int maxHits(){
        return %s;
    }

    ~Column(){
        %s
    }
};
""" % (pre, indent(indent("\n,".join(["%s(0)" % x.lower() for x in all]))), indent("\n".join(["%s * %s;" % (x, x.lower()) for x in all])), hit_count, len(rules), indent(indent("\n".join(["delete %s;" % x.lower() for x in all]))))

            return data

        chunks = makeChunks(self.rules)

        rule_types = ",\n".join(["Rule_%s" % rule.name for rule in self.rules])

        prototypes = "\n".join(["Result rule_%s(Stream &, int, Value ** arguments);" % rule.name for rule in self.rules])

        data = """
%s
%s
""" % (CppInterpreterGenerator.start_code % (chunks, rule_types, self.error_size), prototypes)
        rules = "\n".join([rule.generate_cpp_interpreter(self, findAccessor(rule)) for rule in self.rules])

        setup_rules = "\n".join(["stream.addRule(Peg::Rule_%s, create_rule_%s());" % (rule.name, rule.name) for rule in self.rules])

        main = """
static const void * doParse(Stream & stream, bool stats, const std::string & context){
    %s
    Peg::errorResult.setError();
    Result done = rule_%s(stream, 0, NULL);
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

""" % (indent(setup_rules), self.start)

        top_code = ""
        if self.include_code != None:
            top_code = self.include_code
        more_code = ""
        if self.more_code != None:
            more_code = self.more_code

        namespace_start = self.cppNamespaceStart()
        namespace_end = self.cppNamespaceEnd()

        data = """
#include <list>
#include <stdarg.h>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <iostream>

%s
%s
%s
%s
%s
%s
%s
""" % (top_code, namespace_start, data, more_code, rules, main, namespace_end)
        return data

    def list_files(self, name, directory = '.'):
        use_rules = [rule for rule in self.rules if not rule.isInline()]
        out = []
        for rule in use_rules:
            file = '%s/%s-%s.cpp' % (directory, name, rule.name)
            out.append(file)
        return out

    def print_list_files(self, name):
        return '\n'.join(self.list_files(name))

    def generate_cpp(self, parallel = False, separate = None, directory = '.', main = False):
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

            values_per_chunk = 5
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
struct %s{
    %s
};
""" % (name, indent("\n".join(["Result chunk_%s;" % rule.name for rule in values])))
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
%s
struct Column{
    Column():
        %s{
    }

    %s

    int hitCount(){
        return %s;
    }

    int maxHits(){
        return %s;
    }

    ~Column(){
        %s
    }
};
""" % (pre, indent(indent("\n,".join(["%s(0)" % x.lower() for x in all]))), indent("\n".join(["%s * %s;" % (x, x.lower()) for x in all])), hit_count, len(rules), indent(indent("\n".join(["delete %s;" % x.lower() for x in all]))))

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

        def singleFile():
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

%s
%s

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

%s

%s

%s

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
        """ % (top_code, namespace_start, start_cpp_code % (chunks, self.error_size), '\n'.join([prototype(rule) for rule in use_rules]), more_code, '\n'.join([rule.generate_cpp(self, findAccessor(rule)) for rule in use_rules]), self.start, namespace_end)
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

def test():
    s_code = """
printf("parsed cheese\\n");
value = (void *) 2;
"""
    rules = [
        Rule("s", [PatternNot(PatternVerbatim("hello")), PatternAction(PatternVerbatim("cheese"), s_code), PatternRepeatOnce(PatternVerbatim("once"))]),
        Rule("blah", [PatternRepeatMany(PatternRule("s"))]),
        Rule("or", [PatternOr([PatternVerbatim("joe"), PatternVerbatim("bob"), PatternVerbatim("sally")])]),
        Rule("all", [PatternSequence([PatternVerbatim("abc"), PatternVerbatim("def"), PatternVerbatim("ghi")])]),
    ]
    peg = Peg("Peg", "s", rules)
    print peg.generate_cpp()

def create_peg(peg):
    # import imp
    # module = imp.new_module(peg.namespace)
    # exec peg.generate_python() in module.__dict__
    # return module.parse

    name = "peg_" + '_'.join(peg.module)
    out = open(name + ".py", 'w')
    out.write(peg.generate_python())
    out.close()
    module = __import__(name, globals(), locals(), ['parse'])
    # print module
    # print dir(module)
    return module.parseFile

def test2():
    start_code_abc = """
std::cout << "Parsed abc!" << std::endl;
"""
    start_code_def = """
std::cout << "Parsed def!" << std::endl;
"""
    rules = [
        Rule("start", [
            PatternAction(PatternSequence([PatternRule("a"),PatternRule("b"), PatternRule("c")]), start_code_abc),
            PatternAction(PatternSequence([PatternRule("d"),PatternRule("e"), PatternRule("f")]), start_code_def),
        ]),
        Rule("a", [PatternVerbatim("a")]),
        Rule("b", [PatternVerbatim("b")]),
        Rule("c", [PatternVerbatim("c")]),

        Rule("d", [PatternVerbatim("d")]),
        Rule("e", [PatternVerbatim("e")]),
        Rule("f", [PatternVerbatim("f")]),
    ]

    peg = Peg("Peg", "start", rules)
    print peg.generate_cpp()

# BNF for parsing BNF description
# This bootstraps the system so we can write normal BNF rules in a file
def peg_bnf(peg_name):
    rules = [
        Rule("start", [
            PatternSequence([
                PatternRule("newlines"),
                PatternRule("whitespace"),
                PatternBind("start_symbol", PatternRule("start_symbol")),
                PatternRule("newlines"),
                PatternRule("whitespace"),
                PatternBind("options", PatternMaybe(PatternRule("options"))),
                PatternRule("newlines"),
                PatternRule("whitespace"),
                PatternBind("module", PatternMaybe(PatternRule("module"))),
                PatternRule("newlines"),
                PatternRule("whitespace"),
                PatternBind('include', PatternMaybe(PatternRule("include"))),
                PatternRule("newlines"),
                PatternRule("whitespace"),
                PatternBind("code", PatternMaybe(PatternRule("more_code"))),
                PatternRule("newlines"),
                PatternRule("whitespace"),
                PatternBind("rules", PatternRule("rules")),
                PatternRule("newlines"),
                PatternRule("whitespace"),
                PatternEof(),

                PatternCode("""value = peg.Peg(start_symbol, include, code, module, rules, options)""")
                ]),
            ]),
        Rule('module', [
            PatternSequence([
                PatternVerbatim("module:"),
                PatternRule("spaces"),
                PatternBind("name", PatternRule("word")),
                PatternBind("rest", 
                    PatternRepeatMany(PatternSequence([
                        PatternVerbatim("."),
                        PatternRule("word"),
                        PatternCode("""value = $2"""),
                        ]))),
                    PatternCode("""value = [name] + rest"""),
                    ]),
            ]),
        Rule("include", [
            PatternSequence([
                PatternVerbatim("include:"),
                PatternRule("spaces"),
                PatternBind("code", PatternRule("code")),
                PatternCode("value = code.code"),
                ]),
            ]),
        Rule("more_code", [
            PatternSequence([
                PatternVerbatim("code:"),
                PatternRule("spaces"),
                PatternBind("code", PatternRule("code")),
                PatternCode("""value = code.code"""),
                ]),
            ]),
        Rule("options", [
                PatternSequence([
                    PatternVerbatim("options:"),
                    PatternRule("spaces"),
                    PatternBind("option1", PatternRule("option")),
                    PatternBind("option_rest", PatternRepeatMany(PatternSequence([
                        PatternRule("spaces"),
                        PatternVerbatim(","),
                        PatternRule("spaces"),
                        PatternRule("option"),
                        ]))),
                    PatternCode("""
value = []
for option in ([option1] + option_rest):
    import re
    debug = re.compile("debug(\d+)")
    out = debug.match(option)
    if out != None:
        num = int(out.group(1))
        for x in xrange(1,num+1):
            value.append('debug%d' % x)
    elif option == 'no-memo':
        value.append(option)
    else:
        value.append(option)
"""),
                    ]),
            ]),
        Rule("option", [
            PatternSequence([
                PatternVerbatim("debug"),
                PatternBind('number', PatternRule("number")),
                PatternCode("""
value = 'debug%s' % number
"""),
                ]),
            PatternVerbatim('no-memo'),
            PatternRule('error_option')
            ]),
        Rule('error_option', [
            PatternSequence([
                PatternVerbatim('error-length'),
                PatternRule('whitespace'),
                PatternBind('number', PatternRule("number")),
                PatternCode("""
value = 'error-length %s' % number
""")
                ]),
            ]),
        Rule("word", [
            PatternSequence([
                PatternRepeatOnce(PatternRule("any_char")),
                PatternCode("""
# print "all start symbol values " + str(values)
# print "values[0] " + str(values[0])
value = ''.join(values[0]).replace('-', '__')
# print "got word " + value
""")
                ]),
            ]),
        Rule("rules", [
            PatternSequence([
                PatternVerbatim("rules:"),
                PatternRule("whitespace"),
                PatternBind("rules", PatternRepeatMany(PatternSequence([
                    PatternRule("rule"),
                    PatternRule("whitespace"),
                    PatternCode("""value = $1"""),
                    ]))),
                PatternCode("""value = rules"""),
                ]),
            ]),
        Rule("rule", [
            PatternSequence([
                PatternRule("spaces"),
                PatternBind("inline", PatternMaybe(PatternVerbatim("inline"))),
                PatternRule("spaces"),
                PatternBind("name", PatternRule("word")),
                PatternBind("rule_parameters", PatternMaybe(PatternRule('rule_parameters'))),
                PatternBind("parameters", PatternMaybe(PatternRule('value_parameters'))),
                PatternRule("spaces"),
                PatternVerbatim("="),
                PatternRule("spaces"),
                PatternBind("pattern1", PatternRule("pattern_line")),
                PatternRule("whitespace"),
                PatternBind("patterns",
                    PatternRepeatMany(PatternSequence([
                        PatternRule("spaces"),
                        PatternVerbatim("|"),
                        PatternRule("spaces"),
                        PatternBind("pattern", PatternRule("pattern_line")),
                        # PatternBind("pattern", PatternRepeatMany(PatternRule("pattern"))),
                        PatternRule("whitespace"),
                        PatternCode("""value = pattern"""),
                        ]),
                        )
                    ),
                PatternBind("fail", PatternMaybe(PatternRule("failure"))),
                PatternCode("""
value = peg.Rule(name, [pattern1] + patterns, inline = (inline != None), rules = rule_parameters, parameters = parameters, fail = fail)"""),
                ]),
            ]),
        Rule("pattern_line",[
            PatternSequence([
                PatternBind("patterns", PatternRepeatMany(PatternRule("pattern"))),
                PatternCode("""
value = peg.PatternSequence(patterns)
#if code != None:
#    value = code(peg.PatternSequence(patterns))
#else:
#    value = peg.PatternAction(peg.PatternSequence(patterns), "value = values;")
"""),
                ]),
            ]),
        Rule("pattern", [
            PatternSequence([
                PatternBind("bind", PatternMaybe(PatternRule("bind"))),
                PatternBind("item", PatternRule("item")),
                PatternRule("spaces"),
                PatternCode("""
# value = peg.PatternRule(values[0])
if bind != None:
    item = bind(item)
value = item
# print "Pattern is " + str(value)
"""),
                ]),
            ]),
        Rule('raw_code', [PatternSequence([
            PatternVerbatim("("),
            PatternRule("spaces"),
            PatternBind("code", PatternRule('code')),
            PatternRule("spaces"),
            PatternVerbatim(")"),
            PatternCode("""value = code.code"""),
            ]),
        ]),
        Rule("code", [
            PatternSequence([
                PatternVerbatim("{{"),
                PatternRepeatOnce(PatternSequence([
                    PatternNot(PatternVerbatim("}}")),
                    PatternAny(),
                    PatternCode("""value = values[1]""")
                    ])),
                PatternVerbatim("}}"),
                PatternCode("""value = peg.PatternCode(''.join(values[1]))"""),
                ]),
            ]),
        Rule("item", [
            PatternSequence([
                PatternBind("ensure", PatternMaybe(PatternVerbatim("&"))),
                PatternBind("pnot", PatternMaybe(PatternVerbatim("!"))),
                PatternBind("pattern",
                    PatternOr([
                        PatternRule("x_word"),
                        PatternRule("any"),
                        # Actions inside an Or don't work
                        # PatternAction(PatternVerbatim("."), """value = peg.PatternAny()"""),
                        PatternRule("eof"),
                        PatternRule("void"),
                        PatternRule("range"),
                        PatternRule("string"),
                        PatternRule("line"),
                        PatternRule("ascii"),
                        PatternRule("predicate"),
                        PatternRule("call_rule"),
                        PatternRule("sub_pattern"),
                        PatternRule("code")])),
                    PatternBind("modifier", PatternMaybe(PatternRule("modifier"))),
                    PatternCode("""
if modifier != None:
    pattern = modifier(pattern)
if pnot != None:
    pattern = peg.PatternNot(pattern)
if ensure != None:
    pattern = peg.PatternEnsure(pattern)
value = pattern
""")]),
            ]),
        Rule("failure", [
            PatternSequence([
                PatternRule("whitespace"),
                PatternVerbatim("<fail>"),
                PatternRule("spaces"),
                PatternBind('code', PatternRule('code')),
                PatternCode("""value = code.code"""),
                ]),
            ]),
        Rule("line", [
            PatternSequence([
                PatternVerbatim("<line>"),
                PatternCode("""value = peg.PatternLine()""")
                ]),
            ]),
        Rule("predicate", [
            PatternSequence([
                PatternVerbatim("<predicate"),
                PatternRule("whitespace"),
                PatternBind('variable', PatternRule('word')),
                PatternRule("whitespace"),
                PatternVerbatim(">"),
                PatternRule("whitespace"),
                PatternBind('code', PatternRule('code')),
                PatternCode("value = peg.PatternPredicate(variable, code.code)"),
            ]),
        ]),
        Rule("ascii", [
            PatternSequence([
                PatternVerbatim("<ascii"),
                PatternRule("spaces"),
                PatternBind('num', PatternRule("number")),
                PatternRule("spaces"),
                PatternVerbatim(">"),
                PatternCode("""value = peg.PatternVerbatim(int(num))"""),
                ]),
            ]),
        Rule("call_rule", [
            PatternSequence([
               PatternVerbatim("@"),
               PatternBind('name', PatternRule("word")),
               PatternBind('rule_parameters', PatternMaybe(PatternRule('parameters_rules'))),
               PatternBind('parameters', PatternMaybe(PatternRule('parameters_values'))),
               PatternCode("""value = peg.PatternCallRule(name, rule_parameters, parameters)"""),
           ]),
        ]),
        Rule("eof", [
            PatternSequence([
                PatternVerbatim("<eof>"),
                PatternCode("""value = peg.PatternEof()""")
                ]),
            ]),
        Rule("void", [PatternSequence([
            PatternVerbatim("<void>"),
            PatternCode("""value = peg.PatternVoid()""")])
            ]),
        Rule("range", [
            PatternSequence([
                PatternVerbatim("["),
                PatternRepeatMany(PatternSequence([
                    PatternNot(PatternVerbatim("]")),
                    PatternAny(),
                    PatternCode("value = values[1]")])),
                PatternVerbatim("]"),
                PatternCode("""
value = peg.PatternRange(''.join(values[1]))
""")]),
            ]),
        Rule("sub_pattern", [
            PatternSequence([
                PatternVerbatim("("),
                PatternRepeatOnce(PatternRule("pattern")),
                PatternVerbatim(")"),
                PatternCode("""
value = peg.PatternSequence(values[1])
""")]),
            ]),
        Rule("bind", [
            PatternSequence([
                PatternBind("name", PatternRule("word")),
                PatternVerbatim(":"),
                PatternCode("""
value = lambda p: peg.PatternBind(name, p)
""")]),
            ]),
        Rule("string", [
            PatternSequence([
                PatternVerbatim("\""),
                PatternRepeatMany(PatternSequence([
                    PatternNot(PatternVerbatim("\"")),
                    PatternAny(),
                    PatternCode("value = values[1]")])),
                PatternVerbatim("\""),
                PatternBind("options", PatternMaybe(PatternVerbatim("{case}"))),
                PatternCode("""
value = peg.PatternVerbatim(''.join(values[1]), options)
""")]),
            PatternSequence([
                PatternVerbatim("<quote>"),
                PatternCode("""value = peg.PatternVerbatim('"')"""),
                ]),
            ]),
        Rule("modifier", [
            PatternSequence([PatternVerbatim("*"),
            PatternCode("""
value = lambda p: peg.PatternRepeatMany(p)
""")]),
            PatternSequence([PatternVerbatim("?"),
            PatternCode("""
value = lambda p: peg.PatternMaybe(p)
""")]),
            PatternSequence([PatternVerbatim("+"),
            PatternCode("""
value = lambda p: peg.PatternRepeatOnce(p)
""")]),
            ]),
        Rule("x_word", [
            PatternSequence([
                PatternBind('name', PatternRule("word")),
                PatternBind('rule_parameters', PatternMaybe(PatternRule('parameters_rules'))),
                PatternBind('parameters', PatternMaybe(PatternRule('parameters_values'))),
                PatternCode("""
value = peg.PatternRule(name, rule_parameters, parameters)
""")]),
            ]),
        Rule('rule_parameters', [
            PatternSequence([
                PatternVerbatim("["),
                PatternRule("spaces"),
                PatternBind('param1', PatternRule('word')),
                PatternBind('params', PatternRepeatMany(PatternSequence([
                    PatternRule('spaces'),
                    PatternVerbatim(','),
                    PatternRule('spaces'),
                    PatternBind('exp', PatternRule('word')),
                    PatternCode("""value = exp""")]))),
                PatternRule("spaces"),
                PatternVerbatim("]"),
                PatternCode("""value = [param1] + params""")]),
            ]),
        Rule('value_parameters', [
            PatternSequence([
                PatternVerbatim("("),
                PatternRule("spaces"),
                PatternBind('param1', PatternRule('word')),
                PatternBind('params', PatternRepeatMany(PatternSequence([
                    PatternRule('spaces'),
                    PatternVerbatim(','),
                    PatternRule('spaces'),
                    PatternBind('exp', PatternRule('word')),
                    PatternCode("""value = exp""")]))),
                PatternRule("spaces"),
                PatternVerbatim(")"),
                PatternCode("""value = [param1] + params""")]),
            ]),
        Rule('parameters_rules', [
            PatternSequence([
                PatternVerbatim("["),
                PatternRule("spaces"),
                PatternBind('param1', PatternRule('word_or_at')),
                PatternBind('params', PatternRepeatMany(PatternSequence([
                    PatternRule('spaces'),
                    PatternVerbatim(','),
                    PatternRule('spaces'),
                    PatternBind('exp', PatternRule('word_or_at')),
                    PatternCode("""value = exp""")]))),
                PatternRule("spaces"),
                PatternVerbatim("]"),
                PatternCode("""value = [param1] + params""")]),
            ]),

        Rule('parameters_values', [
            PatternSequence([
                PatternVerbatim("("),
                PatternRule("spaces"),
                PatternBind('param1', PatternRule('word_or_dollar')),
                PatternBind('params', PatternRepeatMany(PatternSequence([
                    PatternRule('spaces'),
                    PatternVerbatim(','),
                    PatternRule('spaces'),
                    PatternBind('exp', PatternRule('word_or_dollar')),
                    PatternCode("""value = exp""")]))),
                PatternRule("spaces"),
                PatternVerbatim(")"),
                PatternCode("""value = [param1] + params""")]),
            ]),
        Rule('word_or_dollar', [
            PatternRule('word'),
            PatternRule('dollar'),
            ]),
        Rule('word_or_at', [
            PatternRule('word'),
            PatternRule('word_at'),
            ]),
        Rule('word_at', [
            PatternSequence([
                PatternVerbatim("@"),
                PatternBind('word', PatternRule("word")),
                PatternCode("""value = '@%s' % word""")]),
            ]),
        Rule('dollar', [
            PatternSequence([
                PatternVerbatim("$"),
                PatternBind('number', PatternRule("number")),
                PatternCode("""value = "$%s" % number""")]),
            ]),
        Rule('number', [
            PatternSequence([
                PatternRepeatOnce(PatternRule('digit')),
                PatternCode("""value = ''.join(values[0])""")]),
            ]),
        Rule('digit', [PatternRange('0123456789')]),
        Rule("start_symbol", [
            PatternSequence([
                PatternVerbatim("start-symbol:"),
                PatternRepeatMany(PatternRule("space")),
                PatternRule("word"),
                PatternCode("""value = values[2]""")]),
            ]),
        Rule("spaces", [PatternSequence([PatternRepeatMany(PatternRule("space"))])]),
        # Rule("space", [PatternRange(' \t')]),
        Rule("space", [PatternSequence([PatternVerbatim(" ")]), PatternSequence([PatternVerbatim("\\t")])]),
        Rule("any_char", [PatternRange('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_-')]),
        Rule("any", [PatternSequence([
            PatternVerbatim("."),
            PatternCode("""value = peg.PatternAny()""")]),
            ]),
        Rule("whitespace", [
            PatternSequence([
                PatternRepeatMany(PatternOr([
                    PatternRange(" \\t\\n"),
                    PatternRule("comment"),
                    ])),
                ]),
            ]),
        Rule("comment", [
            PatternSequence([
                PatternVerbatim("#"),
                PatternUntil(PatternVerbatim("\\n")),
                ]),
            ]),
        Rule("newlines_one", [PatternRepeatOnce(PatternVerbatim("\\n"))]),
        Rule("newlines", [PatternRepeatMany(PatternVerbatim("\\n"))]),
    ]

    peg = Peg("start", None, None, [peg_name], rules, [])
    # print peg.generate_python()
    return peg

def make_peg_parser(name = 'peg'):
    return create_peg(peg_bnf(name))
    # answer = parser('peg.in')
    # print "Got " + str(answer)
    # print answer.generate()
    # module = compile(peg.generate_python(), peg.namespace, 'exec')
    # print module

# test()
# test2()

def help_syntax():
    print "start-symbol: <name>"
    print "rules:"
    print "  <name> = <pattern> | <pattern> ... "
    print
    print "A Pattern can be:"
    print "  \"<literal>\""
    print "  <name of rule>"
    print "  pattern*"
    print "  pattern?"
    print "  pattern+"
    print "  [<characters>]"
    print "  <eof>"
    print
    print "BNF grammar for a peg grammar"
    print peg_bnf('peg').generate_bnf()

def help():
    print "Options:"
    print "-h,--help,help : Print this help"
    print "--help-syntax : Explain syntax of BNF (Backus-Naur form) for grammar files"
    print "--bnf : Generate BNF description (grammar language)"
    print "--ruby : Generate Ruby parser"
    print "--python : Generate Python parser"
    print "--cpp,--c++ : Generate C++ parser"
    # print "--c++-interpreter : Generate a C++ parser that uses an interpreter"
    print "--save=filename : Save all generated parser output to a file, 'filename'"
    print "--peg-name=name : Name the peg module 'name'. The intermediate peg module will be written as peg_<name>.py. Defaults to 'peg'."

# make_peg_parser()
if __name__ == '__main__':
    import sys
    import re
    doit = []
    file = None
    helped = 0
    def default_peg():
        return make_peg_parser()
    peg_maker = default_peg
    save_re = re.compile('--save=(.*)')
    separate_rules_re = re.compile('--separate-rules=(.*)')
    list_separate_rules_re = re.compile('--list-separate-rules=(.*)')
    peg_name_re = re.compile('--peg-name=(.*)')
    def print_it(p):
        print p
    do_output = print_it
    do_close = lambda : 0
    return_code = 0
    parallel = [False]
    separate = [None]
    for arg in sys.argv[1:]:
        if arg == '--bnf':
            doit.append(lambda p: p.generate_bnf())
        elif arg == '--cpp' or arg == '--c++':
            doit.append(lambda p: p.generate_cpp(parallel[0], separate[0]))
        #elif arg == '--c++-interpreter':
        #    doit.append(lambda p: p.generate_cpp_interpreter())
        elif arg == '--ruby':
            doit.append(lambda p: p.generate_ruby())
        elif arg == '--python':
            doit.append(lambda p: p.generate_python())
        elif arg == '--test':
            doit.append(lambda p: p.generate_test())
        elif arg == '--parallel':
            parallel[0] = True
        elif arg == "--help-syntax":
            help_syntax()
        elif peg_name_re.match(arg):
            all = peg_name_re.match(arg)
            name = all.group(1)
            def make_peg():
                return make_peg_parser(name)
            peg_maker = make_peg
        elif separate_rules_re.match(arg):
            all = separate_rules_re.match(arg)
            separate[0] = all.group(1)
        elif list_separate_rules_re.match(arg):
            all = list_separate_rules_re.match(arg)
            doit.append(lambda p: p.print_list_files(all.group(1)))
        elif save_re.match(arg):
            all = save_re.match(arg)
            fout = open(all.group(1), 'w')
            do_close = lambda : fout.close()
            def save(p):
                fout.write(p)
            do_output = save
        elif arg == '-h' or arg == '--help' or arg == 'help':
            help()
            helped = 1
        else:
            file = arg
    
    if file != None:
        parser = peg_maker()
        out = parser(file)
        # print out
        if out != None:
            if len(doit) == 0:
                print "Grammar file '%s' looks good!. Use some options to generate a peg parser. -h will list all available options." % file
            else:
                for generate in doit:
                    do_output(generate(out))
        else:
            print "Uh oh, couldn't parse " + file + ". Are you sure its using BNF format?"
            return_code = 1
    else:
        if helped == 0:
            help()
            print "Give a BNF grammar file as an argument"

    do_close()
    exit(return_code)


# Done
# memoize in python parsers
# include arbitrary code at the top of the file (python, c++, bnf)
# fix binding variables in c++ (move declaration to the top of the function)
# make intra-pattern actions work
# add helper function section
# error reporting for c++
# generator for python, ruby, c++
# getter for the current line and column
# custom error reporting length, options: error-length 40
# If a rule has a <fail> then catch a parsing exception and call the fail function
# Predicates: sequences of host code that evaluate to true/false where the current
#   rule stops on false and continues on true.
