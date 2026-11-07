package main

import (
	"fmt"
	"image"
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/effect"
	"github.com/kazzmir/paintown/game/mugen/palette"
	"github.com/kazzmir/paintown/game/mugen/render"
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

func (s *FightState) beginEffectDraw() {
	if s.effectImages == nil {
		s.effectImages = map[effectImageKey]effectImage{}
	}
	s.effectUsed = map[effectImageKey]bool{}
}

func (s *FightState) endEffectDraw() {
	for key, cached := range s.effectImages {
		if !s.effectUsed[key] {
			cached.image.Deallocate()
			delete(s.effectImages, key)
		}
	}
}

func (s *FightState) ownedEffectLayers(screen *ebiten.Image) []character.SpriteLayer {
	var layers []character.SpriteLayer
	// Earlier-created equal-priority effects are in front, so draw later ones first.
	for i := len(s.match.Effects.Active) - 1; i >= 0; i-- {
		instance := s.match.Effects.Active[i]
		if !instance.Active || !instance.OwnerDriven || instance.OnTop {
			continue
		}
		layers = append(layers, character.SpriteLayer{Priority: instance.SpritePriority, Draw: func() { s.drawEffectInstance(screen, instance) }})
	}
	return layers
}

func (s *FightState) drawEffects(screen *ebiten.Image, layers ...int) {
	for _, instance := range s.match.Effects.Active {
		if !instance.Active {
			continue
		}
		layer := 1
		if instance.Under {
			layer = 0
		} else if instance.OnTop {
			layer = 2
		} else if instance.OwnerDriven {
			continue
		}
		if len(layers) > 0 && layer != layers[0] {
			continue
		}
		s.drawEffectInstance(screen, instance)
	}
}

func (s *FightState) drawEffectInstance(screen *ebiten.Image, instance *effect.Instance) {
	s.drawEffectPose(screen, instance, nil)
}

func (s *FightState) drawEffectShadows(screen *ebiten.Image, shadow render.Shadow) {
	for _, instance := range s.match.Effects.Active {
		if instance.Active && instance.Shadow {
			s.drawEffectPose(screen, instance, &shadow)
		}
	}
}

func (s *FightState) drawEffectPose(screen *ebiten.Image, instance *effect.Instance, shadow *render.Shadow) {
	if s.effectImages == nil {
		s.effectImages = map[effectImageKey]effectImage{}
	}
	if s.effectUsed == nil {
		s.effectUsed = map[effectImageKey]bool{}
	}
	m := s.match
	stageToGame, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	if !instance.Active {
		return
	}
	visual := instance.Clock.Visual()
	el := visual.Element
	if el == nil || el.Group < 0 {
		return
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
		return
	}
	key := effectImageKey{source.Sprites, source.Palette, index}
	s.effectUsed[key] = true

	revision := uint64(0)
	if source.Palette != nil {
		revision = source.Palette.Revision
	}
	cached, present := s.effectImages[key]
	if !present || cached.revision != revision {
		pixels := source.Sprites.Sprites[index].Image
		if source.Palette != nil {
			var err error
			pixels, err = source.Palette.MappedImage(index, true)
			if err != nil {
				s.effectWarning(err)
				return
			}
		}
		if present {
			cached.image.Deallocate()
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
	op.GeoM.Scale(visual.ScaleX, visual.ScaleY)
	op.GeoM.Rotate(-visual.Angle * float64(instance.Facing) * math.Pi / 180)
	op.GeoM.Translate(visual.X*float64(instance.Facing), visual.Y)
	op.GeoM.Scale(source.Scale, source.Scale)
	if instance.HasScale {
		op.GeoM.Scale(instance.ScaleX, instance.ScaleY)
	}
	if sx < 0 {
		op.GeoM.Translate(1, 0)
	}
	if sy < 0 {
		op.GeoM.Translate(0, 1)
	}
	x, y := instance.X, instance.Y
	if _, indexed := sprite.Image.(*image.Paletted); indexed {
		x, y = x+0.5, y+0.5
	} else {
		x, y = math.Trunc(x), math.Trunc(y)
	}
	if !instance.ScreenSpace {
		x += float64(m.Options.GameSpace.Width)/2 - m.CameraX
		y += m.Stage.StageInfo.ZOffset*stageToGame - m.CameraY
	}
	projected := instance.XAngle != 0 || instance.YAngle != 0
	if !projected {
		op.GeoM.Rotate(-instance.Angle * math.Pi / 180)
		op.GeoM.Translate(x, y)
	}
	blend, alphaSrc, alphaDst := el.Blend, visual.AlphaSrc, visual.AlphaDst
	if instance.HasBlend {
		blend, alphaSrc, alphaDst = instance.Blend, instance.AlphaSrc, instance.AlphaDst
	}
	var style render.DrawStyle
	if shadow != nil {
		opacity := shadow.Opacity(instance.Y, blend, alphaDst)
		if opacity <= 0 {
			return
		}
		op.ColorScale.Scale(0, 0, 0, opacity)
		style.DestinationFactor = -1
		if !projected {
			_, indexed := sprite.Image.(*image.Paletted)
			reverse := shadow.SamplingReversal(op.GeoM, 0, indexed, sy < 0)
			style.SourceReversal = &reverse
			op.GeoM = shadow.SpriteGeometry(op.GeoM, 0, indexed, sy < 0)
		}
	} else {
		nativeDestination := render.ApplyBlend(op, blend, alphaSrc, alphaDst)
		style = render.DrawStyle{DestinationFactor: nativeDestination, Palette: palette.Combine(source.PaletteTransform(), m.Timeline.AllPalette.Transform())}
	}

	if projected {
		projection := render.Perspective{X: x, Y: y, CenterX: float64(m.Options.GameSpace.Width) / 2, CenterY: float64(m.Options.GameSpace.Height) / 2, Distance: float64(m.Options.GameSpace.Width) * 500 / 320, Angle: instance.Angle, XAngle: instance.XAngle, YAngle: instance.YAngle}
		if shadow != nil {
			projection.Post = shadow.Projection(0)
		}
		var err error
		if _, indexed := sprite.Image.(*image.Paletted); indexed {
			err = s.indexSampler.DrawStyledProjected(screen, cached.image, op, projection, style)
		} else {
			err = s.linearSampler.DrawStyledProjected(screen, cached.image, op, projection, style)
		}
		if err != nil {
			s.renderError = fmt.Errorf("fight effect projection: %w", err)
			return
		}
	} else if _, indexed := sprite.Image.(*image.Paletted); indexed {
		if err := s.indexSampler.DrawStyledImage(screen, cached.image, op, style); err != nil {
			s.renderError = fmt.Errorf("fight effect action %d indexed sampler: %w", instance.Action, err)
			return
		}
	} else if err := s.linearSampler.DrawStyledImage(screen, cached.image, op, style); err != nil {
		s.renderError = fmt.Errorf("fight effect action %d sprite %d,%d sampler: %w", instance.Action, el.Group, el.Image, err)
		return
	}
}
