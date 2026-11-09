package storyboard

import (
	"bytes"
	"fmt"
	"image"
	"image/color"
	"os"
	"path/filepath"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/background"
	"github.com/kazzmir/paintown/game/mugen/font"
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/resource"
	"github.com/kazzmir/paintown/game/mugen/sff"
	"github.com/kazzmir/paintown/game/mugen/snd"
)

// Load builds CPU storyboard resources without starting playback or uploading
// images. Paths within the DEF are relative to that file, never an ambient CWD.
func Load(path, root string, languages ...string) (*Storyboard, error) {
	source, err := resource.Resolve(path, root, filepath.Join(root, "data"), ".")
	if err != nil {
		return nil, fmt.Errorf("storyboard DEF: %w", err)
	}
	data, err := os.ReadFile(source)
	if err != nil {
		return nil, err
	}
	ast, err := parsers.Parse(bytes.NewReader(data))
	if err != nil {
		return nil, fmt.Errorf("%s: %w", source, err)
	}
	ast = parsers.Localize(ast, languages...)
	definition, err := parsers.ParseStoryboardFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", source, err)
	}
	if len(definition.Scenes) == 0 || definition.StartScene < 0 || definition.StartScene >= len(definition.Scenes) {
		return nil, fmt.Errorf("%s: startscene %d invalid for %d scenes", source, definition.StartScene, len(definition.Scenes))
	}
	dir := filepath.Dir(source)
	context := func(key string, line int, err error) error {
		return fmt.Errorf("%s [SceneDef] %s (line %d): %w", source, key, line, err)
	}
	story := &Storyboard{Source: source, LocalCoord: definition.LocalCoord, StartScene: definition.StartScene, RawSprites: make(map[string]*sff.Sprite)}
	spritePath, err := resource.Resolve(definition.SprFile, dir)
	if err != nil {
		return nil, context("spr", definition.SprLine, err)
	}
	file, err := os.Open(spritePath)
	if err != nil {
		return nil, context("spr", definition.SprLine, err)
	}
	story.SpriteData, err = sff.Parse(file)
	file.Close()
	if err != nil {
		return nil, context("spr", definition.SprLine, fmt.Errorf("%s: %w", spritePath, err))
	}
	for i := range story.SpriteData.Sprites {
		sprite := &story.SpriteData.Sprites[i]
		story.Sprites = append(story.Sprites, sprite)
		story.RawSprites[fmt.Sprintf("%d-%d", sprite.GroupNumber, sprite.ImageNumber)] = sprite
	}
	if definition.SndFile != "" {
		soundPath, err := resource.Resolve(definition.SndFile, dir)
		if err != nil {
			return nil, context("snd", definition.SndLine, err)
		}
		file, err := os.Open(soundPath)
		if err != nil {
			return nil, context("snd", definition.SndLine, err)
		}
		story.SoundBank, err = snd.Parse(file)
		file.Close()
		if err != nil {
			return nil, context("snd", definition.SndLine, fmt.Errorf("%s: %w", soundPath, err))
		}
	}
	for i, name := range definition.FontFiles {
		if name == "" {
			continue
		}
		key := fmt.Sprintf("font%d", i)
		fontPath, err := resource.Resolve(name, dir, filepath.Join(root, "font"))
		if err != nil {
			return nil, context(key, definition.FontLines[i], err)
		}
		story.Fonts[i], err = font.LoadFont(fontPath, languages...)
		if err != nil {
			return nil, context(key, definition.FontLines[i], fmt.Errorf("%s: %w", fontPath, err))
		}
	}
	story.AIR, err = air.ParseFromAST(ast)
	if err != nil {
		return nil, fmt.Errorf("%s embedded AIR: %w", source, err)
	}
	clear := color.RGBA{A: 255}
	clearSet := false
	position := [2]float64{}
	window := image.Rect(0, 0, story.LocalCoord.Width, story.LocalCoord.Height)
	for _, def := range definition.Scenes {
		if !def.EndTimeSet || def.EndTime < 0 {
			return nil, fmt.Errorf("%s [%s] (line %d): end.time is required and must be nonnegative", source, def.Name, def.Line)
		}
		scene := &Scene{Name: def.Name, EndTime: def.EndTime, RawSprites: story.RawSprites,
			MusicFile: def.MusicFile, MusicLoop: def.MusicLoop, MusicSet: def.MusicSet,
			Fader: FadeTool{FadeInTime: def.FadeInTime, FadeOutTime: def.FadeOutTime,
				FadeInColor: rgb(def.FadeInColor), FadeOutColor: rgb(def.FadeOutColor)}}
		if def.ClearColorSet {
			clear, clearSet = rgb(def.ClearColor), def.ClearColor[0] != -1
		}
		scene.ClearColor, scene.ClearColorSet = clear, clearSet
		if def.DefaultPosSet {
			position = [2]float64{float64(def.DefaultPosition[0]), float64(def.DefaultPosition[1])}
		}
		scene.DefaultPosition = position
		if def.WindowSet {
			window = image.Rect(def.Window[0], def.Window[1], def.Window[2]+1, def.Window[3]+1)
		}
		scene.Window = window
		scene.WindowSet = true
		for index, event := range def.Sounds {
			if !event.Set {
				continue
			}
			key := fmt.Sprintf("%d-%d", event.Group, event.Item)
			if story.SoundBank == nil || story.SoundBank.Map[key] == nil {
				return nil, fmt.Errorf("%s [%s] sound%d.value: sound %s unavailable", source, def.Name, index, key)
			}
			scene.Sounds = append(scene.Sounds, SoundEvent{StartTime: event.StartTime, Group: event.Group, Item: event.Item, Volume: max(0, min(100, event.Volume)), Pan: max(-127, min(127, event.Pan))})
		}
		if def.BGName != "" {
			scene.Background = &background.Background{LocalCoord: story.LocalCoord}
			scene.Background.LoadFromAST(ast, def.BGName, story.SpriteData, story.AIR)
		}
		for _, layer := range def.Layers {
			if layer.AnimNo < 0 && layer.Text == "" {
				continue
			}
			l := &Layer{Index: layer.Index, Offset: [2]float64{float64(layer.OffsetX), float64(layer.OffsetY)}, StartTime: layer.StartTime, EndTime: layer.EndTime, EndTimeSet: layer.EndTime >= 0, RawSprites: story.RawSprites, Text: layer.Text, TextDelay: layer.TextDelay, FontSpec: layer.Font}
			if layer.Text != "" {
				if layer.AnimNo >= 0 {
					return nil, fmt.Errorf("%s [%s] layer%d: text and anim are mutually exclusive", source, def.Name, layer.Index)
				}
				index := layer.Font[0]
				if index < 0 || index >= len(story.Fonts) || story.Fonts[index] == nil {
					return nil, fmt.Errorf("%s [%s] layer%d.font (line %d): font %d unavailable", source, def.Name, layer.Index, layer.Line, index)
				}
				l.Font = story.Fonts[index]
			} else {
				action := story.AIR.Actions[layer.AnimNo]
				if action == nil {
					return nil, fmt.Errorf("%s [%s] layer%d.anim (line %d): action %d is missing", source, def.Name, layer.Index, layer.Line, layer.AnimNo)
				}
				l.Player = air.NewPlayer(action)
				if l.Player.Error != nil {
					return nil, fmt.Errorf("%s [%s] layer%d: %w", source, def.Name, layer.Index, l.Player.Error)
				}
			}
			scene.Layers = append(scene.Layers, l)
		}

		story.Scenes = append(story.Scenes, scene)
	}
	return story, nil
}

func rgb(value [3]int) color.RGBA {
	return color.RGBA{R: uint8(value[0]), G: uint8(value[1]), B: uint8(value[2]), A: 255}
}
