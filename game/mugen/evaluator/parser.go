package evaluator

import (
	"fmt"
	"strconv"
	"strings"
	"unicode"
)

type Parser struct {
	tokens      []token
	pos, depth  int
	redirecting bool
}
type tokenType uint8

const (
	tokEOF tokenType = iota
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
	typ    tokenType
	lit    string
	column int
}

func Parse(input string) (Expression, error) {
	tokens, err := tokenize(input)
	if err != nil {
		return nil, err
	}
	p := &Parser{tokens: tokens}
	expr, err := p.parseExpression(0)
	if err != nil {
		return nil, err
	}
	if p.peek().typ != tokEOF {
		return nil, p.errorf("unexpected token %q", p.peek().lit)
	}
	if err := validateIntervals(expr, false); err != nil {
		return nil, p.errorf("%s", err)
	}
	return expr, nil
}

func validateIntervals(expr Expression, allowed bool) error {
	switch e := expr.(type) {
	case *Interval:
		if !allowed {
			return fmt.Errorf("an interval requires = or !=")
		}
		if err := validateIntervals(e.Left, false); err != nil {
			return err
		}
		return validateIntervals(e.Right, false)
	case *Group:
		return validateIntervals(e.Expression, false)
	case *Unary:
		return validateIntervals(e.Expression, false)
	case *Binary:
		if err := validateIntervals(e.Left, false); err != nil {
			return err
		}
		return validateIntervals(e.Right, e.Operator == "=" || e.Operator == "==" || e.Operator == "!=")
	case *Call:
		for _, arg := range e.Arguments {
			if err := validateIntervals(arg, false); err != nil {
				return err
			}
		}
	case *Redirect:
		return validateIntervals(e.Trigger, false)
	}
	return nil
}

func tokenize(input string) ([]token, error) {
	var tokens []token
	chars := []rune(input)
	for i := 0; i < len(chars); {
		if len(tokens) >= 16384 {
			return nil, fmt.Errorf("expression exceeds 16384 tokens")
		}
		ch := chars[i]
		if unicode.IsSpace(ch) {
			i++
			continue
		}
		start := i
		if unicode.IsDigit(ch) || (ch == '.' && i+1 < len(chars) && unicode.IsDigit(chars[i+1])) {
			i++
			for i < len(chars) && (unicode.IsDigit(chars[i]) || chars[i] == '.') {
				i++
			}
			tokens = append(tokens, token{tokNumber, string(chars[start:i]), start + 1})
			continue
		}
		if unicode.IsLetter(ch) || ch == '_' {
			i++
			for i < len(chars) && (unicode.IsLetter(chars[i]) || unicode.IsDigit(chars[i]) || chars[i] == '_' || chars[i] == '.') {
				i++
			}
			tokens = append(tokens, token{tokIdentifier, string(chars[start:i]), start + 1})
			continue
		}
		if ch == '"' {
			i++
			for i < len(chars) && chars[i] != '"' {
				i++
			}
			if i == len(chars) {
				return nil, fmt.Errorf("column %d: unterminated string", start+1)
			}
			tokens = append(tokens, token{tokString, string(chars[start+1 : i]), start + 1})
			i++
			continue
		}
		typ := tokOperator
		switch ch {
		case '(':
			typ = tokLParen
		case ')':
			typ = tokRParen
		case '[':
			typ = tokLBracket
		case ']':
			typ = tokRBracket
		case ',':
			typ = tokComma
		case '+', '-', '*', '/', '%', '=', '!', '>', '<', '&', '|', '^', '~', ':':
		default:
			return nil, fmt.Errorf("column %d: invalid character %q", i+1, ch)
		}
		i++
		if typ == tokOperator && i < len(chars) {
			pair := string([]rune{ch, chars[i]})
			switch pair {
			case "==", "!=", ">=", "<=", "&&", "||", "^^", "**", ":=":
				i++
			}
		}
		lit := string(chars[start:i])
		if lit == ":" {
			return nil, fmt.Errorf("column %d: expected :=", start+1)
		}
		tokens = append(tokens, token{typ, lit, start + 1})
	}
	return append(tokens, token{typ: tokEOF, column: len(chars) + 1}), nil
}

var precedence = map[string]int{
	"||": 1, "^^": 2, "&&": 3, "|": 4, "^": 5, "&": 6, ":=": 7,
	"=": 8, "==": 8, "!=": 8, ">": 9, ">=": 9, "<": 9, "<=": 9,
	"+": 10, "-": 10, "*": 11, "/": 11, "%": 11, "**": 12,
}

func (p *Parser) parseExpression(min int) (Expression, error) {
	p.depth++
	defer func() { p.depth-- }()
	if p.depth > 256 {
		return nil, p.errorf("expression nesting exceeds 256")
	}
	left, err := p.parsePrimary()
	if err != nil {
		return nil, err
	}
	for p.peek().typ == tokOperator {
		op := p.peek().lit
		prec, ok := precedence[op]
		if !ok || prec < min {
			break
		}
		if ungroupedInterval(left) {
			return nil, p.errorf("an interval must end its subexpression")
		}
		p.advance()
		next := prec + 1
		if op == ":=" {
			next = prec
		}
		right, err := p.parseExpression(next)
		if err != nil {
			return nil, err
		}
		if _, ok := right.(*Interval); ok && op != "=" && op != "==" && op != "!=" {
			return nil, p.errorf("an interval requires = or !=")
		}
		if op == ":=" {
			call, ok := left.(*Call)
			if !ok || (strings.ToLower(call.Name) != "var" && strings.ToLower(call.Name) != "fvar") || len(call.Arguments) != 1 {
				return nil, p.errorf("assignment requires an unredirected var(n) or fvar(n)")
			}
		}
		left = &Binary{Left: left, Operator: op, Right: right}
	}
	return left, nil
}

func ungroupedInterval(expr Expression) bool {
	switch e := expr.(type) {
	case *Interval:
		return true
	case *Binary:
		// Earlier operators already rejected intervals on their left. Following
		// only the right branch avoids quadratic scans of long arithmetic chains.
		return ungroupedInterval(e.Right)
	default:
		return false
	}
}

func (p *Parser) parsePrimary() (Expression, error) {
	tok := p.advance()
	switch tok.typ {
	case tokNumber:
		return p.number(tok, "")
	case tokString:
		return &StringExpr{Value: tok.lit}, nil
	case tokIdentifier:
		name := strings.ToLower(tok.lit)
		if isLegacy(name) && p.peek().typ == tokOperator {
			return p.legacy(tok.lit)
		}
		// Only component triggers permit a separate axis word, including tabs.
		if componentTrigger(name) && p.peek().typ == tokIdentifier {
			axis := strings.ToLower(p.peek().lit)
			if axis == "x" || axis == "y" || axis == "z" {
				tok.lit += " " + axis
				p.advance()
			}
		}
		var expr Expression = &Identifier{Name: tok.lit}
		if p.peek().typ == tokLParen {
			p.advance()
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
					p.advance()
				}
			}
			if p.peek().typ != tokRParen {
				return nil, p.errorf("expected ')' after %s arguments", tok.lit)
			}
			p.advance()
			expr = &Call{Name: tok.lit, Arguments: args}
		}
		if redirectName(name) && p.peek().typ == tokComma {
			if p.redirecting {
				return nil, p.errorf("recursive redirection is not supported")
			}
			p.advance()
			p.redirecting = true
			trigger, err := p.parsePrimary()
			p.redirecting = false
			if err != nil {
				return nil, err
			}
			switch trigger.(type) {
			case *Identifier, *Call, *Legacy, *Binary:
			default:
				return nil, p.errorf("redirection requires one trigger; recursive redirection is not supported")
			}
			return &Redirect{Selector: expr, Trigger: trigger}, nil
		}
		return expr, nil
	case tokLBracket, tokLParen:
		first, err := p.parseExpression(0)
		if err != nil {
			return nil, err
		}
		if p.peek().typ == tokComma {
			p.advance()
			last, err := p.parseExpression(0)
			if err != nil {
				return nil, err
			}
			close := p.advance()
			if close.typ != tokRParen && close.typ != tokRBracket {
				return nil, p.errorf("expected interval closing delimiter")
			}
			return &Interval{Left: first, Right: last, LeftInc: tok.typ == tokLBracket, RightInc: close.typ == tokRBracket}, nil
		}
		if tok.typ == tokLBracket || p.peek().typ != tokRParen {
			return nil, p.errorf("expected ')'")
		}
		p.advance()
		return &Group{Expression: first}, nil
	case tokOperator:
		if tok.lit == "-" || tok.lit == "+" || tok.lit == "!" || tok.lit == "~" {
			if tok.lit == "-" && p.peek().typ == tokNumber && p.peek().lit == "2147483648" {
				return p.number(p.advance(), "-")
			}
			expr, err := p.parseExpression(13)
			if err != nil {
				return nil, err
			}
			return &Unary{Operator: tok.lit, Expression: expr}, nil
		}
	}
	return nil, p.errorf("unexpected token %q", tok.lit)
}

func (p *Parser) number(tok token, sign string) (Expression, error) {
	raw := sign + tok.lit
	isFloat := strings.Contains(raw, ".")
	var value float64
	if isFloat {
		n, err := strconv.ParseFloat(raw, 32)
		if err != nil {
			return nil, fmt.Errorf("column %d: invalid float %q", tok.column, raw)
		}
		value = n
	} else {
		n, err := strconv.ParseInt(raw, 10, 32)
		if err != nil {
			return nil, fmt.Errorf("column %d: invalid int32 %q", tok.column, raw)
		}
		value = float64(n)
	}
	return &Number{Value: value, IsFloat: isFloat, Raw: raw}, nil
}

func (p *Parser) legacy(name string) (Expression, error) {
	op := p.advance().lit
	if op != "=" && op != "!=" && op != "<" && op != ">" && op != "<=" && op != ">=" {
		return nil, p.errorf("invalid legacy trigger operator %s", op)
	}
	lower := strings.ToLower(name)
	if lower != "timemod" && op != "=" && op != "!=" {
		return nil, p.errorf("%s requires = or !=", name)
	}
	if lower == "command" || lower == "name" || lower == "authorname" || (len(lower) == 6 && lower[0] == 'p' && strings.HasSuffix(lower, "name")) {
		value := p.advance()
		if value.typ != tokString {
			return nil, p.errorf("%s requires a quoted string", name)
		}
		return &Binary{Left: &Identifier{Name: name}, Operator: op, Right: &StringExpr{Value: value.lit}}, nil
	}
	if lower == "statetype" || lower == "movetype" || lower == "p2statetype" || lower == "p2movetype" || lower == "teammode" {
		value := p.advance()
		if value.typ != tokIdentifier {
			return nil, p.errorf("%s requires a literal type", name)
		}
		var rhs Expression = &Identifier{Name: value.lit}
		if lower == "teammode" {
			rhs = &StringExpr{Value: strings.ToLower(value.lit)}
		}
		return &Binary{Left: &Identifier{Name: name}, Operator: op, Right: rhs}, nil
	}
	legacy := &Legacy{Name: name, Operator: op}
	if lower == "hitdefattr" {
		for {
			value := p.advance()
			if value.typ != tokIdentifier {
				return nil, p.errorf("HitDefAttr requires literal attributes")
			}
			legacy.Arguments = append(legacy.Arguments, &StringExpr{Value: strings.ToUpper(value.lit)})
			if p.peek().typ != tokComma {
				break
			}
			p.advance()
		}
		if len(legacy.Arguments) < 2 {
			return nil, p.errorf("HitDefAttr requires state and attack attributes")
		}
		return legacy, nil
	}
	arg, err := p.legacyInteger()
	if err != nil {
		return nil, err
	}
	legacy.Arguments = []Expression{arg}
	if p.peek().typ == tokComma {
		p.advance()
		comparison := "="
		if p.peek().typ == tokOperator && isComparison(p.peek().lit) {
			if lower == "timemod" {
				return nil, p.errorf("TimeMod's remainder must be an integer literal")
			}
			comparison = p.advance().lit
		}
		if _, ok := precedence[comparison]; !ok || precedence[comparison] < 8 || precedence[comparison] > 9 {
			return nil, p.errorf("invalid legacy time comparison")
		}
		extra, err := p.legacyInteger()
		if err != nil {
			return nil, err
		}
		legacy.Arguments = append(legacy.Arguments, extra)
		legacy.ExtraOperator = comparison
	}
	if lower == "timemod" && len(legacy.Arguments) != 2 {
		return nil, p.errorf("TimeMod requires divisor and remainder")
	}
	return legacy, nil
}

func (p *Parser) legacyInteger() (Expression, error) {
	sign := ""
	if p.peek().lit == "-" || p.peek().lit == "+" {
		sign = p.advance().lit
	}
	tok := p.advance()
	if tok.typ != tokNumber || strings.Contains(tok.lit, ".") {
		return nil, p.errorf("legacy argument must be an integer literal")
	}
	return p.number(tok, sign)
}
func isLegacy(name string) bool {
	switch name {
	case "animelem", "timemod", "hitdefattr", "command", "name", "authorname", "p1name", "p2name", "p3name", "p4name", "statetype", "movetype", "p2statetype", "p2movetype", "teammode":
		return true
	}
	for _, prefix := range []string{"projhit", "projcontact", "projguarded"} {
		if strings.HasPrefix(name, prefix) {
			suffix := strings.TrimPrefix(name, prefix)
			if suffix == "" {
				return true
			}
			if _, err := strconv.ParseUint(suffix, 10, 31); err == nil {
				return true
			}
		}
	}
	return false
}

func isComparison(op string) bool {
	switch op {
	case "=", "==", "!=", "<", ">", "<=", ">=":
		return true
	}
	return false
}
func redirectName(name string) bool {
	switch name {
	case "parent", "root", "helper", "target", "partner", "enemy", "enemynear", "playerid":
		return true
	}
	return false
}
func componentTrigger(name string) bool {
	switch name {
	case "pos", "vel", "p2dist", "p2bodydist", "parentdist", "rootdist", "screenpos", "camerapos", "hitvel":
		return true
	}
	return false
}
func (p *Parser) peek() token { return p.tokens[p.pos] }
func (p *Parser) advance() token {
	tok := p.peek()
	if p.pos < len(p.tokens)-1 {
		p.pos++
	}
	return tok
}
func (p *Parser) errorf(format string, args ...any) error {
	return fmt.Errorf("column %d: %s", p.peek().column, fmt.Sprintf(format, args...))
}
