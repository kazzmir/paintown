package main

import (
	"fmt"
	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

func (s *FightState) effectWarning(err error) {
	if s.effectWarnings == nil {
		s.effectWarnings = map[string]bool{}
	}
	message := err.Error()
	if !s.effectWarnings[message] && len(s.effectWarnings) < 128 {
		s.effectWarnings[message] = true
		fmt.Printf("Fight effect warning: %s\n", message)
	}
}

type effectImageKey struct {
	archive *sff.SFF
	palette *palette.State
	index   int
}
type effectImage struct {
	revision uint64
	image    *ebiten.Image
}

func (s *FightState) drawEffects(screen *ebiten.Image) {
	if s.effectImages == nil {
		s.effectImages = map[effectImageKey]effectImage{}
	}
	used := map[effectImageKey]bool{}
	m := s.match
	toStage, _ := m.Options.GameSpace.ScaleTo(m.Stage.StageInfo.LocalCoord)
	for _, instance := range m.Effects.Active {
		el := instance.Clock.CurrentElement()
		if el == nil || el.Group < 0 {
			continue
		}
		source := instance.Source
		index := -1
		for i, sprite := range source.Sprites.Sprites {
			if int(sprite.GroupNumber) == el.Group && int(sprite.ImageNumber) == el.Image {
				index = i
				break
			}
		}
		if index < 0 {
			s.effectWarning(fmt.Errorf("action %d sprite %d,%d is unavailable", instance.Action, el.Group, el.Image))
			continue
		}
		key := effectImageKey{source.Sprites, source.Palette, index}
		used[key] = true
		revision := uint64(0)
		if source.Palette != nil {
			revision = source.Palette.Revision
		}
		cached, present := s.effectImages[key]
		if !present || cached.revision != revision {
			pixels := source.Sprites.Sprites[index].Image
			if source.Palette != nil {
				var err error
				pixels, err = source.Palette.Image(index, true)
				if err != nil {
					s.effectWarning(err)
					continue
				}
			}
			cached = effectImage{revision, ebiten.NewImageFromImage(pixels)}
			s.effectImages[key] = cached
		}
		sprite := source.Sprites.Sprites[index]
		sx, sy := float64(instance.Facing), 1.0
		if el.FlipX {
			sx = -sx
		}
		if el.FlipY {
			sy = -sy
		}
		op := &ebiten.DrawImageOptions{Filter: ebiten.FilterLinear}
		op.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
		op.GeoM.Scale(sx, sy)
		op.GeoM.Translate(float64(el.XOffset*instance.Facing), float64(el.YOffset))
		op.GeoM.Scale(source.Scale*toStage, source.Scale*toStage)
		op.GeoM.Translate(float64(m.Stage.StageInfo.LocalCoord.Width)/2+(instance.X-m.CameraX)*toStage, m.Stage.StageInfo.ZOffset+(instance.Y-m.CameraY)*toStage)
		switch el.Blend {
		case "alpha":
			src, dst := float32(max(0, min(256, el.AlphaSrc)))/256, float32(max(0, min(256, el.AlphaDst)))/256
			op.ColorScale.Scale(src, src, src, 1-dst)
			op.Blend = ebiten.Blend{BlendOperationRGB: ebiten.BlendOperationAdd, BlendOperationAlpha: ebiten.BlendOperationAdd,
				BlendFactorSourceRGB: ebiten.BlendFactorOne, BlendFactorDestinationRGB: ebiten.BlendFactorOneMinusSourceAlpha,
				BlendFactorSourceAlpha: ebiten.BlendFactorOne, BlendFactorDestinationAlpha: ebiten.BlendFactorOneMinusSourceAlpha}
		case "sub":
			op.Blend = ebiten.Blend{BlendOperationRGB: ebiten.BlendOperationReverseSubtract, BlendOperationAlpha: ebiten.BlendOperationAdd,
				BlendFactorSourceRGB: ebiten.BlendFactorOne, BlendFactorDestinationRGB: ebiten.BlendFactorOne,
				BlendFactorSourceAlpha: ebiten.BlendFactorZero, BlendFactorDestinationAlpha: ebiten.BlendFactorOne}
		}
		screen.DrawImage(cached.image, op)
	}
	for key := range s.effectImages {
		if !used[key] {
			delete(s.effectImages, key)
		}
	}
}
