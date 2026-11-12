package main

import (
	"fmt"
	"image/color"
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/ebitenutil"
	"github.com/hajimehoshi/ebiten/v2/vector"
	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/coord"
)

func (s *FightState) drawCollisionDebug(screen *ebiten.Image) {
	if !s.engine.showCollisions || !s.engine.demoShowsDebug() {
		return
	}
	m := s.match
	space := m.Options.GameSpace
	projection, err := coord.Project(space, coord.Space{Width: screen.Bounds().Dx(), Height: screen.Bounds().Dy()}, s.engine.cfg.Video.KeepAspect)
	if err != nil {
		return
	}
	projection.X += float64(screen.Bounds().Min.X)
	projection.Y += float64(screen.Bounds().Min.Y) + m.Timeline.ShakeOffset()*float64(space.Width)/320*projection.ScaleY
	zoom := m.Viewport.ZoomTransform()
	floorScale, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(space)
	floor := m.Stage.GroundLevel()*floorScale - m.CameraY
	// Match the fitted world window. Debug boxes can be offscreen in scripts
	// without painting over the outer letterbox.
	clip := fittedBounds(s.gameCanvas, projection, screen.Bounds())
	if clip.Empty() {
		return
	}
	target := screen.SubImage(clip).(*ebiten.Image)
	var descriptions []string
	for _, actor := range character.OrderedActors(m.Actors()) {
		c := actor.Character
		unit := float64(space.Width) / float64(c.LocalCoordinateWidth())
		point := func(x, y float64) (float32, float32) {
			x, y = zoom.Point(float64(space.Width)/2+x*unit-m.CameraX, floor+y*unit)
			x, y = projection.Point(x, y)
			return float32(x), float32(y)
		}
		for _, box := range c.CollisionDebugBoxes() {
			x1, y1 := point(box.X1, box.Y1)
			x2, y2 := point(box.X2, box.Y2)
			x, y := float32(math.Min(float64(x1), float64(x2))), float32(math.Min(float64(y1), float64(y2)))
			w, h := float32(math.Abs(float64(x2-x1))), float32(math.Abs(float64(y2-y1)))
			ink := color.RGBA{60, 130, 255, 220}
			if box.Attack {
				ink = color.RGBA{255, 65, 65, 220}
			}
			vector.StrokeRect(target, x, y, w, h, 1, ink, false)
		}
		status := c.CombatDebugState()
		line := fmt.Sprintf("ID %d Juggle %d Targets %v", c.EntityID, status.Juggle, status.Targets)
		for slot, filter := range status.Protection {
			if filter.Time != 0 {
				line += fmt.Sprintf(" HitBy%d=%s (%d)", slot, filter.Attributes, filter.Time)
			}
		}
		descriptions = append(descriptions, line)
	}
	// Diagnostics are an overlay, independent of the world's palette effects.
	ebitenutil.DebugPrintAt(screen, strings.Join(descriptions, "\n"), screen.Bounds().Min.X+8, screen.Bounds().Min.Y+24)
}
