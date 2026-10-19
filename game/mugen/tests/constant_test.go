package tests

import (
	"encoding/json"
	"os"
	"strconv"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

func TestAllConstantsAgainstReference(t *testing.T) {
	f, err := os.Open(referencePath(t, "chars", "kfm", "kfm.cns"))
	if err != nil {
		t.Fatal(err)
	}
	defer f.Close()
	file, err := cns.Parse(f)
	if err != nil {
		t.Fatal(err)
	}
	host := character.NewCharacter(file, nil, nil)
	seen := make(map[string]bool)
	for _, family := range []string{"a", "b"} {
		data, err := os.ReadFile("testdata/reference/constants-" + family + ".json")
		if err != nil {
			t.Fatal(err)
		}
		var trace struct {
			Names []struct {
				Name, Kind string
				Index      int
			}
			Captures []struct{ Variables map[string]float64 }
		}
		if err := json.Unmarshal(data, &trace); err != nil {
			t.Fatal(err)
		}
		values := make(map[int]float64)
		for _, capture := range trace.Captures {
			for key, value := range capture.Variables {
				i, err := strconv.Atoi(key)
				if err != nil {
					t.Fatal(err)
				}
				values[i] = value
			}
		}
		for _, item := range trace.Names {
			seen[item.Name] = true
			expr, err := evaluator.ParseReference("Const(" + item.Name + ")")
			if item.Name == "movement.jump.changeanim.threshold" {
				if err == nil {
					t.Fatal("accepted reference-rejected constant")
				}
				continue
			}
			if err != nil {
				t.Fatal(err)
			}
			want, ok := values[item.Index]
			if !ok {
				t.Fatal("missing reference observation", item.Name)
			}
			value := evaluator.EvaluateValue(expr, host)
			kind := evaluator.FloatKind
			if item.Kind == "int" {
				kind = evaluator.IntKind
			}
			if value.Kind() != kind || float32(value.Number()) != float32(want) {
				t.Errorf("%s=%v (%v), reference=%g (%s)", item.Name, value, value.Kind(), want, item.Kind)
			}
		}
	}
	if len(seen) != 74 {
		t.Fatal("incomplete documented constant coverage", len(seen))
	}
	if len(host.RuntimeWarnings) != 0 {
		t.Fatal(host.RuntimeWarnings)
	}
}
