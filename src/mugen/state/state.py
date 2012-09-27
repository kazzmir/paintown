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
