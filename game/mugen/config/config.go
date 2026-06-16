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
		"mugen.yaml",
		"game/mugen/mugen.yaml",
	}

	// Also search relative to the executable's location
	if exePath, err := os.Executable(); err == nil {
		exeDir := filepath.Dir(exePath)
		extPaths = append(extPaths, filepath.Join(exeDir, "mugen.yaml"))
	}

	loaded := false
	for _, path := range extPaths {
		if path == "" {
			continue
		}
		if ext, err := LoadExtendedConfig(path); err == nil {
			fmt.Printf("Merged extended config from: %s\n", path)
			mergeExtendedConfig(cfg, ext)
			loaded = true
			break
		}
	}

	if !loaded {
		fmt.Printf("WARNING: No mugen.yaml found. Searched paths:\n")
		for _, path := range extPaths {
			fmt.Printf("  - %s\n", path)
		}
		fmt.Printf("Keyboard bindings will use hardcoded defaults.\n")
	}

	fmt.Printf("P1 Keyboard: Jump=%v Crouch=%v Left=%v Right=%v A=%v B=%v C=%v X=%v Y=%v Z=%v Start=%v\n",
		cfg.Input.Player1Keyboard.Jump, cfg.Input.Player1Keyboard.Crouch,
		cfg.Input.Player1Keyboard.Left, cfg.Input.Player1Keyboard.Right,
		cfg.Input.Player1Keyboard.A, cfg.Input.Player1Keyboard.B, cfg.Input.Player1Keyboard.C,
		cfg.Input.Player1Keyboard.X, cfg.Input.Player1Keyboard.Y, cfg.Input.Player1Keyboard.Z,
		cfg.Input.Player1Keyboard.Start)
	fmt.Printf("P2 Keyboard: Jump=%v Crouch=%v Left=%v Right=%v A=%v B=%v C=%v X=%v Y=%v Z=%v Start=%v\n",
		cfg.Input.Player2Keyboard.Jump, cfg.Input.Player2Keyboard.Crouch,
		cfg.Input.Player2Keyboard.Left, cfg.Input.Player2Keyboard.Right,
		cfg.Input.Player2Keyboard.A, cfg.Input.Player2Keyboard.B, cfg.Input.Player2Keyboard.C,
		cfg.Input.Player2Keyboard.X, cfg.Input.Player2Keyboard.Y, cfg.Input.Player2Keyboard.Z,
		cfg.Input.Player2Keyboard.Start)

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

// dosScancodeToKey converts legacy DOS/DirectInput keyboard scancodes
// (as used in original MUGEN mugen.cfg files) to ebiten key constants.
func dosScancodeToKey(code int) ebiten.Key {
	switch code {
	case 1:
		return ebiten.KeyEscape
	case 2:
		return ebiten.KeyDigit1
	case 3:
		return ebiten.KeyDigit2
	case 4:
		return ebiten.KeyDigit3
	case 5:
		return ebiten.KeyDigit4
	case 6:
		return ebiten.KeyDigit5
	case 7:
		return ebiten.KeyDigit6
	case 8:
		return ebiten.KeyDigit7
	case 9:
		return ebiten.KeyDigit8
	case 10:
		return ebiten.KeyDigit9
	case 11:
		return ebiten.KeyDigit0
	case 12:
		return ebiten.KeyMinus
	case 13:
		return ebiten.KeyEqual
	case 14:
		return ebiten.KeyBackspace
	case 15:
		return ebiten.KeyTab
	case 16:
		return ebiten.KeyQ
	case 17:
		return ebiten.KeyW
	case 18:
		return ebiten.KeyE
	case 19:
		return ebiten.KeyR
	case 20:
		return ebiten.KeyT
	case 21:
		return ebiten.KeyY
	case 22:
		return ebiten.KeyU
	case 23:
		return ebiten.KeyI
	case 24:
		return ebiten.KeyO
	case 25:
		return ebiten.KeyP
	case 26:
		return ebiten.KeyBracketLeft
	case 27:
		return ebiten.KeyBracketRight
	case 28:
		return ebiten.KeyEnter
	case 29:
		return ebiten.KeyControlLeft
	case 30:
		return ebiten.KeyA
	case 31:
		return ebiten.KeyS
	case 32:
		return ebiten.KeyD
	case 33:
		return ebiten.KeyF
	case 34:
		return ebiten.KeyG
	case 35:
		return ebiten.KeyH
	case 36:
		return ebiten.KeyJ
	case 37:
		return ebiten.KeyK
	case 38:
		return ebiten.KeyL
	case 39:
		return ebiten.KeySemicolon
	case 40:
		return ebiten.KeyQuote
	case 41:
		return ebiten.KeyBackquote
	case 42:
		return ebiten.KeyShiftLeft
	case 43:
		return ebiten.KeyBackslash
	case 44:
		return ebiten.KeyZ
	case 45:
		return ebiten.KeyX
	case 46:
		return ebiten.KeyC
	case 47:
		return ebiten.KeyV
	case 48:
		return ebiten.KeyB
	case 49:
		return ebiten.KeyN
	case 50:
		return ebiten.KeyM
	case 51:
		return ebiten.KeyComma
	case 52:
		return ebiten.KeyPeriod
	case 53:
		return ebiten.KeySlash
	case 54:
		return ebiten.KeyShiftRight
	case 55:
		return ebiten.KeyNumpadMultiply
	case 56:
		return ebiten.KeyAltLeft
	case 57:
		return ebiten.KeySpace
	case 58:
		return ebiten.KeyCapsLock
	case 59:
		return ebiten.KeyF1
	case 60:
		return ebiten.KeyF2
	case 61:
		return ebiten.KeyF3
	case 62:
		return ebiten.KeyF4
	case 63:
		return ebiten.KeyF5
	case 64:
		return ebiten.KeyF6
	case 65:
		return ebiten.KeyF7
	case 66:
		return ebiten.KeyF8
	case 67:
		return ebiten.KeyF9
	case 68:
		return ebiten.KeyF10
	case 69:
		return ebiten.KeyNumLock
	case 70:
		return ebiten.KeyScrollLock
	case 71:
		return ebiten.KeyNumpad7
	case 72:
		return ebiten.KeyNumpad8
	case 73:
		return ebiten.KeyNumpad9
	case 74:
		return ebiten.KeyNumpadSubtract
	case 75:
		return ebiten.KeyNumpad4
	case 76:
		return ebiten.KeyNumpad5
	case 77:
		return ebiten.KeyNumpad6
	case 78:
		return ebiten.KeyNumpadAdd
	case 79:
		return ebiten.KeyNumpad1
	case 80:
		return ebiten.KeyNumpad2
	case 81:
		return ebiten.KeyNumpad3
	case 82:
		return ebiten.KeyNumpad0
	case 83:
		return ebiten.KeyNumpadDecimal
	case 87:
		return ebiten.KeyF11
	case 88:
		return ebiten.KeyF12
	case 156:
		return ebiten.KeyNumpadEnter
	case 157:
		return ebiten.KeyControlRight
	case 181:
		return ebiten.KeyNumpadDivide
	case 184:
		return ebiten.KeyAltRight
	case 199:
		return ebiten.KeyHome
	case 200:
		return ebiten.KeyArrowUp
	case 201:
		return ebiten.KeyPageUp
	case 203:
		return ebiten.KeyArrowLeft
	case 205:
		return ebiten.KeyArrowRight
	case 207:
		return ebiten.KeyEnd
	case 208:
		return ebiten.KeyArrowDown
	case 209:
		return ebiten.KeyPageDown
	case 210:
		return ebiten.KeyInsert
	case 211:
		return ebiten.KeyDelete
	}
	return 0
}

func parseKeys(sec *parsers.Section, keys *Keys) {
	for _, attr := range sec.Attributes {
		keyName := strings.ToLower(attr.ID.String())
		var val ebiten.Key

		if n, ok := attr.Value.(parsers.NumberValue); ok {
			val = dosScancodeToKey(int(n.Val))
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
	case "tab", "keytab":
		return ebiten.KeyTab
	case "semicolon", "keysemicolon":
		return ebiten.KeySemicolon
	case "comma", "keycomma":
		return ebiten.KeyComma
	case "period", "keyperiod":
		return ebiten.KeyPeriod
	case "slash", "keyslash":
		return ebiten.KeySlash
	case "quote", "keyquote":
		return ebiten.KeyQuote
	case "minus", "keyminus":
		return ebiten.KeyMinus
	case "equal", "keyequal":
		return ebiten.KeyEqual
	case "bracketleft", "keybracketleft":
		return ebiten.KeyBracketLeft
	case "bracketright", "keybracketright":
		return ebiten.KeyBracketRight
	case "backslash", "keybackslash":
		return ebiten.KeyBackslash
	case "backquote", "keybackquote":
		return ebiten.KeyBackquote
	case "shiftleft", "keyshiftleft":
		return ebiten.KeyShiftLeft
	case "shiftright", "keyshiftright":
		return ebiten.KeyShiftRight
	case "controlleft", "keycontrolleft":
		return ebiten.KeyControlLeft
	case "controlright", "keycontrolright":
		return ebiten.KeyControlRight
	case "altleft", "keyaltleft":
		return ebiten.KeyAltLeft
	case "altright", "keyaltright":
		return ebiten.KeyAltRight
	case "capslock", "keycapslock":
		return ebiten.KeyCapsLock
	case "delete", "keydelete":
		return ebiten.KeyDelete
	case "insert", "keyinsert":
		return ebiten.KeyInsert
	case "home", "keyhome":
		return ebiten.KeyHome
	case "end", "keyend":
		return ebiten.KeyEnd
	case "pageup", "keypageup":
		return ebiten.KeyPageUp
	case "pagedown", "keypagedown":
		return ebiten.KeyPageDown
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
