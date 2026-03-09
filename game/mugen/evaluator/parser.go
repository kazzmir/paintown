package evaluator

import (
	"fmt"
	"strconv"
	"strings"
	"unicode"
)

// Parser parses MUGEN CNS expression strings into an AST.
type Parser struct {
	tokens []token
	pos    int
}

type tokenType int

const (
	tokError tokenType = iota
	tokEOF
	tokNumber
	tokIdentifier
	tokOperator
	tokLParen
	tokRParen
	tokLBracket
	tokRBracket
	tokComma
	tokString
)

type token struct {
	typ tokenType
	lit string
}

func Parse(input string) (Expression, error) {
	tokens := tokenize(input)
	p := &Parser{tokens: tokens}
	expr, err := p.parseExpression(0)
	if err != nil {
		return nil, err
	}
	if p.peek().typ != tokEOF {
		return nil, fmt.Errorf("unexpected token at end of expression: %s", p.peek().lit)
	}
	return expr, nil
}

func tokenize(input string) []token {
	var tokens []token
	chars := []rune(input)
	i := 0
	for i < len(chars) {
		ch := chars[i]
		if unicode.IsSpace(ch) {
			i++
			continue
		}

		if unicode.IsDigit(ch) || (ch == '.' && i+1 < len(chars) && unicode.IsDigit(chars[i+1])) {
			start := i
			for i < len(chars) && (unicode.IsDigit(chars[i]) || chars[i] == '.') {
				i++
			}
			tokens = append(tokens, token{tokNumber, string(chars[start:i])})
			continue
		}

		if unicode.IsLetter(ch) || ch == '_' {
			start := i
			for i < len(chars) && (unicode.IsLetter(chars[i]) || unicode.IsDigit(chars[i]) || chars[i] == '_' || chars[i] == ' ' || chars[i] == '.') {
				// Special check for spaces in identifiers like "pos x"
				// But we should be careful not to consume symbols.
				// Actually, "pos x" should probably be treated as two tokens or handled specifically.
				// In MUGEN, identifiers can have spaces between keywords sometimes, but usually lexers are stricter.
				// Let's allow one space if followed by internal chars.
				if chars[i] == ' ' && (i+1 >= len(chars) || !unicode.IsLetter(chars[i+1])) {
					break
				}
				i++
			}
			lit := strings.TrimSpace(string(chars[start:i]))
			tokens = append(tokens, token{tokIdentifier, lit})
			continue
		}

		if ch == '"' {
			start := i
			i++ // Skip open quote
			for i < len(chars) && chars[i] != '"' {
				i++
			}
			if i < len(chars) {
				i++ // Skip close quote
			}
			lit := string(chars[start+1 : i-1])
			tokens = append(tokens, token{tokString, lit})
			continue
		}

		switch ch {
		case '[':
			tokens = append(tokens, token{tokLBracket, "["})
			i++
		case ']':
			tokens = append(tokens, token{tokRBracket, "]"})
			i++
		case '(':
			tokens = append(tokens, token{tokLParen, "("})
			i++
		case ')':
			tokens = append(tokens, token{tokRParen, ")"})
			i++
		case ',':
			tokens = append(tokens, token{tokComma, ","})
			i++
		case '+', '-', '*', '/', '%', '=', '!', '>', '<', '&', '|', '^', '~', ':':
			i++
			// Handle multi-char ops like ==, !=, >=, <=, &&, ||, **, :=
			if i < len(chars) {
				next := chars[i]
				combined := string(ch) + string(next)
				switch combined {
				case "==", "!=", ">=", "<=", "&&", "||", "**", ":=":
					tokens = append(tokens, token{tokOperator, combined})
					i++
					continue
				}
			}
			// Convert single '=' to "==" behavior if it's a comparison?
			// MUGEN uses '=' for both assignment and comparison in different contexts.
			tokens = append(tokens, token{tokOperator, string(ch)})
		default:
			i++
		}
	}
	tokens = append(tokens, token{tokEOF, ""})
	return tokens
}

var precedence = map[string]int{
	"||": 1,
	"&&": 2,
	"|":  3,
	"^":  4,
	"&":  5,
	"=":  6, "==": 6, "!=": 6,
	">": 7, ">=": 7, "<": 7, "<=": 7,
	"+": 8, "-": 8,
	"*": 9, "/": 9, "%": 9,
	"**": 10,
}

func (p *Parser) parseExpression(minPrecedence int) (Expression, error) {
	left, err := p.parsePrimary()
	if err != nil {
		return nil, err
	}

	for {
		tok := p.peek()
		if tok.typ != tokOperator {
			break
		}
		prec, ok := precedence[tok.lit]
		if !ok || prec < minPrecedence {
			break
		}

		p.advance()
		right, err := p.parseExpression(prec + 1)
		if err != nil {
			return nil, err
		}
		left = &Binary{Left: left, Operator: tok.lit, Right: right}
	}

	return left, nil
}

func (p *Parser) parsePrimary() (Expression, error) {
	tok := p.advance()
	switch tok.typ {
	case tokNumber:
		val, _ := strconv.ParseFloat(tok.lit, 64)
		return &Number{Value: val}, nil
	case tokIdentifier:
		if p.peek().typ == tokLParen {
			p.advance() // Consume '('
			var args []Expression
			if p.peek().typ != tokRParen {
				for {
					arg, err := p.parseExpression(0)
					if err != nil {
						return nil, err
					}
					args = append(args, arg)
					if p.peek().typ != tokComma {
						break
					}
					p.advance() // Consume ','
				}
			}
			if p.peek().typ != tokRParen {
				return nil, fmt.Errorf("expected ')' after arguments, got %s", p.peek().lit)
			}
			p.advance() // Consume ')'
			return &Call{Name: tok.lit, Arguments: args}, nil
		}
		return &Identifier{Name: tok.lit}, nil
	case tokString:
		return &StringExpr{Value: tok.lit}, nil
	case tokLBracket:
		expr, err := p.parseExpression(0)
		if err != nil {
			return nil, err
		}
		if p.advance().typ != tokComma {
			return nil, fmt.Errorf("expected ',' in interval")
		}
		right, err := p.parseExpression(0)
		if err != nil {
			return nil, err
		}
		closeTok := p.advance()
		if closeTok.typ != tokRBracket && closeTok.typ != tokRParen {
			return nil, fmt.Errorf("expected ']' or ')' to close interval")
		}
		return &Interval{Left: expr, Right: right, LeftInc: true, RightInc: closeTok.typ == tokRBracket}, nil
	case tokLParen:
		expr, err := p.parseExpression(0)
		if err != nil {
			return nil, err
		}
		if p.peek().typ == tokComma {
			p.advance() // Consume ','
			right, err := p.parseExpression(0)
			if err != nil {
				return nil, err
			}
			closeTok := p.advance()
			if closeTok.typ != tokRBracket && closeTok.typ != tokRParen {
				return nil, fmt.Errorf("expected ']' or ')' to close interval")
			}
			return &Interval{Left: expr, Right: right, LeftInc: false, RightInc: closeTok.typ == tokRBracket}, nil
		}
		if p.advance().typ != tokRParen {
			return nil, fmt.Errorf("expected ')'")
		}
		return expr, nil
	case tokOperator:
		// Handle unary ops
		if tok.lit == "-" || tok.lit == "!" || tok.lit == "~" {
			expr, err := p.parsePrimary()
			if err != nil {
				return nil, err
			}
			return &Unary{Operator: tok.lit, Expression: expr}, nil
		}
	}
	return nil, fmt.Errorf("unexpected token: %s (%v)", tok.lit, tok.typ)
}

func (p *Parser) peek() token {
	return p.tokens[p.pos]
}

func (p *Parser) advance() token {
	tok := p.tokens[p.pos]
	if p.pos < len(p.tokens)-1 {
		p.pos++
	}
	return tok
}
