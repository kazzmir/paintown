package tests

import (
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/character"
)

type resourceCase struct {
	CNSHash      string            `json:"cns_sha256"`
	AIRHash      string            `json:"air_sha256"`
	CMDHash      string            `json:"cmd_sha256"`
	ExtraHashes  map[string]string `json:"extra_sha256"`
	Files        map[string]string `json:"files"`
	Omitted      []string          `json:"omitted_files"`
	StateOrder   []string          `json:"state_file_order"`
	ExpectedFail string            `json:"expected_load_error"`
	Captures     []struct{ Variables map[string]int }
}

func checkedFixture(t *testing.T, path, hash string) []byte {
	t.Helper()
	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatal(err)
	}
	sum := sha256.Sum256(data)
	if hash != hex.EncodeToString(sum[:]) {
		t.Fatalf("authored fixture changed since reference measurement: %s", path)
	}
	return data
}

func writeResource(t *testing.T, path string, data []byte) {
	t.Helper()
	if err := os.MkdirAll(filepath.Dir(path), 0755); err != nil {
		t.Fatal(err)
	}
	if err := os.WriteFile(path, data, 0600); err != nil {
		t.Fatal(err)
	}
}

func resourceDefinition(t *testing.T, dir, fixture string, test resourceCase) string {
	t.Helper()
	stem := filepath.Join("testdata", "reference", fixture)
	stock, err := os.ReadFile(referencePath(t, "chars", "kfm", "kfm.cns"))
	if err != nil {
		t.Fatal(err)
	}
	constants := stock[:strings.Index(strings.ToLower(string(stock)), "[statedef")]
	writeResource(t, filepath.Join(dir, "probe.cns"), append(constants, checkedFixture(t, stem+".cns", test.CNSHash)...))
	writeResource(t, filepath.Join(dir, "probe.air"), checkedFixture(t, stem+".air", test.AIRHash))
	commands := []byte("[Statedef -1]\n[State -1, no input]\ntype=Null\ntrigger1=1\n")
	if test.CMDHash != "" {
		commands = checkedFixture(t, stem+".cmd", test.CMDHash)
	}
	writeResource(t, filepath.Join(dir, "probe.cmd"), commands)
	for name, hash := range test.ExtraHashes {
		writeResource(t, filepath.Join(dir, name), checkedFixture(t, filepath.Join(stem+".extra", name), hash))
	}
	files := map[string]string{"cns": "probe.cns", "st": "probe.cns", "cmd": "probe.cmd",
		"anim": "probe.air", "stcommon": "", "sprite": referencePath(t, "chars", "kfm", "kfm.sff"),
		"sound": referencePath(t, "chars", "kfm", "kfm.snd")}
	for key, value := range test.Files {
		files[key] = value
	}
	for _, key := range test.Omitted {
		delete(files, key)
	}
	keys := make([]string, 0, len(files))
	for key := range files {
		keys = append(keys, key)
	}
	sort.Strings(keys)
	// Retain the measured reversed declaration orders as well as sparse slots.
	keys = append(append([]string{}, test.StateOrder...), keys...)
	seen := make(map[string]bool)
	var definition strings.Builder
	definition.WriteString("[Info]\nname=\"Resource Probe\"\nmugenversion=1.1\n[Files]\n")
	for _, key := range keys {
		if seen[key] {
			continue
		}
		seen[key] = true
		if value, exists := files[key]; exists {
			fmt.Fprintf(&definition, "%s = %s\n", key, value)
		}
	}
	path := filepath.Join(dir, "probe.def")
	writeResource(t, path, []byte(definition.String()))
	return path
}

func TestReferenceCharacterResources(t *testing.T) {
	for _, fixture := range []string{"resource-load", "resource-order", "resource-priority", "resource-common"} {
		data, err := os.ReadFile(filepath.Join("testdata", "reference", fixture+".json"))
		if err != nil {
			t.Fatal(err)
		}
		var catalogue struct{ Cases map[string]resourceCase }
		if err := json.Unmarshal(data, &catalogue); err != nil {
			t.Fatal(err)
		}
		if catalogue.Cases == nil {
			var single resourceCase
			if err := json.Unmarshal(data, &single); err != nil {
				t.Fatal(err)
			}
			catalogue.Cases = map[string]resourceCase{"default": single}
		}
		for name, test := range catalogue.Cases {
			t.Run(fixture+"/"+name, func(t *testing.T) {
				dir := t.TempDir()
				path := resourceDefinition(t, dir, fixture, test)
				p, err := character.LoadPlayerWithOptions(dir, filepath.Base(path), character.LoadOptions{RootDir: referenceRoot(t)})
				if test.ExpectedFail != "" {
					if err == nil || p != nil || !strings.Contains(err.Error(), path+" [Files]") {
						t.Fatalf("reference rejects resource; player=%v error=%v", p, err)
					}
					return
				}
				if err != nil {
					t.Fatal(err)
				}
				soundName, soundDeclared := test.Files["sound"]
				if soundDeclared && soundName == "" {
					if p.SND != nil {
						t.Fatal("blank sound should not load a bank")
					}
				} else if p.SND == nil || len(p.SND.Sounds) != 12 {
					t.Fatal("stock character sound bank was not loaded")
				}
				if fixture == "resource-common" {
					// The native probe waits for the round's initial control grant,
					// then forces 5900 itself. Begin at that same controlled point.
					p.Character.ChangeState(0, 1, -1)
				} else {
					p.Character.ChangeState(5900, -1, -1)
				}
				for tick := 0; tick < 40; tick++ {
					if err := p.Update(); err != nil {
						t.Fatal(err)
					}
				}
				for _, capture := range test.Captures {
					for index, want := range capture.Variables {
						i, err := strconv.Atoi(index)
						if err != nil {
							t.Fatal(err)
						}
						if got := p.Character.GetVar(i); got != want {
							t.Errorf("var(%d)=%d reference=%d", i, got, want)
						}
					}
				}
			})
		}
	}
}

func TestCharacterResourceErrorsKeepContext(t *testing.T) {
	data, err := os.ReadFile(filepath.Join("testdata", "reference", "resource-priority.json"))
	if err != nil {
		t.Fatal(err)
	}
	var baseline resourceCase
	if err := json.Unmarshal(data, &baseline); err != nil {
		t.Fatal(err)
	}
	for _, key := range []string{"cns", "st", "cmd", "stcommon", "sprite", "anim", "sound"} {
		for _, mode := range []string{"missing", "malformed"} {
			t.Run(key+"/"+mode, func(t *testing.T) {
				dir := t.TempDir()
				test := baseline
				test.Files = map[string]string{key: "broken.asset"}
				path := resourceDefinition(t, dir, "resource-priority", test)
				if mode == "malformed" {
					writeResource(t, filepath.Join(dir, "broken.asset"), []byte("[unclosed\n"))
				}
				p, err := character.LoadPlayerWithOptions(dir, filepath.Base(path), character.LoadOptions{RootDir: referenceRoot(t)})
				if p != nil || err == nil {
					t.Fatalf("broken %s %s returned player=%v error=%v", key, mode, p, err)
				}
				for _, want := range []string{path, "[Files] " + key, "line ", "broken.asset"} {
					if !strings.Contains(err.Error(), want) {
						t.Errorf("missing %q in %v", want, err)
					}
				}
			})
		}
	}
}

func TestCharacterResourceNamesAndIndependentLoads(t *testing.T) {
	data, err := os.ReadFile(filepath.Join("testdata", "reference", "resource-priority.json"))
	if err != nil {
		t.Fatal(err)
	}
	var test resourceCase
	if err := json.Unmarshal(data, &test); err != nil {
		t.Fatal(err)
	}
	dir := t.TempDir()
	test.Files = map[string]string{"st": `STATES\first.CNS`, "sound": ""}
	path := resourceDefinition(t, dir, "resource-priority", test)
	common := filepath.Join(dir, "motif", "Common.CNS")
	writeResource(t, common, []byte("[Statedef 25]\n[State 25, first]\ntype=Null\ntrigger1=1\n"))
	writeResource(t, filepath.Join(dir, "States", "First.cns"), []byte("[Statedef 0]\n[State 0, first]\ntype=Null\ntrigger1=1\n"))
	definition, err := os.ReadFile(path)
	if err != nil {
		t.Fatal(err)
	}
	definition = []byte(strings.Replace(string(definition), "stcommon = \n", "stcommon = common.cns\n", 1))
	writeResource(t, path, definition)
	options := character.LoadOptions{RootDir: referenceRoot(t), MotifDir: filepath.Dir(common)}
	first, err := character.LoadPlayerWithOptions(dir, "PROBE.DEF", options)
	if err != nil {
		t.Fatal(err)
	}
	if first.SND != nil || first.Definition.Info["name"] != "Resource Probe" {
		t.Fatal("blank optional sound or DEF metadata was lost")
	}
	writeResource(t, common, []byte("[Statedef 26]\n[State 26, replacement]\ntype=Null\ntrigger1=1\n"))
	second, err := character.LoadPlayerWithOptions(dir, "probe.def", options)
	if err != nil {
		t.Fatal(err)
	}
	if first.Character.StateFile.States[25] == nil || first.Character.StateFile.States[26] != nil || second.Character.StateFile.States[25] != nil || second.Character.StateFile.States[26] == nil {
		t.Fatal("common definitions were stale or shared between independent loads")
	}
}
