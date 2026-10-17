package character

import (
	"bytes"
	"fmt"
	"image/color"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/snd"
)

// LoadOptions supplies explicit roots for shared common-state resources.
// An empty RootDir uses the conventional root/chars/character directory layout.
type LoadOptions struct {
	RootDir  string
	MotifDir string
}

func LoadPlayer(baseDir, defFile string) (*Player, error) {
	return LoadPlayerWithOptions(baseDir, defFile, LoadOptions{})
}

// LoadPlayerWithOptions loads all declared character resources before returning
// a usable player. Missing/malformed resources retain their DEF key and location.
func LoadPlayerWithOptions(baseDir, defFile string, options LoadOptions) (*Player, error) {
	defPath, err := resource.Resolve(defFile, baseDir)
	if err != nil {
		return nil, fmt.Errorf("character DEF: %w", err)
	}
	definition, err := os.ReadFile(defPath)
	if err != nil {
		return nil, err
	}
	def, err := parsers.ParseDef(bytes.NewReader(definition))
	if err != nil {
		return nil, fmt.Errorf("%s: %w", defPath, err)
	}
	baseDir = filepath.Dir(defPath)
	local, err := coord.Parse(def.Info["localcoord"])
	if err != nil {
		return nil, fmt.Errorf("%s [Info] localcoord (line %d): %w", defPath, def.InfoLines["localcoord"], err)
	}
	if options.RootDir == "" {
		options.RootDir = filepath.Dir(filepath.Dir(baseDir))
	}
	context := func(key string, err error) error {
		return fmt.Errorf("%s [Files] %s (line %d): %w", defPath, key, def.FileLines[key], err)
	}
	resolve := func(key string, directories ...string) (string, error) {
		name, declared := def.Files[key]
		if key == "stcommon" && strings.TrimSpace(name) == "" {
			name, declared = "common1.cns", true
		}
		if !declared || strings.TrimSpace(name) == "" {
			return "", context(key, fmt.Errorf("required file is missing or blank"))
		}
		if len(directories) == 0 {
			directories = []string{baseDir}
		}
		path, err := resource.Resolve(name, directories...)
		if err != nil {
			return "", context(key, err)
		}
		return path, nil
	}
	// A per-load cache shares the primary CNS parse with st when both name the
	// same file. No process-global mutable common-state cache is involved.
	parsedCNS := make(map[string]*cns.CNS)
	loadCNS := func(key string, directories ...string) (*cns.CNS, error) {
		path, err := resolve(key, directories...)
		if err != nil {
			return nil, err
		}
		if parsed, found := parsedCNS[path]; found {
			return parsed, nil
		}
		data, err := os.ReadFile(path)
		if err != nil {
			return nil, context(key, err)
		}
		parsed, err := cns.ParseNamed(bytes.NewReader(data), path)
		if err != nil {
			return nil, context(key, err)
		}
		parsedCNS[path] = parsed
		return parsed, nil
	}
	constants, err := loadCNS("cns")
	if err != nil {
		return nil, err
	}
	master := &cns.CNS{Data: constants.Data, Size: constants.Size,
		Velocity: constants.Velocity, Movement: constants.Movement,
		ConstantFields: constants.ConstantFields, States: make(map[int]*cns.StateDef)}
	type stateFile struct {
		key  string
		data *cns.CNS
	}
	var stateFiles []stateFile
	merge := func(parsed *cns.CNS) {
		for id, state := range parsed.States {
			if _, exists := master.States[id]; !exists {
				master.States[id] = state
			}
		}
	}
	// The executable uses first-definition precedence: st, st0..st9, CMD,
	// common. Numbered slots may have gaps; st10 and later are ignored.
	stateKeys := []string{"st"}
	for i := 0; i <= 9; i++ {
		key := fmt.Sprintf("st%d", i)
		if strings.TrimSpace(def.Files[key]) != "" {
			stateKeys = append(stateKeys, key)
		}
	}
	for _, key := range stateKeys {
		parsed, err := loadCNS(key)
		if err != nil {
			return nil, err
		}
		stateFiles = append(stateFiles, stateFile{key, parsed})
		merge(parsed)
	}
	cmdPath, err := resolve("cmd")
	if err != nil {
		return nil, err
	}
	commands, err := os.ReadFile(cmdPath)
	if err != nil {
		return nil, context("cmd", err)
	}
	commandData, err := cmd.Parse(bytes.NewReader(commands))
	if err != nil {
		return nil, context("cmd", fmt.Errorf("%s: %w", cmdPath, err))
	}
	commandStates, err := cns.ParseNamed(bytes.NewReader(commands), cmdPath)
	if err != nil {
		return nil, context("cmd", err)
	}
	stateFiles = append(stateFiles, stateFile{"cmd", commandStates})
	merge(commandStates)
	// A blank/omitted stcommon uses the standard common file. A supplied
	// filename must resolve; silently dropping common states hides broken chars.
	common, err := loadCNS("stcommon", baseDir, options.MotifDir, filepath.Join(options.RootDir, "data"), options.RootDir)
	if err != nil {
		return nil, err
	}
	stateFiles = append(stateFiles, stateFile{"stcommon", common})
	merge(common)
	version := strings.TrimSpace(def.Info["mugenversion"])
	if version == "" {
		version = "legacy"
	}
	master.SetLanguageVersion(version)
	compiler := NewStateMachine()
	for _, file := range stateFiles {
		// Native checks every loaded state file even when a higher-priority
		// file already owns its IDs. Same-file duplicate definitions are ignored.
		file.data.SetLanguageVersion(version)
		ids := make([]int, 0, len(file.data.States))
		for id := range file.data.States {
			ids = append(ids, id)
		}
		sort.Ints(ids)
		for _, id := range ids {
			if err := compiler.CompileState(file.data.States[id]); err != nil {
				return nil, context(file.key, fmt.Errorf("state program: %w", err))
			}
		}
	}
	p := &Player{Dir: baseDir, DefPath: defPath, Definition: def, CMD: commandData,
		LocalCoord: local,
		Commands:   NewCommandBuffer(120), InputSlot: 1}
	spritePath, err := resolve("sprite")
	if err != nil {
		return nil, err
	}
	sprites, err := os.Open(spritePath)
	if err != nil {
		return nil, context("sprite", err)
	}
	p.SFF, err = sff.ParseWithPalette(sprites, legacyPalette(baseDir, defFile))
	sprites.Close()
	if err != nil {
		return nil, context("sprite", fmt.Errorf("%s: %w", spritePath, err))
	}
	animPath, err := resolve("anim")
	if err != nil {
		return nil, err
	}
	animations, err := os.ReadFile(animPath)
	if err != nil {
		return nil, context("anim", err)
	}
	p.AIR, err = air.Parse(bytes.NewReader(animations))
	if err != nil {
		return nil, context("anim", fmt.Errorf("%s: %w", animPath, err))
	}
	if strings.TrimSpace(def.Files["sound"]) != "" {
		soundPath, err := resolve("sound")
		if err != nil {
			return nil, err
		}
		file, err := os.Open(soundPath)
		if err != nil {
			return nil, context("sound", err)
		}
		p.SND, err = snd.Parse(file)
		file.Close()
		if err != nil {
			return nil, context("sound", fmt.Errorf("%s: %w", soundPath, err))
		}
	}
	p.Character = NewCharacter(master, nil, p.CMD)
	p.Character.Name, p.Character.AuthorName = def.Info["name"], def.Info["author"]
	p.Character.SM = compiler
	p.Character.OwnSounds = p.SND
	p.Character.LocalCoord = local
	p.Character.Palettes = palette.New(p.SFF)
	p.Character.Commands, p.Character.AirFile = p.Commands, p.AIR
	return p, nil
}

// Preserve the earlier v1 palette fallback until selected-palette behavior has
// its own reference fixtures. This is not the final DEF palN implementation.
func legacyPalette(baseDir, defFile string) color.Palette {
	name := strings.TrimSuffix(filepath.Base(defFile), filepath.Ext(defFile))
	path := filepath.Join(baseDir, name+".act")
	if _, err := os.Stat(path); err != nil {
		entries, _ := os.ReadDir(baseDir)
		for _, entry := range entries {
			if strings.EqualFold(filepath.Ext(entry.Name()), ".act") {
				path = filepath.Join(baseDir, entry.Name())
				break
			}
		}
	}
	file, err := os.Open(path)
	if err != nil {
		return nil
	}
	defer file.Close()
	palette, _ := sff.ReadPaletteACT(file, true)
	return palette
}
