package evaluator

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

// ParseReference adds static constant validation to syntax parsing. The broader
// world-trigger/controller schema remains separate work; no environment is
// consulted and no expressions execute during this validation.
func ParseReference(source string) (Expression, error) {
	expr, err := Parse(source)
	if err != nil {
		return nil, err
	}
	if err := validateConstants(expr); err != nil {
		return nil, err
	}
	return expr, nil
}

func validateConstants(expr Expression) error {
	var children []Expression
	switch e := expr.(type) {
	case *Call:
		name := strings.ToLower(e.Name)
		if name == "gethitvar" {
			if len(e.Arguments) != 1 {
				return fmt.Errorf("GetHitVar requires one literal selector")
			}
			selector := ""
			switch arg := e.Arguments[0].(type) {
			case *Identifier:
				selector = arg.Name
			case *StringExpr:
				selector = arg.Value
			}
			if !knownGetHitSelector(selector) {
				return fmt.Errorf("GetHitVar: invalid selector %s", e.Arguments[0].String())
			}
			return nil
		}
		if (name == "const240p" || name == "const480p" || name == "const720p") && len(e.Arguments) != 1 {
			return fmt.Errorf("%s requires one expression", e.Name)
		}
		if strings.EqualFold(e.Name, "const") {
			if len(e.Arguments) != 1 {
				return fmt.Errorf("Const requires one constant name")
			}
			name, ok := e.Arguments[0].(*Identifier)
			if !ok || !cns.ConstantSupported(name.Name) {
				return fmt.Errorf("Const argument %s is unknown or unavailable in MUGEN 1.1 beta", e.Arguments[0].String())
			}
			return nil
		}
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
		if err := validateConstants(child); err != nil {
			return err
		}
	}
	return nil
}

// The manual's argument summary lists hitid, but the beta rejects it. The
// detailed selector list uses chainid for the last authored HitDef ID instead.
func knownGetHitSelector(name string) bool {
	switch strings.ToLower(name) {
	case "xveladd", "yveladd", "type", "animtype", "airtype", "groundtype", "damage", "hitcount", "fallcount", "hitshaketime", "hittime", "slidetime", "ctrltime", "recovertime", "xoff", "yoff", "zoff", "xvel", "yvel", "yaccel", "chainid", "guarded", "isbound", "fall", "fall.damage", "fall.xvel", "fall.yvel", "fall.recover", "fall.time", "fall.recovertime", "fall.kill", "fall.envshake.time", "fall.envshake.freq", "fall.envshake.ampl", "fall.envshake.phase":
		return true
	}
	return false
}
