import core
from core import newResult, indent, gensym, special_char, newOut, Accessor

class LuaGenerator(core.CodeGenerator):
    def generate_verbatim(self, pattern, rule, peg):
        return 'verbatim: ' + pattern.letters

    def generate_ensure(self, pattern, rule, peg):
        return 'ensure: ' + pattern.next.generate_v3(self, rule, peg)

    def generate_repeat_many(self, pattern, rule, peg):
        return 'repeat*: ' + pattern.next.generate_v3(self, rule, peg)
    
    def generate_repeat_once(self, pattern, rule, peg):
        return 'repeat+: ' + pattern.next.generate_v3(self, rule, peg)

    def generate_eof(self, pattern, rule, peg):
        return 'eof'

    def generate_sequence(self, pattern, rule, peg):
        def newPattern(what):
            return what.generate_v3(self, rule, peg)
        return ' '.join([newPattern(what) for what in pattern.patterns])

main = """
%(rules)s

function parse(file)
  print("Parse " .. file)
end
"""

def generate(peg):
    rules = '\n'.join([rule.generate_lua(peg) for rule in peg.rules])
    return main % {'rules' : rules}
