package parsers

import (
	"bufio"
	"bytes"
	"io"
	"strings"
	"unicode"
)

type TokenType int

const (
	TokenError TokenType = iota
	TokenEOF
	TokenSectionStart
	TokenSectionEnd
	TokenAssign // =
	TokenComma  // ,
	TokenLBrace // (
	TokenRBrace // )
	TokenKeyword
	TokenString // "some value"
	TokenNumber // 1.2
	TokenNewline
	TokenIdentifier
)

func (t TokenType) String() string {
	switch t {
	case TokenEOF:
		return "EOF"
	case TokenSectionStart:
		return "["
	case TokenSectionEnd:
		return "]"
	case TokenAssign:
		return "="
	case TokenComma:
		return ","
	case TokenLBrace:
		return "("
	case TokenRBrace:
		return ")"
	case TokenKeyword:
		return "Keyword"
	case TokenString:
		return "String"
	case TokenNumber:
		return "Number"
	case TokenIdentifier:
		return "Identifier"
	case TokenNewline:
		return "Newline"
	default:
		return "Unknown"
	}
}

type Token struct {
	Type   TokenType
	Lit    string
	Line   int
	Column int
}

type Lexer struct {
	r    *bufio.Reader
	line int
	col  int
	buf  bytes.Buffer
}

func NewLexer(r io.Reader) *Lexer {
	return &Lexer{
		r:    bufio.NewReader(r),
		line: 1,
		col:  1,
	}
}

func (l *Lexer) read() (rune, error) {
	ch, _, err := l.r.ReadRune()
	if err != nil {
		return 0, err
	}
	if ch == '\n' {
		l.line++
		l.col = 1
	} else {
		l.col++
	}
	return ch, nil
}

func (l *Lexer) unread(ch rune) {
	if ch == '\n' {
		l.line--
		// Note: col tracking is imperfect after an unread of \n, but it's rarely needed across newlines.
	} else {
		l.col--
	}
	l.r.UnreadRune()
}

func (l *Lexer) skipSpace() {
	for {
		ch, err := l.read()
		if err != nil {
			return
		}
		if ch != ' ' && ch != '\t' && ch != '\r' {
			l.unread(ch)
			return
		}
	}
}

func (l *Lexer) skipCommentOrNewlines() {
	// We want to skip comments but NOT newlines in standard operation because Newlines are syntax boundaries in def files.
	// We only skip comments.
	for {
		ch, err := l.read()
		if err != nil || ch == '\n' {
			if err == nil {
				l.unread(ch)
			}
			return
		}
	}
}

func (l *Lexer) NextToken() Token {
	for {
		ch, err := l.read()
		if err != nil {
			return Token{TokenEOF, "", l.line, l.col}
		}

		if ch == ' ' || ch == '\t' || ch == '\r' {
			continue
		}

		// Drop leading newlines on first encounter
		if ch == '\n' {
			if l.line == 2 && l.col == 1 {
				// The lexer starts at line 1, reading \n increments to line 2.
				// Drop the leading newline.
				continue
			}
			// Let it fall to the break so we can emit the newline properly below
		}

		if ch == ';' {
			// skip to end of line
			for {
				c2, err2 := l.read()
				if err2 != nil || c2 == '\n' {
					if err2 == nil {
						l.unread(c2) // leave newline to be processed
					}
					break
				}
			}
			continue
		}

		l.unread(ch)
		break
	}

	startLine := l.line
	startCol := l.col

	ch, err := l.read()
	if err != nil {
		return Token{TokenEOF, "", startLine, startCol}
	}

	switch ch {
	case '\n':
		// Don't emit multiple sequential newlines.
		for {
			next, err := l.read()
			if err != nil {
				return Token{TokenNewline, "\n", startLine, startCol}
			}
			if next == '\n' || next == '\r' || next == ' ' || next == '\t' {
				continue
			}
			l.unread(next)
			break
		}

		return Token{TokenNewline, "\n", startLine, startCol}
	case '[':
		// We parsed a [, let's just return it.
		// But in the identifier loop, if we are inside [], we need a way to allow spaces.
		// A cleaner way for the parser is that Lexer returns [ then an identifier that includes spaces.
		// However, it's simpler if the parser just loops identifiers/keywords until ].
		return Token{TokenSectionStart, "[", startLine, startCol}
	case ']':
		return Token{TokenSectionEnd, "]", startLine, startCol}
	case '=':
		return Token{TokenAssign, "=", startLine, startCol}
	case ',':
		return Token{TokenComma, ",", startLine, startCol}
	case '(':
		return Token{TokenLBrace, "(", startLine, startCol}
	case ')':
		return Token{TokenRBrace, ")", startLine, startCol}
	case '"':
		l.buf.Reset()
		for {
			ch2, err := l.read()
			if err != nil || ch2 == '"' || ch2 == '\n' {
				if ch2 == '\n' {
					l.unread(ch2)
				}
				break
			}
			l.buf.WriteRune(ch2)
		}
		return Token{TokenString, l.buf.String(), startLine, startCol}
	}

	// Number parsing
	if unicode.IsDigit(ch) || ch == '-' || ch == '+' || ch == '.' {
		l.buf.Reset()
		l.buf.WriteRune(ch)
		for {
			ch2, err := l.read()
			if err != nil {
				break
			}
			if unicode.IsDigit(ch2) || ch2 == '.' {
				l.buf.WriteRune(ch2)
			} else {
				l.unread(ch2)
				break
			}
		}

		val := l.buf.String()
		if val != "-" && val != "+" && val != "." {
			return Token{TokenNumber, val, startLine, startCol}
		}

		// If it's just a sign or dot, it's not a number. Fallthrough.
		for i := 0; i < len([]rune(val))-1; i++ {
			// Unread everything but the first char
			l.col--
			l.r.UnreadRune()
		}
		ch = rune(val[0])
	}

	if unicode.IsLetter(ch) || ch == '_' {
		l.buf.Reset()
		l.buf.WriteRune(ch)
		for {
			ch2, err := l.read()
			if err != nil {
				break
			}
			if ch2 == ' ' {
				// Internal space check: only allow if followed by a letter/digit/underscore
				next, err3 := l.read()
				if err3 == nil {
					if unicode.IsLetter(next) || unicode.IsDigit(next) || next == '_' || next == '.' || next == '-' || next == '/' || next == '\\' || next == ':' || next == '+' {
						l.buf.WriteRune(' ')
						l.buf.WriteRune(next)
						continue
					}
					l.unread(next)
				}
				l.unread(ch2)
				break
			}
			if unicode.IsLetter(ch2) || unicode.IsDigit(ch2) || ch2 == '_' || ch2 == '.' || ch2 == '-' || ch2 == '/' || ch2 == '\\' || ch2 == ':' || ch2 == '+' {
				l.buf.WriteRune(ch2)
			} else {
				l.unread(ch2)
				break
			}
		}

		val := l.buf.String()
		// If it has a dot it's part of an identifier or complicated keyword.
		return Token{TokenIdentifier, val, startLine, startCol}
	}

	// Default to keyword, filepaths, etc that aren't strings
	l.buf.Reset()
	l.buf.WriteRune(ch)
	for {
		ch2, err := l.read()
		if err != nil {
			break
		}
		// Keywords space separated. In MUGEN everything after = to a comma or newline is a keyword value string if unquoted.
		// However, section headers can have special characters like + that shouldn't break the name unless it's a delimiter.
		if ch2 == '\n' || ch2 == ',' || ch2 == ';' || ch2 == '\r' || ch2 == ' ' || ch2 == '\t' || ch2 == ']' {
			l.unread(ch2)
			break
		}
		l.buf.WriteRune(ch2)
	}
	return Token{TokenKeyword, strings.TrimSpace(l.buf.String()), startLine, startCol}
}
