package character

import (
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestCharacterReferenceStatePrograms(t *testing.T) {
	for _, name := range []string{"state-order", "persistence"} {
		t.Run(name, func(t *testing.T) {
			path := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(path + ".cns")
			if err != nil {
				t.Fatal(err)
			}
			file, err := cns.Parse(strings.NewReader(string(data)))
			if err != nil {
				t.Fatal(err)
			}
			if commands, err := os.ReadFile(path + ".cmd"); err == nil {
				cmd, err := cns.Parse(strings.NewReader(string(commands)))
				if err != nil {
					t.Fatal(err)
				}
				for id, state := range cmd.States {
					file.States[id] = state
				}
			}
			c := NewCharacter(file, nil, nil)
			c.ChangeState(0, -1, -1)
			for i := 0; i < 40; i++ {
				c.Update()
			}
			if c.RuntimeError != nil {
				t.Fatal(c.RuntimeError)
			}
			data, err = os.ReadFile(path + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var trace struct {
				Captures []struct{ Variables map[string]int }
			}
			if err = json.Unmarshal(data, &trace); err != nil {
				t.Fatal(err)
			}
			for _, capture := range trace.Captures {
				for key, want := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					if c.IntVars[index] != want {
						t.Errorf("var(%d)=%d reference=%d", index, c.IntVars[index], want)
					}
				}
			}
		})
	}
}
