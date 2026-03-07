package config

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type Keys struct {
	Jump   ebiten.Key
	Crouch ebiten.Key
	Left   ebiten.Key
	Right  ebiten.Key
	A      ebiten.Key
	B      ebiten.Key
	C      ebiten.Key
	X      ebiten.Key
	Y      ebiten.Key
	Z      ebiten.Key
	Start  ebiten.Key
	Escape ebiten.Key
}

type AIRamp struct {
	Start []int
	End   []int
}

type MugenConfig struct {
	Options struct {
		Difficulty   int
		Life         int
		Time         int
		GameSpeed    int
		WavVolume    int
		MidiVolume   int
		Team1Vs2Life int
		TeamLoseOnKO bool
		Motif        string
	}
	Rules struct {
		GameType                    string
		DefaultAttackLifeToPowerMul float64
		DefaultGetHitLifeToPowerMul float64
		SuperTargetDefenceMul       float64
	}
	Config struct {
		GameSpeed           int
		DrawShadows         bool
		AfterImageMax       int
		LayeredSpriteMax    int
		ExplodMax           int
		SysExplodMax        int
		HelperMax           int
		PlayerProjectileMax int
		FirstRun            bool
	}
	Debug struct {
		Debug          bool
		AllowDebugMode bool
		AllowDebugKeys bool
		Speedup        bool
		StartStage     string
	}
	Video struct {
		Width      int
		Height     int
		Depth      int
		Stretch    bool
		DoubleRes  int
		VRetrace   bool
		FullScreen bool
		BlitMode   string
	}
	Sound struct {
		Sound         bool
		StereoEffects bool
		PanningWidth  int
		ReverseStereo bool
	}
	Arcade struct {
		AIRandomColor bool
		AICheat       bool
		ArcadeRamp    AIRamp
		TeamRamp      AIRamp
		SurvivalRamp  AIRamp
	}
	Input struct {
		Player1Keyboard Keys
		Player2Keyboard Keys
		Player1Joystick Keys
		Player2Joystick Keys
	}
}

func LoadConfig(dataDir string) (*MugenConfig, error) {
	// Robustness: check if dataDir already ends in /data or is the data folder
	base := dataDir
	if filepath.Base(dataDir) == "data" {
		base = filepath.Dir(dataDir)
	}

	cfgPath := filepath.Join(base, "data", "mugen.cfg")
	f, err := os.Open(cfgPath)
	if err != nil {
		return nil, fmt.Errorf("failed to open mugen.cfg at %s: %w", cfgPath, err)
	}
	defer f.Close()

	fmt.Printf("Loading MUGEN config from: %s\n", cfgPath)

	ast, err := parsers.Parse(f)
	if err != nil {
		return nil, fmt.Errorf("failed to parse mugen.cfg: %w", err)
	}

	cfg := &MugenConfig{}
	// Set some defaults
	cfg.Config.GameSpeed = 60
	cfg.Config.DrawShadows = true

	for _, sec := range ast.Sections {
		name := strings.ToLower(strings.TrimSpace(sec.Name))
		switch {
		case name == "options":
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				switch key {
				case "difficulty":
					cfg.Options.Difficulty = int(getFloat(attr.Value))
				case "life":
					cfg.Options.Life = int(getFloat(attr.Value))
				case "time":
					cfg.Options.Time = int(getFloat(attr.Value))
				case "gamespeed":
					cfg.Options.GameSpeed = int(getFloat(attr.Value))
				case "wavvolume":
					cfg.Options.WavVolume = int(getFloat(attr.Value))
				case "midivolume":
					cfg.Options.MidiVolume = int(getFloat(attr.Value))
				case "team.1vs2life":
					cfg.Options.Team1Vs2Life = int(getFloat(attr.Value))
				case "team.loseonko":
					cfg.Options.TeamLoseOnKO = getBool(attr.Value)
				case "motif":
					cfg.Options.Motif = getString(attr.Value)
				}
			}
		case name == "rules":
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				switch key {
				case "gametype":
					cfg.Rules.GameType = getString(attr.Value)
				case "default.attack.lifetopowermul":
					cfg.Rules.DefaultAttackLifeToPowerMul = getFloat(attr.Value)
				case "default.gethit.lifetopowermul":
					cfg.Rules.DefaultGetHitLifeToPowerMul = getFloat(attr.Value)
				case "super.targetdefencemul":
					cfg.Rules.SuperTargetDefenceMul = getFloat(attr.Value)
				}
			}
		case name == "config":
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				switch key {
				case "gamespeed":
					cfg.Config.GameSpeed = int(getFloat(attr.Value))
				case "drawshadows":
					cfg.Config.DrawShadows = getBool(attr.Value)
				case "afterimagemax":
					cfg.Config.AfterImageMax = int(getFloat(attr.Value))
				case "layeredspritemax":
					cfg.Config.LayeredSpriteMax = int(getFloat(attr.Value))
				case "explodmax":
					cfg.Config.ExplodMax = int(getFloat(attr.Value))
				case "sysexplodmax":
					cfg.Config.SysExplodMax = int(getFloat(attr.Value))
				case "helpermax":
					cfg.Config.HelperMax = int(getFloat(attr.Value))
				case "playerprojectilemax":
					cfg.Config.PlayerProjectileMax = int(getFloat(attr.Value))
				case "firstrun":
					cfg.Config.FirstRun = getBool(attr.Value)
				}
			}
		case name == "debug":
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				switch key {
				case "debug":
					cfg.Debug.Debug = getBool(attr.Value)
				case "allowdebugmode":
					cfg.Debug.AllowDebugMode = getBool(attr.Value)
				case "allowdebugkeys":
					cfg.Debug.AllowDebugKeys = getBool(attr.Value)
				case "speedup":
					cfg.Debug.Speedup = getBool(attr.Value)
				case "startstage":
					cfg.Debug.StartStage = getString(attr.Value)
				}
			}
		case strings.HasPrefix(name, "video"):
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				switch key {
				case "width":
					cfg.Video.Width = int(getFloat(attr.Value))
				case "height":
					cfg.Video.Height = int(getFloat(attr.Value))
				case "depth":
					cfg.Video.Depth = int(getFloat(attr.Value))
				case "stretch":
					cfg.Video.Stretch = getBool(attr.Value)
				case "doubleres":
					cfg.Video.DoubleRes = int(getFloat(attr.Value))
				case "vretrace":
					cfg.Video.VRetrace = getBool(attr.Value)
				case "fullscreen":
					cfg.Video.FullScreen = getBool(attr.Value)
				case "blitmode":
					cfg.Video.BlitMode = getString(attr.Value)
				}
			}
		case strings.HasPrefix(name, "sound"):
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				switch key {
				case "sound":
					cfg.Sound.Sound = getBool(attr.Value)
				case "stereoeffects":
					cfg.Sound.StereoEffects = getBool(attr.Value)
				case "panningwidth":
					cfg.Sound.PanningWidth = int(getFloat(attr.Value))
				case "reversestereo":
					cfg.Sound.ReverseStereo = getBool(attr.Value)
				}
			}
		case name == "arcade":
			for _, attr := range sec.Attributes {
				key := strings.ToLower(attr.ID.String())
				switch key {
				case "ai.randomcolor":
					cfg.Arcade.AIRandomColor = getBool(attr.Value)
				case "ai.cheat":
					cfg.Arcade.AICheat = getBool(attr.Value)
				case "arcade.airamp.start":
					cfg.Arcade.ArcadeRamp.Start = getIntArray(attr.Value)
				case "arcade.airamp.end":
					cfg.Arcade.ArcadeRamp.End = getIntArray(attr.Value)
				case "team.airamp.start":
					cfg.Arcade.TeamRamp.Start = getIntArray(attr.Value)
				case "team.airamp.end":
					cfg.Arcade.TeamRamp.End = getIntArray(attr.Value)
				case "survival.airamp.start":
					cfg.Arcade.SurvivalRamp.Start = getIntArray(attr.Value)
				case "survival.airamp.end":
					cfg.Arcade.SurvivalRamp.End = getIntArray(attr.Value)
				}
			}
		case name == "p1 keys":
			parseKeys(sec, &cfg.Input.Player1Keyboard)
		case name == "p2 keys":
			parseKeys(sec, &cfg.Input.Player2Keyboard)
		case name == "p1 joystick":
			parseKeys(sec, &cfg.Input.Player1Joystick)
		case name == "p2 joystick":
			parseKeys(sec, &cfg.Input.Player2Joystick)
		}
	}

	// 2. Load Extended Config (YAML) from multiple locations
	homeDir, _ := os.UserHomeDir()
	extPaths := []string{
		filepath.Join(base, "mugen.yaml"),
		filepath.Join(base, "data", "mugen.yaml"),
		filepath.Join(homeDir, ".paintown", "mugen.yaml"),
		"game/mugen/mugen.yaml",
	}

	for _, path := range extPaths {
		if path == "" {
			continue
		}
		if ext, err := LoadExtendedConfig(path); err == nil {
			fmt.Printf("Merged extended config from: %s\n", path)
			mergeExtendedConfig(cfg, ext)
		}
	}

	return cfg, nil
}

func getFloat(v parsers.Value) float64 {
	if n, ok := v.(parsers.NumberValue); ok {
		return n.Val
	}
	return 0
}

func getString(v parsers.Value) string {
	if v == nil {
		return ""
	}
	if s, ok := v.(parsers.StringValue); ok {
		return s.Val
	}
	if k, ok := v.(parsers.KeywordValue); ok {
		return k.Val
	}
	return ""
}

func getBool(v parsers.Value) bool {
	return getFloat(v) != 0
}

func parseKeys(sec *parsers.Section, keys *Keys) {
	for _, attr := range sec.Attributes {
		keyName := strings.ToLower(attr.ID.String())
		var val ebiten.Key

		if n, ok := attr.Value.(parsers.NumberValue); ok {
			val = ebiten.Key(n.Val)
		} else {
			str := strings.ToLower(getString(attr.Value))
			val = stringToKey(str)
		}

		switch keyName {
		case "jump":
			keys.Jump = val
		case "crouch":
			keys.Crouch = val
		case "left":
			keys.Left = val
		case "right":
			keys.Right = val
		case "a":
			keys.A = val
		case "b":
			keys.B = val
		case "c":
			keys.C = val
		case "x":
			keys.X = val
		case "y":
			keys.Y = val
		case "z":
			keys.Z = val
		case "start":
			keys.Start = val
		case "escape":
			keys.Escape = val
		}
	}
}

func stringToKey(s string) ebiten.Key {
	// Basic mapping for common keys.
	// We can expand this or use a more comprehensive map.
	switch strings.ToLower(s) {
	case "up", "keyarrowup":
		return ebiten.KeyUp
	case "down", "keyarrowdown":
		return ebiten.KeyDown
	case "left", "keyarrowleft":
		return ebiten.KeyLeft
	case "right", "keyarrowright":
		return ebiten.KeyRight
	case "enter", "keyenter":
		return ebiten.KeyEnter
	case "space", "keyspace":
		return ebiten.KeySpace
	case "escape", "keyescape":
		return ebiten.KeyEscape
	case "a", "keya":
		return ebiten.KeyA
	case "b", "keyb":
		return ebiten.KeyB
	case "c", "keyc":
		return ebiten.KeyC
	case "d", "keyd":
		return ebiten.KeyD
	case "e", "keye":
		return ebiten.KeyE
	case "f", "keyf":
		return ebiten.KeyF
	case "g", "keyg":
		return ebiten.KeyG
	case "h", "keyh":
		return ebiten.KeyH
	case "i", "keyi":
		return ebiten.KeyI
	case "j", "keyj":
		return ebiten.KeyJ
	case "k", "keyk":
		return ebiten.KeyK
	case "l", "keyl":
		return ebiten.KeyL
	case "m", "keym":
		return ebiten.KeyM
	case "n", "keyn":
		return ebiten.KeyN
	case "o", "keyo":
		return ebiten.KeyO
	case "p", "keyp":
		return ebiten.KeyP
	case "q", "keyq":
		return ebiten.KeyQ
	case "r", "keyr":
		return ebiten.KeyR
	case "s", "keys":
		return ebiten.KeyS
	case "t", "keyt":
		return ebiten.KeyT
	case "u", "keyu":
		return ebiten.KeyU
	case "v", "keyv":
		return ebiten.KeyV
	case "w", "keyw":
		return ebiten.KeyW
	case "x", "keyx":
		return ebiten.KeyX
	case "y", "keyy":
		return ebiten.KeyY
	case "z", "keyz":
		return ebiten.KeyZ
	case "backspace", "keybackspace":
		return ebiten.KeyBackspace
	}
	return 0
}

func mergeExtendedConfig(cfg *MugenConfig, ext *ExtendedConfig) {
	// Merge Video
	if ext.Video.Width > 0 {
		cfg.Video.Width = ext.Video.Width
	}
	if ext.Video.Height > 0 {
		cfg.Video.Height = ext.Video.Height
	}
	cfg.Video.FullScreen = ext.Video.FullScreen
	cfg.Video.VRetrace = ext.Video.VSync

	// Merge Keys
	applyKeys := func(target *Keys, source map[string]string) {
		for k, v := range source {
			val := stringToKey(v)
			if val == 0 {
				continue
			}
			switch strings.ToLower(k) {
			case "jump", "up":
				target.Jump = val
			case "crouch", "down":
				target.Crouch = val
			case "left":
				target.Left = val
			case "right":
				target.Right = val
			case "a":
				target.A = val
			case "b":
				target.B = val
			case "c":
				target.C = val
			case "x":
				target.X = val
			case "y":
				target.Y = val
			case "z":
				target.Z = val
			case "start":
				target.Start = val
			case "escape":
				target.Escape = val
			}
		}
	}

	applyKeys(&cfg.Input.Player1Keyboard, ext.Input.P1.Keyboard)
	applyKeys(&cfg.Input.Player2Keyboard, ext.Input.P2.Keyboard)

	// Future: handle Joystick merging
	// applyKeys(&cfg.Input.Player1Joystick, ext.Input.P1.Joy)
	// applyKeys(&cfg.Input.Player2Joystick, ext.Input.P2.Joy)
}

func getIntArray(v parsers.Value) []int {
	if list, ok := v.(*parsers.ValueList); ok {
		var out []int
		for _, item := range list.Values {
			out = append(out, int(getFloat(item)))
		}
		return out
	}
	return []int{int(getFloat(v))}
}
