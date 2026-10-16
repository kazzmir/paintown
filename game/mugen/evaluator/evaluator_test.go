package evaluator

import (
	"math"
	"strings"
	"testing"
)

// Embedding the interface keeps unsupported host calls visible as test panics;
// these language tests exercise only the explicitly provided context methods.
type fixtureEnvironment struct {
	EvaluatorEnvironment
	time     int
	vars     [60]int
	fvars    [40]float64
	warnings []string
	parent   *fixtureEnvironment
}

func (e *fixtureEnvironment) GetTime() int               { return e.time }
func (e *fixtureEnvironment) GetVar(i int) int           { return e.vars[i] }
func (e *fixtureEnvironment) SetVar(i, v int)            { e.vars[i] = v }
func (e *fixtureEnvironment) GetFVar(i int) float64      { return e.fvars[i] }
func (e *fixtureEnvironment) SetFVar(i int, v float64)   { e.fvars[i] = v }
func (e *fixtureEnvironment) GetStateType() string       { return "A" }
func (e *fixtureEnvironment) GetMoveType() string        { return "I" }
func (e *fixtureEnvironment) Command(name string) bool   { return name == "a" }
func (e *fixtureEnvironment) EvaluationWarning(s string) { e.warnings = append(e.warnings, s) }
func (e *fixtureEnvironment) Redirect(name string, id Value, hasID bool) EvaluatorEnvironment {
	if name == "parent" && !hasID && e.parent != nil {
		return e.parent
	}
	return nil
}

func evaluate(t *testing.T, source string, env EvaluatorEnvironment) Value {
	t.Helper()
	expr, err := Parse(source)
	if err != nil {
		t.Fatalf("%s: %v", source, err)
	}
	return EvaluateValue(expr, env)
}

func TestTypedArithmetic(t *testing.T) {
	cases := []struct {
		source string
		kind   Kind
		value  float64
	}{
		{"5/6", IntKind, 0}, {"5.0/6", FloatKind, float64(float32(5.0 / 6))},
		{"7/-2", IntKind, -3}, {"7%-2", IntKind, 1},
		{"2**3**2", IntKind, 64}, {"-2**2", FloatKind, 4},
		{"0**0", IntKind, 1}, {"2**31", IntKind, math.MaxInt32},
		{"2**-1", FloatKind, 0.5}, {"-2147483648", IntKind, math.MinInt32},
		{"3+2*5", IntKind, 13}, {"~0", IntKind, -1}, {"6&3", IntKind, 2},
		{"6|3", IntKind, 7}, {"6^3", IntKind, 5}, {"2^^0", IntKind, 1},
		{"2^^3", IntKind, 0}, {"0||1&&0", IntKind, 0},
		{"16777217=16777216.0", IntKind, 1},
		{"(1=[0,2]) = (0,1)", IntKind, 0}, {"1=[0.0,1]", IntKind, 1},
		{"1=(0,1)", IntKind, 0}, {"1!=(0,1)", IntKind, 1},
		{"floor(-1.2)", IntKind, -2}, {"ceil(-1.2)", IntKind, -1},
		{"abs(-3)", IntKind, 3}, {"abs(-3.0)", FloatKind, 3},
		{"sin(0)", FloatKind, 0}, {"cos(0)", FloatKind, 1},
		{"ln(exp(1))", FloatKind, float64(float32(math.Log(float64(float32(math.E)))))},
		{"log(2,8)", FloatKind, 3},
	}
	for _, tc := range cases {
		t.Run(tc.source, func(t *testing.T) {
			got := evaluate(t, tc.source, nil)
			if got.Kind() != tc.kind || got.Number() != tc.value {
				t.Fatalf("got %v (kind %d), want %g (kind %d)", got, got.Kind(), tc.value, tc.kind)
			}
		})
	}
}

func TestBottomPropagation(t *testing.T) {
	for _, source := range []string{"1/0", "1%0", "1.0%2", "~1.0", "1.0&1", "(-1)**0.5", "ln(0)", "acos(2)", "log(1,2)", "exp(1000)", "unknown", "unknown=0", "!unknown", "unknown(1)", "0&&(1/0)", "1||(1/0)", "var(-1)", "fvar(40)", "var(0.0)"} {
		t.Run(source, func(t *testing.T) {
			env := &fixtureEnvironment{}
			if got := evaluate(t, source, env); !got.IsBottom() {
				t.Fatalf("got %v, want bottom", got)
			}
			if len(env.warnings) == 0 && !strings.Contains(source, "&&") && !strings.Contains(source, "||") {
				t.Fatal("missing diagnostic")
			}
		})
	}
}

func TestAssignmentAndEvaluationOrder(t *testing.T) {
	env := &fixtureEnvironment{}
	if got := evaluate(t, "var(0):=var(1):=5", env); got.Number() != 5 || env.vars[0] != 5 || env.vars[1] != 5 {
		t.Fatalf("assignment chain: %v %+v", got, env.vars)
	}
	if got := evaluate(t, "var(0):=0||1", env); got.Number() != 1 || env.vars[0] != 0 {
		t.Fatalf("assignment precedence: %v / %d", got, env.vars[0])
	}
	if got := evaluate(t, "var(0):=3.9", env); got.Kind() != IntKind || env.vars[0] != 3 {
		t.Fatalf("integer conversion: %v", got)
	}
	if got := evaluate(t, "fvar(0):=3", env); got.Kind() != FloatKind || env.fvars[0] != 3 {
		t.Fatalf("float conversion: %v", got)
	}
	if got := evaluate(t, "0&&(var(0):=9)", env); got.Number() != 0 || env.vars[0] != 9 {
		t.Fatalf("ordinary operands were short circuited: %v", got)
	}
	cases := []struct {
		source   string
		assigned int
		warnings int
	}{
		{"ifelse(1,7,var(0):=9)", 9, 0},
		{"cond(1,7,var(0):=9)", 0, 0},
		{"ifelse(0,1/0,7)", 0, 1},
		{"cond(0,1/0,7)", 0, 0},
	}
	for _, tc := range cases {
		t.Run(tc.source, func(t *testing.T) {
			env := &fixtureEnvironment{}
			if got := evaluate(t, tc.source, env); got.Number() != 7 || got.IsBottom() || env.vars[0] != tc.assigned || len(env.warnings) != tc.warnings {
				t.Fatalf("got %v, var0=%d, warnings=%v", got, env.vars[0], env.warnings)
			}
		})
	}
}

func TestLegacyClausesAndRedirects(t *testing.T) {
	env := &fixtureEnvironment{time: 3, parent: &fixtureEnvironment{time: 10}}
	for source, want := range map[string]float64{
		"TimeMod = 4,3": 1, "TimeMod != 4,3": 0,
		`command = "a" + 4`: 5, "StateType = A + 4": 5,
		"MoveType != H": 1, "parent, time + time": 13,
		"parent, StateType = A": 1,
	} {
		if got := evaluate(t, source, env); got.IsBottom() || got.Number() != want {
			t.Errorf("%s: got %v, want %g", source, got, want)
		}
	}
	if got := evaluate(t, "helper(123),time", env); !got.IsBottom() {
		t.Fatalf("missing redirect = %v", got)
	}
}

func TestStrictExpressionParsing(t *testing.T) {
	for _, source := range []string{"([1,2])", "abs([1,2])", "1=([1,2])", "TimeMod=4,>3"} {
		if _, err := Parse(source); err == nil {
			t.Errorf("accepted %q", source)
		}
	}
	for _, source := range []string{"", "1@2", "1.2.3", "2147483648", `"unterminated`, "var(0):", "1:=2", "(var(0)):=2", "sysvar(0):=2", "[1,2]", "1>[0,2]", "1=[0,2]=[0,1]", "1=[0,2]||1", "AnimElem=(1+1)", "TimeMod=2", "parent,root,time", strings.Repeat("(", 300) + "1" + strings.Repeat(")", 300)} {
		if _, err := Parse(source); err == nil {
			t.Errorf("accepted %q", source)
		}
	}
	for _, source := range []string{"pos\tx", "AnimElem = 3, >= -1", "AnimElem = 3, -1", "HitDefAttr = SCA, NA, SA", "ProjHit123 = 1, < 15", "enemynear(0),var(1)", "ifelse(time=1, 2.0, 3)"} {
		if _, err := Parse(source); err != nil {
			t.Errorf("%s: %v", source, err)
		}
	}
}

func FuzzExpression(f *testing.F) {
	for _, source := range []string{"5/6", "ifelse(0,1/0,7)", "var(0):=3", "AnimElem=3,-1", "parent,time", "(1=[0,2])=0", "", "\xff"} {
		f.Add(source)
	}
	f.Fuzz(func(t *testing.T, source string) {
		if expr, err := Parse(source); err == nil {
			EvaluateValue(expr, nil)
		}
	})
}
