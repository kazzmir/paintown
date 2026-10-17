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
	configured uint16
	Jump       ebiten.Key
	Crouch     ebiten.Key
	Left       ebiten.Key
	Right      ebiten.Key
	A          ebiten.Key
	B          ebiten.Key
	C          ebiten.Key
	X          ebiten.Key
	Y          ebiten.Key
	Z          ebiten.Key
	Start      ebiten.Key
	Escape     ebiten.Key
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
		GameWidth, GameHeight int
		GameSpeed             int
		DrawShadows           bool
		AfterImageMax         int
		LayeredSpriteMax      int
		ExplodMax             int
		SysExplodMax          int
		HelperMax             int
		PlayerProjectileMax   int
		FirstRun              bool
	}
	Debug struct {
		Debug          bool
		AllowDebugMode bool
		AllowDebugKeys bool
		Speedup        bool
		StartStage     string
	}
	Video struct {
		Width                           int
		Height                          int
		Depth                           int
		Stretch                         bool
		DoubleRes                       int
		VRetrace                        bool
		FullScreen                      bool
		BlitMode                        string
		RenderMode                      string
		StageFit, SystemFit, KeepAspect bool
	}
	Sound  SoundConfig
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
	base := dataDir
	if filepath.Base(base) == "data" {
		base = filepath.Dir(base)
	}
	homeDir, _ := os.UserHomeDir()
	paths := []string{
		filepath.Join(base, "mugen.yaml"),
		filepath.Join(base, "data", "mugen.yaml"),
		filepath.Join(homeDir, ".paintown", "mugen.yaml"),
		"mugen.yaml",
		"game/mugen/mugen.yaml",
	}
	if exePath, err := os.Executable(); err == nil {
		paths = append(paths, filepath.Join(filepath.Dir(exePath), "mugen.yaml"))
	}
	return LoadConfigWithOverrides(dataDir, paths...)
}

// LoadConfigWithOverrides loads the legacy configuration and the first readable
// override in paths. An empty list disables ambient user/executable overrides,
// allowing tools and tests to use reproducible configuration.
func LoadConfigWithOverrides(dataDir string, paths ...string) (*MugenConfig, error) {
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
	cfg.Sound = DefaultSoundConfig()
	cfg.Config.GameWidth, cfg.Config.GameHeight = 320, 240
	cfg.Config.HelperMax = 56
	cfg.Video.StageFit, cfg.Video.SystemFit, cfg.Video.KeepAspect = true, true, true
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
					cfg.Sound.WavVolume = cfg.Options.WavVolume
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
				case "gamewidth", "gameheight":
					value := getFloat(attr.Value)
					if value <= 0 || value != float64(int(value)) {
						return nil, fmt.Errorf("%s [Config] %s (line %d): dimension must be a positive integer", cfgPath, key, attr.Line)
					}
					if key == "gamewidth" {
						cfg.Config.GameWidth = int(value)
					} else {
						cfg.Config.GameHeight = int(value)
					}
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
				case "rendermode":
					cfg.Video.RenderMode = getString(attr.Value)
				case "stagefit":
					cfg.Video.StageFit = getBool(attr.Value)
				case "systemfit":
					cfg.Video.SystemFit = getBool(attr.Value)
				case "keepaspect":
					cfg.Video.KeepAspect = getBool(attr.Value)
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
				case "samplerate":
					cfg.Sound.SampleRate = int(getFloat(attr.Value))
				case "buffersize":
					cfg.Sound.BufferSize = int(getFloat(attr.Value))
				case "wavchannels":
					cfg.Sound.WavChannels = int(getFloat(attr.Value))
				case "mastervolume":
					cfg.Sound.MasterVolume = int(getFloat(attr.Value))
				case "wavvolume":
					cfg.Sound.WavVolume = int(getFloat(attr.Value))
				case "bgmvolume":
					cfg.Sound.BGMVolume = int(getFloat(attr.Value))
				case "sfxresamplemethod":
					cfg.Sound.SFXResampleMethod = getString(attr.Value)
				case "sfxresamplequality":
					cfg.Sound.SFXResampleQuality = int(getFloat(attr.Value))
				case "bgmresamplequality":
					cfg.Sound.BGMResampleQuality = int(getFloat(attr.Value))
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
			parseKeys(sec, &cfg.Input.Player1Keyboard, true)
		case name == "p2 keys":
			parseKeys(sec, &cfg.Input.Player2Keyboard, true)
		case name == "p1 joystick":
			parseKeys(sec, &cfg.Input.Player1Joystick, false)
		case name == "p2 joystick":
			parseKeys(sec, &cfg.Input.Player2Joystick, false)
		}
	}

	loaded := false
	for _, path := range paths {
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

	if !loaded && len(paths) > 0 {
		fmt.Printf("No extended configuration found; using mugen.cfg settings.\n")
	}
	if err := cfg.Sound.Validate(); err != nil {
		return nil, fmt.Errorf("%s [Sound]: %w", cfgPath, err)
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

// Keyboard numeric values use SDL symbols; joystick codes remain raw device
// numbers until the gamepad adapter interprets them.
func parseKeys(sec *parsers.Section, keys *Keys, keyboard bool) {
	for _, attr := range sec.Attributes {
		action := strings.ToLower(attr.ID.String())
		value := NoKey
		if number, ok := attr.Value.(parsers.NumberValue); ok {
			if keyboard {
				value = sdlKeyToKey(int(number.Val))
				if value == NoKey && number.Val != 0 {
					fmt.Printf("mugen.cfg [%s] %s (line %d): unsupported SDL key number %v\n", sec.Name, action, attr.Line, number.Val)
				}
			} else {
				value = ebiten.Key(int(number.Val))
			}
		} else {
			var valid bool
			value, valid = stringToKey(getString(attr.Value))
			if !valid {
				fmt.Printf("mugen.cfg [%s] %s (line %d): unknown key name %q\n", sec.Name, action, attr.Line, getString(attr.Value))
			}
		}
		keys.SetBinding(action, value)
	}
}

func mergeExtendedConfig(cfg *MugenConfig, ext *ExtendedConfig) {
	if ext.Sound.Enabled != nil {
		cfg.Sound.Sound = *ext.Sound.Enabled
	}
	for _, volume := range []struct{ target, source *int }{{&cfg.Sound.MasterVolume, ext.Sound.MasterVolume}, {&cfg.Sound.WavVolume, ext.Sound.WavVolume}, {&cfg.Sound.BGMVolume, ext.Sound.BGMVolume}} {
		if volume.source != nil {
			*volume.target = *volume.source
		}
	}
	// Merge Video
	if ext.Video.Width > 0 {
		cfg.Video.Width = ext.Video.Width
	}
	if ext.Video.Height > 0 {
		cfg.Video.Height = ext.Video.Height
	}
	for _, flag := range []struct{ target, source *bool }{
		{&cfg.Video.FullScreen, ext.Video.FullScreen}, {&cfg.Video.VRetrace, ext.Video.VSync},
		{&cfg.Video.StageFit, ext.Video.StageFit}, {&cfg.Video.SystemFit, ext.Video.SystemFit}, {&cfg.Video.KeepAspect, ext.Video.KeepAspect},
	} {
		if flag.source != nil {
			*flag.target = *flag.source
		}
	}

	// Merge Keys
	applyKeys := func(target *Keys, source map[string]string) {
		for action, name := range source {
			key, valid := stringToKey(name)
			if !valid {
				fmt.Printf("YAML keyboard %s: unknown key name %q\n", action, name)
				continue
			}
			if !target.SetBinding(action, key) {
				fmt.Printf("YAML keyboard: unknown action %q\n", action)
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
