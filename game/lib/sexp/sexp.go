package sexp

import (
    // "log"
    "io"
    "os"
    "fmt"
    "bufio"
    "strings"
)

type SExpr struct {
    Name string
    // an sexpr with no children is just a value
    Children []*SExpr
}

func (sexpr *SExpr) GetChild(name string) *SExpr {
    for _, child := range sexpr.Children {
        if child.Name == name {
            return child
        }
    }

    return nil
}

func (sexpr *SExpr) String() string {
    if sexpr.IsValue() {
        return sexpr.Name
    }

    result := "(" + sexpr.Name
    for _, child := range sexpr.Children {
        result += " " + child.String()
    }
    result += ")"

    return result
}

func (sexpr *SExpr) IsValue() bool {
    return len(sexpr.Children) == 0
}

func (sexpr *SExpr) GetValue(index int) string {
    if index < len(sexpr.Children) {
        child := sexpr.Children[index]
        if child.IsValue() {
            return child.Name
        }

        return ""
    } else {
        return ""
    }
}

// TODO: possibly store column/line position
func tokenize(reader io.Reader) []string {
    byteReader, ok := reader.(io.ByteReader)
    if !ok {
        byteReader = bufio.NewReader(reader)
    }

    var tokens []string

    whitespace := func (char byte) bool {
        return char == ' ' || char == '\t' || char == '\n' || char == '\r'
    }

    for {
        char, err := byteReader.ReadByte()
        if err != nil {
            break
        }

        switch {
            case char == '(':
                tokens = append(tokens, "(")
            case char == ')':
                tokens = append(tokens, ")")
            case char == ';':
                for char != '\n' {
                    char, err = byteReader.ReadByte()
                    if err != nil {
                        break
                    }
                }
            case char == '"':
                token := ""
                char, err = byteReader.ReadByte()
                if err != nil {
                    break
                }
                for char != '"' {
                    token += string(char)
                    char, err = byteReader.ReadByte()
                    if err != nil {
                        break
                    }
                }
                tokens = append(tokens, strings.Trim(token, `"`))

            case whitespace(char):
            default:
                token := ""
                for char != '(' && char != ')' && !whitespace(char) {
                    token += string(char)
                    char, err = byteReader.ReadByte()
                    if err != nil {
                        break
                    }
                }

                tokens = append(tokens, strings.Trim(token, `"`))

                if char == '(' {
                    tokens = append(tokens, "(")
                } else if char == ')' {
                    tokens = append(tokens, ")")
                }
        }
    }

    return tokens
}

func parseSExpr(reader io.Reader) (*SExpr, error) {
    tokens := tokenize(reader)

    var root *SExpr
    var current *SExpr
    var parents []*SExpr

    parents = append(parents, nil)

    for _, token := range tokens {
        // log.Printf("Token: %v. Parents=%v", token, parents)
        if token == "(" {
            if current != nil {
                parents = append(parents, current)
            }
            newChild := &SExpr{}
            if root == nil {
                root = newChild
            }

            if current != nil {
                current.Children = append(current.Children, newChild)
            }

            current = newChild
        } else if token == ")" {
            if len(parents) == 0 {
                return nil, fmt.Errorf("Unbalanced close parentheses")
            }
            current = parents[len(parents) - 1]
            parents = parents[:len(parents) - 1]
        } else {
            if current.Name == "" {
                current.Name = token
            } else {
                current.Children = append(current.Children, &SExpr{Name: token})
            }
        }
    }

    if len(parents) != 0 {
        return nil, fmt.Errorf("Unbalanced open parentheses: %v", len(parents))
    }

    return root, nil
}

func ReadSExpression(path string) (*SExpr, error) {
    file, err := os.Open(path)
    if err != nil {
        return nil, err
    }
    defer file.Close()

    parsed, err := parseSExpr(file)
    if err != nil {
        return nil, fmt.Errorf("Unable to parse %v: %v", path, err)
    }

    return parsed, nil
}
