package paintown

import (
    "testing"

    "github.com/kazzmir/paintown/game/lib/sexp"
)

type numeric interface {
    ~int | ~float64
}

func isClose[T numeric](a, b T) bool {
    const epsilon = 1e-4
    return float64(a-b) < epsilon && float64(b-a) < epsilon
}

func TestAnimationAttack(test *testing.T) {
    data := `
(attack
  (x1 8)
  (y1 12)
  (x2 40)
  (y2 37)
  (force 2)
  (damage 3.4))
`

    s, err := sexp.ReadSExpressionString(data)
    if err != nil {
        test.Fatalf("Failed to parse S-expression: %v", err)
    }

    attack := parseAttack(s)

    if !isClose(attack.X1, 8) {
        test.Errorf("Expected X1 to be 8, got %v", attack.X1)
    }

    if !isClose(attack.Y1, 12) {
        test.Errorf("Expected Y1 to be 12, got %v", attack.Y1)
    }

    if !isClose(attack.X2, 40) {
        test.Errorf("Expected X2 to be 40, got %v", attack.X2)
    }
    
    if !isClose(attack.Y2, 37) {
        test.Errorf("Expected Y2 to be 37, got %v", attack.Y2)
    }

    if !isClose(attack.Force, 2) {
        test.Errorf("Expected Force to be 2, got %v", attack.Force)
    }

    if !isClose(attack.Damage, 3.4) {
        test.Errorf("Expected Damage to be 3.4, got %v", attack.Damage)
    }
}
