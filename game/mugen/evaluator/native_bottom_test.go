package evaluator

import (
	"bytes"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"os"
	"strconv"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativeBottomOperatorsWithoutDisplay(t *testing.T) {
	stem := "../tests/testdata/reference/identity-bottom-operators"
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
	env := &fixtureEnvironment{}
	for _, ctrl := range file.States[700].Controllers {
		index, err := strconv.Atoi(ctrl.Params["fv"])
		if err != nil {
			t.Fatal(err)
		}
		expr, err := ParseReferenceForVersion(ctrl.Params["value"], "1.1")
		if err != nil {
			t.Fatal(err)
		}
		value := EvaluateValue(expr, env)
		if value.RuntimeError() != nil {
			t.Fatal(value.RuntimeError())
		}
		env.fvars[index] = float64(float32(value.Number()))
	}
	for _, capture := range reference.Captures {
		for key, want := range capture.Variables {
			index, err := strconv.Atoi(key)
			if err != nil {
				t.Fatal(err)
			}
			if got := env.fvars[index]; float32(got) != float32(want) {
				t.Errorf("fvar(%d)=%v native=%v", index, got, want)
			}
		}
	}
	if value := evaluate(t, "~(parent,ID)", env); value.RuntimeError() == nil {
		t.Fatal("native malformed bottom bitwise-not was recoverable")
	}
}
