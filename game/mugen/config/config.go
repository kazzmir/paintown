package config

import (
	"os"
	"path/filepath"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type MugenConfig struct {
	Options struct {
		Difficulty int
		Life       int
		Time       int
		GameSpeed  int
		WavVolume  int
		MidiVolume int
		Motif      string
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
	}
	Video struct {
		Width      int
		Height     int
		Depth      int
		Stretch    bool
		DoubleRes  int
		VRetrace   bool
		FullScreen bool
	}
}

func LoadConfig(dataDir string) (*MugenConfig, error) {
	cfgPath := filepath.Join(dataDir, "data", "mugen.cfg")
	f, err := os.Open(cfgPath)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	ast, err := parsers.Parse(f)
	if err != nil {
		return nil, err
	}

	cfg := &MugenConfig{}
	// Set some defaults
	cfg.Config.GameSpeed = 60
	cfg.Config.DrawShadows = true

	for _, sec := range ast.Sections {
		switch strings.ToLower(sec.Name) {
		case "options":
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
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
				case "motif":
					cfg.Options.Motif = getString(attr.Value)
				}
			}
		case "rules":
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
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
		case "config":
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
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
				}
			}
		case "video linux", "video system", "video": // Support different video sections
			for _, attr := range sec.Attributes {
				switch strings.ToLower(attr.ID.String()) {
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
				}
			}
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
