package main

import (
	"fmt"
	"image"
	"math"

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

func (s *FightState) drawEffects(screen *ebiten.Image, layers ...int) {
	if s.effectImages == nil {
		s.effectImages = map[effectImageKey]effectImage{}
	}
	used := map[effectImageKey]bool{}
	m := s.match
	stageToGame, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	for _, instance := range m.Effects.Active {
		if !instance.Active {
			continue
		}
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
		layer := 1
		if instance.OnTop {
			layer = 2
		} else if instance.OwnerDriven && instance.SpritePriority < 0 {
			layer = 0
		}
		if len(layers) > 0 && layer != layers[0] {
			continue
		}
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
		if instance.VFacing < 0 {
			sy = -sy
		}
		if el.FlipX {
			sx = -sx
		}
		if el.FlipY {
			sy = -sy
		}
		op := &ebiten.DrawImageOptions{Filter: ebiten.FilterNearest}
		op.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))
		op.GeoM.Scale(sx, sy)
		op.GeoM.Translate(float64(el.XOffset*instance.Facing), float64(el.YOffset))
		op.GeoM.Scale(source.Scale, source.Scale)
		if instance.HasScale {
			op.GeoM.Scale(instance.ScaleX, instance.ScaleY)
		}
		x, y := instance.X, instance.Y
		if _, indexed := sprite.Image.(*image.Paletted); !indexed {
			x, y = math.Trunc(x), math.Trunc(y)
		}
		if !instance.ScreenSpace {
			x += float64(m.Options.GameSpace.Width)/2 - m.CameraX
			y += m.Stage.StageInfo.ZOffset*stageToGame - m.CameraY
		}
		op.GeoM.Translate(x, y)
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
		if _, indexed := sprite.Image.(*image.Paletted); indexed {
			screen.DrawImage(cached.image, op)
		} else if err := s.linearSampler.DrawImage(screen, cached.image, op); err != nil {
			s.renderError = fmt.Errorf("fight effect action %d sprite %d,%d sampler: %w", instance.Action, el.Group, el.Image, err)
			return
		}
	}
	for key := range s.effectImages {
		if !used[key] {
			delete(s.effectImages, key)
		}
	}
}
