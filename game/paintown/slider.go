package paintown

import (
	"image"
	"image/color"
	"strconv"
	"sync/atomic"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/vector"
)

// Slider binds a bounded, discrete GUI value to shared settings. It does not
// store a second copy, so multiple menus always display the current value.
type Slider struct {
	Min, Max, Step int
	Suffix         string
	GetValue       func() int
	SetValue       func(int)
}

func (slider *Slider) Value() int {
	if slider.GetValue == nil {
		return slider.Min
	}
	return max(slider.Min, min(slider.Max, slider.GetValue()))
}

func (slider *Slider) Label() string { return strconv.Itoa(slider.Value()) + slider.Suffix }

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

var configuredStartingLives atomic.Int32
var configuredNPCBuddyCount atomic.Int32

func init() {
	configuredStartingLives.Store(4)
	configuredNPCBuddyCount.Store(2)
}

// NPCBuddyCount controls new AI-mode parties; it never changes an active roster.
func NPCBuddyCount() int         { return int(configuredNPCBuddyCount.Load()) }
func SetNPCBuddyCount(count int) { configuredNPCBuddyCount.Store(int32(max(1, min(10, count)))) }

func makeNPCBuddiesMenu(parent *Menu) *Menu {
	menu := makeSliderMenu(parent, "NPC Buddies", "AI buddies", &Slider{Min: 1, Max: 10, Step: 1, GetValue: NPCBuddyCount, SetValue: SetNPCBuddyCount})
	menu.Options[0].Info = "Left / Right: adjust\nApplies to new Arcade games with AI buddies"
	return menu
}

func StartingLives() int         { return int(configuredStartingLives.Load()) }
func SetStartingLives(lives int) { configuredStartingLives.Store(int32(max(1, min(100, lives)))) }

func makeLivesMenu(parent *Menu) *Menu {
	return makeSliderMenu(parent, "Lives", "Starting lives", &Slider{Min: 1, Max: 100, Step: 1, GetValue: StartingLives, SetValue: SetStartingLives})
}

func makeVolumeMenu(parent *Menu, title, label string, get func() int, set func(int)) *Menu {
	return makeSliderMenu(parent, title, label, &Slider{Min: 0, Max: 100, Step: 1, Suffix: "%", GetValue: get, SetValue: set})
}

func makeSliderMenu(parent *Menu, title, label string, slider *Slider) *Menu {
	menu := *parent
	menu.Name, menu.MenuInfo = title, title
	menu.Options = []MenuOption{{
		Kind: "slider", Name: label, Info: "Left / Right: adjust\nEscape / Circle: back",
		Slider: slider,
	}}
	menu.Selected, menu.ticks = 0, 0
	menu.opacity, menu.scrollOffset = 0, 0
	menu.Notice = ""
	menu.Bounds = image.Rect(100, 150, 540, 380)
	menu.InfoX, menu.InfoY, menu.MenuInfoX, menu.MenuInfoY = 320, 85, 320, 450
	menu.Scroll, menu.Wrap, menu.DistanceFade = false, false, false
	return &menu
}
