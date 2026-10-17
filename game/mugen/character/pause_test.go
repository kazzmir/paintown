package character

import (
	"bytes"
	"encoding/json"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func TestNativePauseRootClocks(t *testing.T) {
	for _, name := range []string{"pause-three", "pause-move-one", "pause-move-all", "pause-zero", "pause-one", "pause-other", "pause-missing", "pause-negative", "pause-overmove", "pause-negmove", "pause-replace", "pause-cancel", "pause-pair"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var reference struct {
				CNSHash  string `json:"cns_sha256"`
				AIRHash  string `json:"air_sha256"`
				Captures []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &reference); err != nil {
				t.Fatal(err)
			}
			source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
			frames := checkedAudioInput(t, stem+".air", reference.AIRHash)
			file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
			if err != nil {
				t.Fatal(err)
			}
			file.SetLanguageVersion("1.1")
			file.Data.Life, file.Data.Power = 1000, 3000
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
			for slot, c := range players {
				c.PlayerID, c.Facing, c.X = slot+1, 1-2*slot, float64(-70+140*slot)
				c.ChangeState(0, -1, -1)
				c.Ctrl = true
			}
			for tick := 0; tick < 50; tick++ {
				if err := TickCharacters(players); err != nil {
					t.Fatal(err)
				}
			}
			for _, capture := range reference.Captures {
				for key, want := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					if got := players[0].FloatVars[index]; float32(got) != float32(want) {
						t.Errorf("fvar(%d)=%v native=%v", index, got, want)
					}
				}
			}
		})
	}
}

// Command buffering has authored documentation-based checks until the native
// harness can inject input into its private window. Root clock cases above do
// not establish command grammar/buffering parity.
func TestPauseEndCommandBufferWindow(t *testing.T) {
	for _, buffer := range []int{0, 2} {
		t.Run(strconv.Itoa(buffer), func(t *testing.T) {
			file, err := cns.Parse(strings.NewReader("[Statedef 0]\nphysics=N\n[State 0, command]\ntype=VarAdd\ntrigger1=command=\"press\"\nv=0\nvalue=1\n"))
			if err != nil {
				t.Fatal(err)
			}
			commands, err := cmd.Parse(strings.NewReader("[Command]\nname=\"press\"\ncommand=x\ntime=10\nbuffer.time=1\n"))
			if err != nil {
				t.Fatal(err)
			}
			c := NewCharacter(file, nil, commands)
			c.Life = 1000
			c.Commands = NewCommandBuffer(120)
			frame := 0
			c.InputSource = func(int, int) []string {
				if frame == 1 {
					return []string{"ButtonX"}
				}
				return nil
			}
			c.Timeline.BeginFrame()
			if err := c.PauseGame(engine.PauseParameters{Time: 3, EndCommandBufferTime: buffer}); err != nil {
				t.Fatal(err)
			}
			c.Timeline.EndFrame()
			for ; frame < 4; frame++ {
				c.Update()
				if c.RuntimeError != nil {
					t.Fatal(c.RuntimeError)
				}
			}
			want := 0
			if buffer != 0 {
				want = 1
			}
			if c.IntVars[0] != want {
				t.Fatal("paused command window mismatch", c.IntVars[0], want)
			}
		})
	}
}
