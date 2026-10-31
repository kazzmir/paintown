package paintown

import (
	"image"
	"image/color"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/vector"
)

// Slider binds a bounded, discrete GUI value to shared settings. It does not
// store a second copy, so multiple menus always display the current value.
type Slider struct {
	Min, Max, Step int
	GetValue       func() int
	SetValue       func(int)
}

func (slider *Slider) Value() int {
	if slider.GetValue == nil {
		return slider.Min
	}
	return max(slider.Min, min(slider.Max, slider.GetValue()))
}

func (slider *Slider) Adjust(direction int) bool {
	if direction == 0 || slider.SetValue == nil || slider.Max <= slider.Min {
		return false
	}
	current := slider.Value()
	next := max(slider.Min, min(slider.Max, current+direction*max(1, slider.Step)))
	if next == current {
		return false
	}
	slider.SetValue(next)
	return true
}

func (slider *Slider) Fraction() float64 {
	if slider.Max <= slider.Min {
		return 0
	}
	return float64(slider.Value()-slider.Min) / float64(slider.Max-slider.Min)
}

func (slider *Slider) Draw(screen *ebiten.Image, bounds image.Rectangle, selected color.NRGBA) {
	if bounds.Empty() {
		return
	}
	radius := float32(min(8, min(bounds.Dx(), bounds.Dy())/2))
	left, right := float32(bounds.Min.X)+radius, float32(bounds.Max.X)-radius
	y := float32(bounds.Min.Y+bounds.Max.Y) / 2
	thumb := left + (right-left)*float32(slider.Fraction())
	vector.StrokeLine(screen, left, y, right, y, 6, color.NRGBA{R: 65, G: 65, B: 75, A: 230}, true)
	if thumb > left {
		vector.StrokeLine(screen, left, y, thumb, y, 6, selected, true)
	}
	vector.FillCircle(screen, thumb, y, radius, selected, true)
	vector.StrokeCircle(screen, thumb, y, radius, 1, color.NRGBA{R: 235, G: 235, B: 235, A: 255}, true)
}

func makeSoundVolumeMenu(parent *Menu) *Menu {
	return makeVolumeMenu(parent, "Sound Volume", "Sound effects", SoundVolume, SetSoundVolume)
}

func makeMusicVolumeMenu(parent *Menu) *Menu {
	return makeVolumeMenu(parent, "Music Volume", "Music", MusicVolume, SetMusicVolume)
}

func makeVolumeMenu(parent *Menu, title, label string, get func() int, set func(int)) *Menu {
	menu := *parent
	menu.Name, menu.MenuInfo = title, title
	menu.Options = []MenuOption{{
		Kind: "slider", Name: label, Info: "Left / Right: adjust volume\nEscape / Circle: back",
		Slider: &Slider{Min: 0, Max: 100, Step: 1, GetValue: get, SetValue: set},
	}}
	menu.Selected, menu.ticks = 0, 0
	menu.opacity, menu.scrollOffset = 0, 0
	menu.Notice = ""
	menu.Bounds = image.Rect(100, 150, 540, 380)
	menu.InfoX, menu.InfoY, menu.MenuInfoX, menu.MenuInfoY = 320, 85, 320, 450
	menu.Scroll, menu.Wrap, menu.DistanceFade = false, false, false
	return &menu
}
