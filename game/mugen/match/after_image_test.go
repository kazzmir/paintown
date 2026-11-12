package match

import (
	"bytes"
	"encoding/json"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
)

func TestNativeAfterImagePrograms(t *testing.T) {
	for _, group := range []struct {
		prefix string
		count  int
	}{
		{"afterpilot", 12}, {"afterhist", 32}, {"afterpos", 4}, {"afterparam", 40},
		{"afterfollow", 15}, {"afterorder", 4}, {"afteredge", 14}, {"afterphase", 8}, {"afterage", 12},
	} {
		replayNativeGeometryPrograms(t, group.prefix+"-*.json", group.count)
	}
}

func TestNativeAfterImageIntent(t *testing.T) {
	var list struct{ Programs []struct{ Fixture string } }
	for _, name := range []string{"afterimage-native-ledger.json", "afterimage-phase-native-ledger.json", "afterimage-age-native-ledger.json"} {
		ledger, err := os.ReadFile(filepath.Join("../tests/testdata/reference", name))
		if err != nil {
			t.Fatal(err)
		}
		var group struct{ Programs []struct{ Fixture string } }
		if err = json.Unmarshal(ledger, &group); err != nil {
			t.Fatal(err)
		}
		list.Programs = append(list.Programs, group.Programs...)
	}
	if len(list.Programs) != 141 {
		t.Fatal(len(list.Programs))
	}
	for _, item := range list.Programs {
		stem := filepath.Join("../tests/testdata/reference", item.Fixture)
		var meta struct {
			Count     int   `json:"controller_count"`
			TimeCount int   `json:"afterimage_time_count"`
			States    []int `json:"controller_states"`
		}
		raw, err := os.ReadFile(stem + ".json")
		if err != nil {
			t.Fatal(err)
		}
		if err = json.Unmarshal(raw, &meta); err != nil {
			t.Fatal(err)
		}
		raw, err = os.ReadFile(stem + ".cns")
		if err != nil {
			t.Fatal(err)
		}
		file, err := cns.ParseNamed(bytes.NewReader(raw), stem+".cns")
		if err != nil {
			t.Fatal(err)
		}
		count, duration := 0, 0
		for number, state := range file.States {
			for _, ctrl := range state.Controllers {
				if strings.EqualFold(ctrl.Type, "AfterImage") {
					count++
					if number != 700 {
						t.Fatal(stem, number)
					}
				}
				if strings.EqualFold(ctrl.Type, "AfterImageTime") {
					duration++
					if number != 700 {
						t.Fatal(stem, number)
					}
				}
			}
		}
		if count != meta.Count || duration != meta.TimeCount || len(meta.States) > 1 || len(meta.States) == 1 && meta.States[0] != 700 {
			t.Fatal(stem, count, duration, meta)
		}
	}
}
