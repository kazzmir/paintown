package evaluator

import (
	"fmt"
	"strings"
)

// Names come from the supplied beta's 127 trigger entries. Recognizing a name
// does not imply its environment binding or parameter variants are implemented.
var referenceTriggerNames = func() map[string]bool {
	const names = `abs acos ailevel alive anim animelem animelemno animelemtime animexist animtime asin atan authorname backedge backedgebodydist backedgedist bottomedge camerapos camerazoom canrecover ceil command cond const const240p const480p const720p cos ctrl drawgame e exp facing floor frontedge frontedgebodydist frontedgedist fvar gameheight gametime gamewidth gethitvar hitcount hitdefattr hitfall hitover hitpausetime hitshakeover hitvel id ifelse inguarddist ishelper ishometeam leftedge life lifemax ln log lose matchno matchover movecontact moveguarded movehit movetype movereversed name numenemy numexplod numhelper numpartner numproj numprojid numtarget p1name p2bodydist p2dist p2life p2movetype p2name p2stateno p2statetype p3name p4name palno parentdist pi pos power powermax playeridexist prevstateno projcanceltime projcontact projcontacttime projguarded projguardedtime projhit projhittime random rightedge rootdist roundno roundsexisted roundstate screenpos screenheight screenwidth selfanimexist sin stateno statetype stagevar sysfvar sysvar tan teammode teamside tickspersecond time timemod topedge uniqhitcount var vel win`
	result := map[string]bool{}
	for _, name := range strings.Fields(names) {
		result[name] = true
	}
	return result
}()

// ValidateKnownTriggers provides opt-in reference name validation for compiled
// parameter schemas. Parse remains available for environment-local extensions.
// Symbolic Const/GetHitVar/StageVar fields and redirect selectors are distinct
// from ordinary trigger names.
func ValidateKnownTriggers(expr Expression) error {
	known := func(name string) bool {
		name = strings.ToLower(name)
		base, axis, component := strings.Cut(name, " ")
		if component {
			return componentTrigger(base) && (axis == "x" || axis == "y" || axis == "z")
		}
		if referenceTriggerNames[name] {
			return true
		}
		// These are suffix variants of the documented Win/Lose entries.
		switch name {
		case "winko", "wintime", "winperfect", "loseko", "losetime":
			return true
		}
		for _, prefix := range []string{"projhit", "projguarded", "projcontact"} {
			if strings.HasPrefix(name, prefix) && len(name) > len(prefix) {
				for _, digit := range name[len(prefix):] {
					if digit < '0' || digit > '9' {
						return false
					}
				}
				return true
			}
		}
		return len(name) == 1 && strings.Contains("scalihn", name)
	}
	var children []Expression
	switch value := expr.(type) {
	case *Identifier:
		if !known(value.Name) {
			return fmt.Errorf("unknown reference trigger %q", value.Name)
		}
	case *Call:
		if !known(value.Name) {
			return fmt.Errorf("unknown reference trigger %q", value.Name)
		}
		switch strings.ToLower(value.Name) {
		case "const", "gethitvar", "stagevar":
			return nil
		}
		children = value.Arguments
	case *Group:
		children = []Expression{value.Expression}
	case *Unary:
		children = []Expression{value.Expression}
	case *Binary:
		children = []Expression{value.Left, value.Right}
	case *Interval:
		children = []Expression{value.Left, value.Right}
	case *Legacy:
		if !known(value.Name) {
			return fmt.Errorf("unknown reference trigger %q", value.Name)
		}
		children = value.Arguments
	case *Redirect:
		switch selector := value.Selector.(type) {
		case *Call:
			children = append(children, selector.Arguments...)
		case *Identifier:
		default:
			return fmt.Errorf("invalid redirect selector")
		}
		children = append(children, value.Trigger)
	}
	for _, child := range children {
		if err := ValidateKnownTriggers(child); err != nil {
			return err
		}
	}
	return nil
}
