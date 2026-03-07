package parsers

import (
	"bytes"
	"fmt"
	"io"
	"strconv"
	"strings"
)

// Parser parses a stream of tokens into an AST File.
type Parser struct {
	l    *Lexer
	tok  Token
	next Token
}

// Parse parses a M.U.G.E.N .def file from the given reader.
func Parse(r io.Reader) (*File, error) {
	p := &Parser{
		l: NewLexer(r),
	}
	p.advance()
	p.advance()
	return p.parseFile()
}

func (p *Parser) advance() {
	p.tok = p.next
	p.next = p.l.NextToken()
	// Ignore extra newlines for simplicity in most contexts, though we handle them carefully in attributes.
	// Actually, returning Newlines is useful for end of attribute. Let's filter redundant ones.
	for p.next.Type == TokenNewline && p.tok.Type == TokenNewline {
		p.next = p.l.NextToken()
	}
}

func (p *Parser) parseFile() (*File, error) {
	file := &File{}

	for p.tok.Type != TokenEOF {
		if p.tok.Type == TokenNewline {
			p.advance()
			continue
		}

		if p.tok.Type == TokenSectionStart {
			sec, err := p.parseSection()
			if err != nil {
				return nil, err
			}
			file.Sections = append(file.Sections, sec)
			continue
		}

		// Unexpected token
		return nil, fmt.Errorf("unexpected token at line %d:%d - %v (expected section start '[' or newline)", p.tok.Line, p.tok.Column, p.tok.Type)
	}

	return file, nil
}

func (p *Parser) parseSection() (*Section, error) {
	sec := &Section{
		Line:   p.tok.Line,
		Column: p.tok.Column,
	}
	p.advance() // Consume '['

	var sectionName bytes.Buffer
	for p.tok.Type != TokenSectionEnd && p.tok.Type != TokenNewline && p.tok.Type != TokenEOF {
		if sectionName.Len() > 0 {
			// Maintain spacing if they were separate tokens
			sectionName.WriteString(" ")
		}
		sectionName.WriteString(p.tok.Lit)
		p.advance()
	}

	if p.tok.Type != TokenSectionEnd {
		return nil, fmt.Errorf("expected ']' at line %d:%d, got %v", p.tok.Line, p.tok.Column, p.tok.Type)
	}
	p.advance() // Consume ']'

	sec.Name = sectionName.String()

	// Parse attributes until next section or EOF
	for p.tok.Type != TokenEOF && p.tok.Type != TokenSectionStart {
		if p.tok.Type == TokenNewline {
			p.advance()
			continue
		}

		attr, err := p.parseAttribute()
		if err != nil {
			return nil, err
		}
		if attr != nil {
			sec.Attributes = append(sec.Attributes, attr)
		}
	}

	return sec, nil
}

func (p *Parser) parseAttribute() (*Attribute, error) {
	attr := &Attribute{
		Line:   p.tok.Line,
		Column: p.tok.Column,
	}

	var idParts []string
	for p.tok.Type != TokenEOF && p.tok.Type != TokenAssign && p.tok.Type != TokenComma && p.tok.Type != TokenNewline {
		idParts = append(idParts, p.tok.Lit)
		p.advance()
	}

	if len(idParts) == 0 {
		return nil, fmt.Errorf("expected identifier at line %d:%d, got %v", p.tok.Line, p.tok.Column, p.tok.Lit)
	}

	attrID := strings.Join(idParts, "")
	attr.ID = &Identifier{Parts: []string{attrID}}

	// MUGEN attributes typically have an '=', but animations like "200,4, 0,0, 1" do not.
	// If the next token is '=', consume it and parse as normal assignment.
	// Otherwise, the entire line from the identifier onwards (including the identifier) might be considered the "value"
	// or the identifier is the first part, and the rest is the value.
	// Let's treat the first token as the ID, and everything after (if no '=') as the value.
	if p.tok.Type == TokenAssign {
		p.advance() // Consume '='
	} else if p.tok.Type == TokenComma {
		// It's a comma-separated list without an equals sign (e.g., animations).
		// We'll leave the comma there so parseValueList can pick it up if needed.
		// Actually, `parseValueList` expects values separated by commas.
		// So if we just continue, it will parse the first comma.
		// Wait, `parseValueList` expects a value *first*. If p.tok is Comma, it will fail.
		// We should unread the identifier, or parse the whole line as a ValueList.
		// Let's re-think: For animations, maybe the "ID" is empty, or the ID is the first number?
		// Let's stick to ID is the first number. The next token is a comma.
		// We can inject a dummy value or just parse the rest of the list.
		// `parseValueList` logic:
		// val := p.parseSingleValue()
		// if p.tok.Type == TokenComma ...
		// If we are at a comma, it means the list is missing its first item? No, the ID *was* the first item in a way,
		// but `parseAttribute` separates ID from Value.
		// Let's unread the ID, make ID string empty, and let `parseValueList` take the whole thing.

		// Unread `idTok` is hard with the current parser structure.
		// Instead, we can just say ID is empty, and construct a ValueList starting with `idTok` as a NumberValue.
		// But let's keep it simple: ID is the first number. The value is everything after the first comma.
		// If it's a comma, we advance past it.
		p.advance()
	}

	// It's possible for an attribute to have no value, e.g. "param = \n"
	if p.tok.Type == TokenNewline || p.tok.Type == TokenEOF {
		attr.Value = nil
	} else {
		// Parse the value(s)
		val, err := p.parseValueList()
		if err != nil {
			return nil, err
		}
		attr.Value = val
	}

	// Ensure newline or EOF ends the attribute
	if p.tok.Type != TokenNewline && p.tok.Type != TokenEOF {
		fmt.Printf("Warning: trailing garbage in attribute %s: %s (%v)\n", attrID, p.tok.Lit, p.tok.Type)
		return nil, fmt.Errorf("expected newline after attribute at line %d:%d, got %v (%s)", p.tok.Line, p.tok.Column, p.tok.Type, p.tok.Lit)
	}

	return attr, nil
}

func (p *Parser) parseIdentifier() (*Identifier, error) {
	id := &Identifier{}

	if p.tok.Type != TokenIdentifier && p.tok.Type != TokenKeyword {
		return nil, fmt.Errorf("expected identifier at line %d:%d, got %v", p.tok.Line, p.tok.Column, p.tok.Lit)
	}

	id.Parts = strings.Split(p.tok.Lit, ".")
	p.advance()
	return id, nil
}

func (p *Parser) parseValueList() (Value, error) {
	startLine := p.tok.Line
	startCol := p.tok.Column

	var values []Value

	for {
		// Stop condition
		if p.tok.Type == TokenNewline || p.tok.Type == TokenEOF {
			break
		}

		if p.tok.Type == TokenComma {
			// Empty value if we encounter a comma immediately (e.g. initial comma or consecutive commas: x = , , 2)
			values = append(values, StringValue{Val: ""})
			p.advance() // consume comma
			continue
		}

		val, err := p.parseValue()
		if err != nil {
			return nil, err
		}
		if val == nil {
			break
		}
		values = append(values, val)

		// After a value, if we have a comma, consume it and continue
		if p.tok.Type == TokenComma {
			p.advance()
			// If a comma is the last thing before a newline/EOF, we had a trailing empty value: `x = 1, `
			if p.tok.Type == TokenNewline || p.tok.Type == TokenEOF {
				values = append(values, StringValue{Val: ""})
				break
			}
		} else {
			// No comma means the list is done
			break
		}
	}

	if len(values) == 1 {
		return values[0], nil
	}
	if len(values) == 0 {
		return StringValue{Val: ""}, nil
	}

	return &ValueList{
		Line:   startLine,
		Column: startCol,
		Values: values,
	}, nil
}

func (p *Parser) parseValue() (Value, error) {
	// Values in mugen can be space separated keywords, or have multiple dots
	var parts []string

	for {
		switch p.tok.Type {
		case TokenNumber, TokenIdentifier, TokenKeyword, TokenAssign, TokenSectionStart, TokenSectionEnd, TokenLBrace, TokenRBrace:
			parts = append(parts, p.tok.Lit)
			p.advance()
		case TokenString:
			// If it's a quoted string, it must be the only thing or the first thing.
			if len(parts) == 0 {
				v := StringValue{Val: p.tok.Lit}
				p.advance()
				return v, nil
			}
			parts = append(parts, fmt.Sprintf("\"%s\"", p.tok.Lit))
			p.advance()
		default:
			if len(parts) == 0 {
				return nil, nil
			}
			// We've gathered some parts, and hit a boundary (like a comma, newline, etc)
			combined := strings.Join(parts, "")

			// Try parsing as simple number
			if val, err := strconv.ParseFloat(combined, 64); err == nil && len(parts) == 1 {
				return NumberValue{Val: val}, nil
			}

			return KeywordValue{Val: combined}, nil
		}
	}
}
