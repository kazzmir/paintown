package evaluator

import (
	"fmt"
	"strconv"
	"strings"
)

// ValidateVersion covers the measured 1.1-only viewport surface. Other language
// and controller version rules remain separate compatibility obligations.
func ValidateVersion(expr Expression, version string) error {
	if version == "" {
		return nil
	}
	number, err := strconv.ParseFloat(strings.TrimSpace(version), 64)
	if err == nil && number >= 1.1 {
		return nil
	}
	var children []Expression
	switch e := expr.(type) {
	case *Identifier:
		switch normalizeIdentifier(e.Name) {
		case "screenwidth", "screenheight", "camerazoom":
			return fmt.Errorf("trigger %s requires character mugenversion 1.1 (got %s)", e.Name, version)
		}
	case *Call:
		children = e.Arguments
	case *Binary:
		children = []Expression{e.Left, e.Right}
	case *Unary:
		children = []Expression{e.Expression}
	case *Group:
		children = []Expression{e.Expression}
	case *Interval:
		children = []Expression{e.Left, e.Right}
	case *Redirect:
		children = []Expression{e.Selector, e.Trigger}
	case *Legacy:
		children = e.Arguments
	}
	for _, child := range children {
		if err := ValidateVersion(child, version); err != nil {
			return err
		}
	}
	return nil
}

func ParseReferenceForVersion(source, version string) (Expression, error) {
	expr, err := ParseReference(source)
	if err == nil {
		err = ValidateVersion(expr, version)
	}
	return expr, err
}

func ParseReferenceTupleForVersion(source string, count int, version string) ([]Expression, error) {
	expressions, err := ParseReferenceTuple(source, count)
	if err != nil {
		return nil, err
	}
	for _, expr := range expressions {
		if err := ValidateVersion(expr, version); err != nil {
			return nil, err
		}
	}
	return expressions, nil
}
