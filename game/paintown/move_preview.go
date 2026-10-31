package paintown

import (
	"image"
	"image/color"
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/vector"
	"github.com/kazzmir/paintown/game/data"
)

// A preview is an animation owner, never a live PlayerState. Animation events
// can move it, change facing, and emit local projectiles without touching the
// paused world, attack tickets, trails, or audio.
type movePreview struct {
	source, animation *Animation
	x, y, z           float64
	facing            Facing
	projectiles       []*Projectile
	wait              int
	width, height     int
}

func newMovePreview(source *Animation) *movePreview {
	preview := &movePreview{source: source, animation: source.Clone()}
	for _, event := range source.Events {
		if frame, ok := event.(*AnimationEventFrame); ok && frame.Image != nil {
			preview.width = max(preview.width, frame.Image.Bounds().Dx())
			preview.height = max(preview.height, frame.Image.Bounds().Dy())
		}
	}
	preview.reset()
	return preview
}

func (preview *movePreview) reset() {
	preview.x, preview.y, preview.z = 0, 0, 0
	if preview.source.Status == "jump" {
		preview.y = 30
	}
	preview.facing = FacingRight
	preview.wait = 0
	preview.projectiles = nil
	animation := preview.animation
	animation.Owner = preview
	// Clone may come from a move paused halfway through playback. Restore the
	// initial event-driven state, not its live frame, offsets, or delay.
	animation.Frame = nil
	animation.OffsetX, animation.OffsetY, animation.Delay = 0, 0, 1
	animation.Reset()
	animation.Update(false, &dummySystem{})
}

func (preview *movePreview) Update() {
	system := &dummySystem{}
	if preview.wait > 0 {
		preview.wait--
		if preview.wait == 0 {
			preview.reset()
		}
	} else if preview.animation.Update(false, system) && preview.animation.CurrentDelay == 0 {
		preview.wait = 30
	}
	out := preview.projectiles[:0]
	for _, projectile := range preview.projectiles {
		projectile.Update(system)
		if !projectile.Done {
			out = append(out, projectile)
		}
	}
	clear(preview.projectiles[len(out):])
	preview.projectiles = out
}

func (preview *movePreview) Move(x, y, z int) {
	preview.x += float64(x)
	preview.y += float64(y)
	preview.z += float64(z)
}
func (preview *movePreview) GetX() float64                 { return preview.x }
func (preview *movePreview) GetY() float64                 { return preview.y }
func (preview *movePreview) GetZ() float64                 { return preview.z }
func (preview *movePreview) GetFacing() Facing             { return preview.facing }
func (preview *movePreview) SetFacing(facing Facing)       { preview.facing = facing }
func (preview *movePreview) SetTrail(generate, length int) {}
func (preview *movePreview) NextAttackId()                 {}
func (preview *movePreview) QueueProjectile(projectile *Projectile) {
	preview.projectiles = append(preview.projectiles, projectile)
}

func (pause *PauseMenu) updateMovePreview() {
	menu := pause.Current
	if menu.Selected < 0 || menu.Selected >= len(menu.Options) || menu.Options[menu.Selected].moveAnimation == nil {
		pause.preview = nil
		return
	}
	source := menu.Options[menu.Selected].moveAnimation
	if pause.preview == nil || pause.preview.source != source {
		pause.preview = newMovePreview(source)
	} else {
		pause.preview.Update()
	}
}

func (pause *PauseMenu) Draw(screen *ebiten.Image) {
	pause.Current.DrawOverlay(screen)
	if pause.preview == nil {
		return
	}
	if pause.previewLayer == nil {
		pause.previewLayer = ebiten.NewImage(data.ScreenWidth, data.ScreenHeight)
	}
	layer := pause.previewLayer
	layer.Clear()
	pause.preview.Draw(layer, pause.Current)
	var options ebiten.DrawImageOptions
	options.ColorScale.ScaleAlpha(float32(pause.Current.opacity))
	screen.DrawImage(layer, &options)
}

func (preview *movePreview) Draw(screen *ebiten.Image, menu *Menu) {
	bounds := image.Rect(310, 120, 610, 420)
	menuRoundRect(screen, bounds, menu.Radius, menu.BodyColor, menu.BorderColor)
	target := screen.SubImage(bounds.Inset(1)).(*ebiten.Image)
	centerX, groundY := float64(bounds.Min.X+bounds.Max.X)/2, float64(bounds.Max.Y-22)
	menu.drawText(target, "Preview", centerX, float64(bounds.Min.Y+18), color.NRGBA{R: 255, G: 255, B: 255, A: 220})
	vector.StrokeLine(target, float32(bounds.Min.X+12), float32(groundY), float32(bounds.Max.X-12), float32(groundY), 1, color.NRGBA{R: 160, G: 160, B: 160, A: 120}, false)
	scale := math.Min(1.5, math.Min(float64(bounds.Dx()-24)/float64(max(1, preview.width)), float64(bounds.Dy()-60)/float64(max(1, preview.height))))
	var world ebiten.GeoM
	world.Scale(scale, scale)
	world.Translate(centerX, groundY)
	animation := preview.animation
	if frame := animation.CurrentFrame(); frame != nil {
		var options ebiten.DrawImageOptions
		options.GeoM.Translate(-float64(frame.Bounds().Dx())/2+float64(animation.GetOffsetX()), -float64(frame.Bounds().Dy())+float64(animation.GetOffsetY()))
		if preview.facing == FacingLeft {
			options.GeoM.Scale(-1, 1)
		}
		options.GeoM.Translate(preview.x, preview.z-preview.y)
		options.GeoM.Concat(world)
		target.DrawImage(frame, &options)
	}
	for _, projectile := range preview.projectiles {
		projectile.Draw(target, 0, world)
	}
}
