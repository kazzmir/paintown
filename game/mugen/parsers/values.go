package parsers

import (
	"strconv"
	"strings"
)

// StripComment removes semicolon comments while preserving quoted semicolons.
func StripComment(s string) string {
	quoted := false
	for i, ch := range s {
		if ch == '"' {
			quoted = !quoted
		}
		if ch == ';' && !quoted {
			return s[:i]
		}
	}
	return s
}

// SplitList separates tuple fields without splitting calls, intervals or strings.
// Interval delimiters can be mixed: (a,b] and [a,b) are both legal.
func SplitList(s string) []string {
	var parts []string
	depth, start := 0, 0
	quoted := false
	for i, ch := range s {
		if ch == '"' {
			quoted = !quoted
		}
		if quoted {
			continue
		}
		switch ch {
		case '(', '[':
			depth++
		case ')', ']':
			depth--
		case ',':
			if depth == 0 {
				parts = append(parts, strings.TrimSpace(s[start:i]))
				start = i + 1
			}
		}
	}
	return append(parts, strings.TrimSpace(s[start:]))
}

func containerValue(raw string) Value {
	parts := SplitList(raw)
	if len(parts) > 1 {
		list := &ValueList{}
		for _, part := range parts {
			list.Values = append(list.Values, containerValue(part))
		}
		return list
	}
	if len(raw) >= 2 && raw[0] == '"' && raw[len(raw)-1] == '"' && strings.Count(raw, "\"") == 2 {
		return StringValue{Val: raw[1 : len(raw)-1]}
	}
	if n, err := strconv.ParseFloat(raw, 64); err == nil {
		return NumberValue{Val: n}
	}
	return KeywordValue{Val: raw}
}
