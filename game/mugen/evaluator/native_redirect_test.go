package evaluator

import (
	"bytes"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

type nativeRedirectHost struct {
	*fixtureEnvironment
	other *nativeRedirectHost
}

func (e *nativeRedirectHost) GetVel() (float64, float64) { return 0, 0 }
func (e *nativeRedirectHost) Redirect(name string, id Value, hasID bool) EvaluatorEnvironment {
	if name == "enemy" && !hasID {
		return e.other
	}
	return nil
}

func TestNativeRedirectArgumentScopeWithoutDisplay(t *testing.T) {
	stem := "../tests/testdata/reference/redirect-arguments"
	data, err := os.ReadFile(stem + ".json")
	if err != nil {
		t.Fatal(err)
	}
	var reference struct {
		Hash     string `json:"cns_sha256"`
		Captures []struct{ Variables map[string]float64 }
	}
	if err := json.Unmarshal(data, &reference); err != nil {
		t.Fatal(err)
	}
	source, err := os.ReadFile(stem + ".cns")
	if err != nil {
		t.Fatal(err)
	}
	sum := sha256.Sum256(source)
	if hex.EncodeToString(sum[:]) != reference.Hash {
		t.Fatal("native source hash differs")
	}
	file, err := cns.Parse(bytes.NewReader(source))
	if err != nil {
		t.Fatal(err)
	}
	origin := &nativeRedirectHost{fixtureEnvironment: &fixtureEnvironment{}}
	destination := &nativeRedirectHost{fixtureEnvironment: &fixtureEnvironment{}}
	origin.other, destination.other = destination, origin
	apply := func(ctrl cns.StateController, env *nativeRedirectHost) {
		t.Helper()
		expr, err := ParseReferenceForVersion(ctrl.Params["value"], "1.1")
		if err != nil {
			t.Fatal(err)
		}
		value := EvaluateValue(expr, env)
		if value.RuntimeError() != nil {
			t.Fatal(value.RuntimeError())
		}
		if index, present := ctrl.Params["fv"]; present {
			i, err := strconv.Atoi(index)
			if err != nil {
				t.Fatal(err)
			}
			env.SetFVar(i, value.Number())
		} else {
			i, err := strconv.Atoi(ctrl.Params["v"])
			if err != nil {
				t.Fatal(err)
			}
			env.SetVar(i, int(value.Number()))
		}
	}
	for _, ctrl := range file.States[701].Controllers {
		apply(ctrl, destination)
	}
	for _, ctrl := range file.States[700].Controllers {
		if strings.ReplaceAll(ctrl.Triggers[0], " ", "") == "trigger1=Time=0" {
			apply(ctrl, origin)
		}
	}
	for _, ctrl := range file.States[700].Controllers {
		if strings.ReplaceAll(ctrl.Triggers[0], " ", "") == "trigger1=Time=2" {
			apply(ctrl, origin)
		}
	}
	for _, capture := range reference.Captures {
		for key, want := range capture.Variables {
			index, err := strconv.Atoi(key)
			if err != nil {
				t.Fatal(err)
			}
			if got := origin.fvars[index]; float32(got) != float32(want) {
				t.Errorf("fvar(%d)=%v native=%v", index, got, want)
			}
		}
	}
}
