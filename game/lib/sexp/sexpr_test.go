package sexp

import (
    "testing"
    "strings"
)

func TestSExpr1(test *testing.T) {
    sexpr, err := parseSExpr(strings.NewReader("(hello"))
    if err != nil {
        test.Logf("Expected error: %s", err)
    }

    if len(sexpr.Children) != 0 {
        test.Errorf("Expected no children, got %d", len(sexpr.Children))
    }
}
