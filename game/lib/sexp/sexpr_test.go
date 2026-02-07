package sexp

import (
    "testing"
    "strings"
)

func TestTokenize(test *testing.T) {
    tokens := tokenize(strings.NewReader("(hello world)"))
    if len(tokens) != 4 {
        test.Fatalf("Expected 4 tokens, got %d", len(tokens))
    }

    expected := []string{"(", "hello", "world", ")"}
    for i, token := range expected {
        if tokens[i] != token {
            test.Errorf("Expected token '%s', got '%s'", token, tokens[i])
        }
    }
}

func TestSExpr1(test *testing.T) {
    sexpr, err := parseSExpr(strings.NewReader("(hello)"))
    if err != nil {
        test.Fatalf("Error parsing sexpr: %s", err)
    }

    if sexpr.Name != "hello" {
        test.Errorf("Expected name 'hello', got '%s'", sexpr.Name)
    }

    if len(sexpr.Children) != 0 {
        test.Errorf("Expected no children, got %d", len(sexpr.Children))
    }

    s2, err := parseSExpr(strings.NewReader("(hello (nested) world)"))
    if err != nil {
        test.Fatalf("Unexpected error: %s", err)
    }

    if s2.Name != "hello" {
        test.Errorf("Expected name 'hello', got '%s'", s2.Name)
    }

    if len(s2.Children) != 2 {
        test.Errorf("Expected 2 children, got %d", len(s2.Children))
    }
}

func TestSexprString(test *testing.T) {
    sexpr, err := parseSExpr(strings.NewReader(`("this is a string" "another string")`))
    if err != nil {
        test.Fatalf("Error parsing sexpr: %s", err)
    }

    if sexpr.Name != "this is a string" {
        test.Errorf("Expected name 'this is a string', got '%s'", sexpr.Name)
    }

    if len(sexpr.Children) != 1 {
        test.Fatalf("Expected 1 children, got %d", len(sexpr.Children))
    }

    if sexpr.Children[0].Name != "another string" {
        test.Errorf("Expected child name 'another string', got '%s'", sexpr.Children[0].Name)
    }
}

func TestSExprComment(test *testing.T) {
    sexpr, err := parseSExpr(strings.NewReader(`(hello ; this is a comment
world)`))

    if err != nil {
        test.Fatalf("Error parsing sexpr: %s", err)
    }

    if sexpr.Name != "hello" {
        test.Errorf("Expected name 'hello', got '%s'", sexpr.Name)
    }

    if len(sexpr.Children) != 1 {
        test.Fatalf("Expected 1 children, got %d", len(sexpr.Children))
    }

    if sexpr.Children[0].Name != "world" {
        test.Errorf("Expected child name 'world', got '%s'", sexpr.Children[0].Name)
    }
}
