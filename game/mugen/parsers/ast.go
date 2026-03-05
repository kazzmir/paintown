package parsers

// AST represents the Abstract Syntax Tree for a parsed M.U.G.E.N .def file.
// A .def file is primarily a list of sections, each containing attributes.

// File represents the entire parsed .def file.
type File struct {
	Sections []*Section
}

// Section represents a section like [Info] or [Files].
type Section struct {
	Name       string
	Line       int
	Column     int
	Attributes []*Attribute
}

// Attribute represents a key-value assignment, e.g., name = "Player 1"
type Attribute struct {
	ID     *Identifier
	Value  Value
	Line   int
	Column int
}

// Identifier represents the left-hand side of an attribute assignment.
// While usually simple (like 'name'), M.U.G.E.N identifiers can have dots, like 'anim.offset'.
type Identifier struct {
	Parts []string
}

// String returns the string representation of the Identifier.
func (i *Identifier) String() string {
	if len(i.Parts) == 0 {
		return ""
	}
	s := i.Parts[0]
	for _, p := range i.Parts[1:] {
		s += "." + p
	}
	return s
}

// Value represents the right-hand side of an attribute assignment.
// It is an interface that can be implemented by different value types.
type Value interface {
	isValue()
}

// NumberValue represents an integer or floating-point number.
type NumberValue struct {
	Val float64
}

func (NumberValue) isValue() {}

// StringValue represents a quoted string value.
type StringValue struct {
	Val string
}

func (StringValue) isValue() {}

// KeywordValue represents a raw unquoted string, often used for filenames or specific constants like "addalpha".
type KeywordValue struct {
	Val string
}

func (KeywordValue) isValue() {}

// ValueList represents a comma-separated list of values.
type ValueList struct {
	Values []Value
	Line   int
	Column int
}

func (ValueList) isValue() {}

// DateValue represents a parsed date value like 01.01.2000
type DateValue struct {
	Month int
	Day   int
	Year  int
}

func (DateValue) isValue() {}
