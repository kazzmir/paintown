package engine

import (
	"github.com/kazzmir/paintown/game/mugen/combat"
	"math"
	"strings"
	"testing"
)

type parameterHost struct {
	*host
	floats                     [40]float64
	sys                        [5]int
	sysFloats                  [5]float64
	x, y, px, py               float64
	defence                    float64
	warnings                   []string
	parent                     Environment
	overrides                  EntryOverrides
	self                       bool
	quote, removed, combo      int
	paletteSource, paletteDest [2]int
	override                   combat.HitOverride
}

func newParameterHost() *parameterHost {
	return &parameterHost{host: newHost(), defence: 1, x: 2, y: -3, px: 30, py: -10}
}
func (h *parameterHost) GetFVar(i int) float64          { return h.floats[i] }
func (h *parameterHost) SetFVar(i int, n float64)       { h.floats[i] = n }
func (h *parameterHost) GetSysVar(i int) int            { return h.sys[i] }
func (h *parameterHost) SetSysVar(i, n int)             { h.sys[i] = n }
func (h *parameterHost) GetSysFVar(i int) float64       { return h.sysFloats[i] }
func (h *parameterHost) SetSysFVar(i int, n float64)    { h.sysFloats[i] = n }
func (h *parameterHost) EvaluationWarning(s string)     { h.warnings = append(h.warnings, s) }
func (h *parameterHost) GetVel() (float64, float64)     { return h.x, h.y }
func (h *parameterHost) SetVelocity(x, y float64)       { h.x, h.y = x, y }
func (h *parameterHost) AddVelocity(x, y float64)       { h.x += x; h.y += y }
func (h *parameterHost) VelMul(x, y float64)            { h.x *= x; h.y *= y }
func (h *parameterHost) GetPos() (float64, float64)     { return h.px, h.py }
func (h *parameterHost) SetPosition(x, y float64)       { h.px, h.py = x, y }
func (h *parameterHost) AddPosition(x, y float64)       { h.px += x; h.py += y }
func (h *parameterHost) GetFacing() int                 { return -1 }
func (h *parameterHost) GetLife() int                   { return h.life }
func (h *parameterHost) GetDefenceMultiplier() float64  { return h.defence }
func (h *parameterHost) ParentEnvironment() Environment { return h.parent }
func (h *parameterHost) VictoryQuote(n int)             { h.quote = n }
func (h *parameterHost) RemoveExplod(n int)             { h.removed = n }
func (h *parameterHost) HitAdd(n int)                   { h.combo += n }
func (h *parameterHost) SetPaletteMapping(source, dest [2]int) error {
	h.paletteSource, h.paletteDest = source, dest
	return nil
}
func (h *parameterHost) SetHitOverride(value combat.HitOverride) error {
	h.override = value
	return nil
}
func (h *parameterHost) GetHitVar(name string) float64 {
	if name == "xvel" {
		return -4
	}
	return -6
}
func (h *parameterHost) Transition(n int, o EntryOverrides, self bool) error {
	h.ChangeState(n, -1, -1)
	h.overrides, h.self = o, self
	return nil
}

func runController(t *testing.T, h *parameterHost, name, params string) {
	t.Helper()
	state := stateSource(t, "[State 7, test]\ntype="+name+"\ntrigger1=1\n"+params+"\n")
	if _, err := NewStateMachine().ExecuteState(state, h); err != nil {
		t.Fatal(err)
	}
}

func TestVariableExpressions(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "VarSet", "v=1+1\nvalue=7.9")
	if h.vars[2] != 7 || len(h.warnings) != 1 {
		t.Fatalf("vars/warnings = %v/%v", h.vars, h.warnings)
	}
	runController(t, h, "VarAdd", "var(1+1)=2147483647")
	if h.vars[2] != math.MinInt32+6 {
		t.Fatal("integer addition did not wrap as int32")
	}
	runController(t, h, "VarSet", "fv=2\nvalue=1.0/3")
	if h.floats[2] != float64(float32(1.0/3)) {
		t.Fatalf("float32 result = %g", h.floats[2])
	}
	runController(t, h, "VarAdd", "fvar(2)=1.0")
	if h.floats[2] != float64(float32(1.0/3)+float32(1)) {
		t.Fatal("fvar addition")
	}
	runController(t, h, "VarSet", "sysvar(4)=12")
	runController(t, h, "VarSet", "sysfvar(4)=2.5")
	if h.sys[4] != 12 || h.sysFloats[4] != 2.5 {
		t.Fatal("system variable forms")
	}
}

func TestCompiledPaletteParameters(t *testing.T) {
	h := newParameterHost()
	h.vars[0] = 3
	runController(t, h, "RemapPal", "source=1,ifelse(1,2,3)\ndest=1,var(0)+1")
	if h.paletteSource != [2]int{1, 2} || h.paletteDest != [2]int{1, 4} {
		t.Fatal("palette expressions/tuple nesting", h.paletteSource, h.paletteDest)
	}
	runController(t, h, "RemapPal", "")
	if h.paletteSource != [2]int{-1, 0} || h.paletteDest != [2]int{-1, 0} {
		t.Fatal("palette omission defaults")
	}
	for _, params := range []string{"source=1\ndest=1,2", "source=1,2,3", "dest=1,1/0"} {
		state := stateSource(t, "[State 7, bad palette]\ntype=RemapPal\ntrigger1=1\n"+params+"\n")
		if _, err := NewStateMachine().ExecuteState(state, h); err == nil {
			t.Fatal("bad palette parameter accepted", params)
		}
		if h.paletteSource != [2]int{-1, 0} || h.paletteDest != [2]int{-1, 0} {
			t.Fatal("bad parameters mutated mapping")
		}
	}
}

func TestCompiledHitOverrideParameters(t *testing.T) {
	h := newParameterHost()
	h.vars[0] = 2
	runController(t, h, "HitOverride", "attr=SA,AP\nstateno=700+var(0)\nslot=1+1\ntime=2+3\nforceair=1")
	hit, _ := combat.ParseAttributes("A,HP")
	if h.override.StateNo != 702 || h.override.Slot != 2 || h.override.Time != 5 || !h.override.ForceAir || !h.override.Attributes.Matches(hit) {
		t.Fatal("override expressions/filter", h.override)
	}
	runController(t, h, "HitOverride", "attr=SCA,NA\nstateno=710")
	if h.override.Slot != 0 || h.override.Time != 1 || h.override.ForceAir {
		t.Fatal("override defaults", h.override)
	}
	before := h.override
	for _, params := range []string{"attr=S,XX\nstateno=710", "attr=S,NA\nstateno=1/0", "attr=S,NA\nstateno=710\nslot=8"} {
		state := stateSource(t, "[State 7, invalid override]\ntype=HitOverride\ntrigger1=1\n"+params+"\n")
		if _, err := NewStateMachine().ExecuteState(state, h); err == nil {
			t.Fatal("invalid override accepted", params)
		}
		if h.override != before {
			t.Fatal("invalid override mutated slot")
		}
	}
}

func TestParentVariablesUseChildExpressions(t *testing.T) {
	parent, child := newParameterHost(), newParameterHost()
	child.parent, child.vars[0], parent.vars[0] = parent, 3, 100
	runController(t, child, "ParentVarSet", "v=var(0)\nvalue=var(0)+4")
	runController(t, child, "ParentVarAdd", "v=3\nvalue=2")
	if parent.vars[3] != 9 || child.vars[3] != 0 {
		t.Fatal("parent destination or evaluation context")
	}
	child.parent = nil
	runController(t, child, "ParentVarSet", "v=3\nvalue=99")
	if child.vars[3] != 0 {
		t.Fatal("root ParentVarSet must do nothing")
	}
}

func TestVariableRangesEvaluateValueOnce(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "VarRangeSet", "first=1\nlast=3\nvalue=var(0):=var(0)+1")
	if h.vars[0] != 1 || h.vars[1] != 1 || h.vars[2] != 1 || h.vars[3] != 1 || h.vars[4] != 0 {
		t.Fatal(h.vars)
	}
	runController(t, h, "VarRangeSet", "fvalue=2.5")
	if h.floats[0] != 2.5 || h.floats[39] != 2.5 {
		t.Fatal("float range defaults")
	}
	runController(t, h, "VarRangeSet", "value=7")
	if h.vars[0] != 7 || h.vars[59] != 7 {
		t.Fatal("integer range defaults")
	}
}

func TestVectorExpressionsAndOmittedAxes(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "VelSet", "x=(var(0):=1)+0.25\ny=(var(0):=var(0)+1)*-1")
	if h.x != 1.25 || h.y != -2 || h.vars[0] != 2 {
		t.Fatalf("velocity = %g,%g, var=%d", h.x, h.y, h.vars[0])
	}
	runController(t, h, "VelMul", "y=2")
	if h.x != 1.25 || h.y != -4 {
		t.Fatal("VelMul omitted x")
	}
	runController(t, h, "VelAdd", "x=0.5")
	if h.x != 1.75 || h.y != -4 {
		t.Fatal("VelAdd omitted y")
	}
	runController(t, h, "PosSet", "y=-2")
	runController(t, h, "PosAdd", "x=5")
	if h.px != 25 || h.py != -2 {
		t.Fatalf("position = %g,%g", h.px, h.py)
	}
	runController(t, h, "HitVelSet", "x=-3\ny=0")
	if h.x != -4 || h.y != -4 {
		t.Fatal("HitVelSet flag must not multiply hit velocity")
	}
}

func TestLifeAddDefenceAndKill(t *testing.T) {
	h := newParameterHost()
	h.life, h.defence = 20, 2
	runController(t, h, "LifeAdd", "value=-10")
	if h.life != 15 {
		t.Fatal(h.life)
	}
	runController(t, h, "LifeAdd", "value=-10\nabsolute=1")
	if h.life != 5 {
		t.Fatal(h.life)
	}
	runController(t, h, "LifeAdd", "value=-20\nkill=0")
	if h.life != 1 {
		t.Fatal(h.life)
	}
}

func TestOptionalAndTransitionParameters(t *testing.T) {
	h := newParameterHost()
	runController(t, h, "VictoryQuote", "")
	if h.quote != -1 {
		t.Fatal("random victory quote default")
	}
	runController(t, h, "RemoveExplod", "")
	if h.removed != -1 {
		t.Fatal("remove all default")
	}
	runController(t, h, "SelfState", "value=200+var(0)\nctrl=-1\nanim=-1")
	if h.state != 200 || !h.self || !h.overrides.HasCtrl || !h.overrides.Ctrl || !h.overrides.HasAnim || h.overrides.Anim != -1 {
		t.Fatalf("transition = %+v", h)
	}
}

func TestBadControllerParametersFailBeforeStateEffects(t *testing.T) {
	for _, bad := range []string{
		"type=LifeAdd", "type=VarSet\nv=0", "type=VarSet\nv=0\nfv=0\nvalue=1",
		"type=VelSet\nx=1@2", "type=ParentVarSet\nsysvar(0)=1",
		"type=VarRangeSet\nvalue=1\nfvalue=1.0",
		"type=LifeAdd\nvalue=Const(data.unknown)",
		"type=LifeAdd\nvalue=Const(movement.jump.changeanim.threshold)",
	} {
		h := newParameterHost()
		state := stateSource(t, lifeController("trigger1=1")+"[State 7, bad]\ntrigger1=1\n"+bad+"\n")
		_, err := NewStateMachine().ExecuteState(state, h)
		if err == nil || !strings.Contains(err.Error(), "state 7 controller 1") {
			t.Fatalf("%s: %v", bad, err)
		}
		if h.life != 0 {
			t.Fatal("compiled invalid state partially executed")
		}
	}
	h := newParameterHost()
	state := stateSource(t, "[State 7, invalid runtime]\ntype=VelSet\ntrigger1=1\nx=5\ny=1/0\n")
	if _, err := NewStateMachine().ExecuteState(state, h); err == nil {
		t.Fatal("accepted bottom argument")
	}
	if h.x != 2 || h.y != -3 {
		t.Fatal("applied incomplete vector")
	}
}
