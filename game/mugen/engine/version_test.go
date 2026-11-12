package engine

import (
	"strings"
	"testing"
)

func TestVersionValidationDoesNotFollowTheExpressionCacheOwner(t *testing.T) {
	machine := NewStateMachine()
	source := "[State 7, gate]\ntype=Null\ntrigger1=ScreenWidth>0\n"
	modern, legacy := stateSource(t, source), stateSource(t, source)
	modern.LanguageVersion, legacy.LanguageVersion = "1.1", "1.0"
	if err := machine.CompileState(modern); err != nil {
		t.Fatal(err)
	}
	if err := machine.CompileState(legacy); err == nil || !strings.Contains(err.Error(), "mugenversion 1.1") {
		t.Fatal("cached modern expression bypassed legacy gate", err)
	}
	if legacy.Controllers[0].LanguageVersion != "" {
		t.Fatal("compilation changed shared controller metadata")
	}
	entry := stateSource(t, "[State 7, keep]\ntype=Null\ntrigger1=1\n")
	entry.LanguageVersion = "1.0"
	entry.Parameters["velset"] = "ScreenHeight,0"
	if err := machine.CompileState(entry); err == nil {
		t.Fatal("entry expression bypassed version validation")
	}
}
