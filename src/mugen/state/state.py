class State:
    def __init__(self, name):
        self.name = name
        self.fields = []

    def addField(self, field):
        self.fields.append(field)

class Field:
    def __init__(self, type_, name):
        self.type_ = type_
        self.name = name

    def zero(self):
        return '0'

def combineTemplate(name, templates):
    if templates != None:
        return "%s<%s>" % (name, ', '.join(templates))
    return name

def combineModifier(modifier, name):
    if modifier != None:
        return '%s %s' % (modifier, name)
    return name

def makeType(modifier, name, template, namespace):
    all = combineModifier(modifier, combineTemplate(name, template))
    if namespace != None:
        return all + "::" + namespace
    return all
