package paintown

import (
	"bytes"
	"fmt"
	"image"
	"image/color"
	"io"
	"math"
	"path/filepath"
	"sort"
	"strconv"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/hajimehoshi/ebiten/v2/text/v2"
	"github.com/hajimehoshi/ebiten/v2/vector"
	"github.com/kazzmir/paintown/game/data"
	"github.com/kazzmir/paintown/game/lib/sexp"
)

// Menu is the data-driven GUI menu, separate from character animations.
// Coordinates are resolved in the menu's 640x480 logical drawing space.
type Menu struct {
	Name, Info, MenuInfo                           string
	Options                                        []MenuOption
	Selected                                       int
	Bounds                                         image.Rectangle
	InfoX, InfoY, MenuInfoX, MenuInfoY             float64
	Radius, FontSize, FontWidth, FadeSpeed         float64
	FontPath, BackgroundPath                       string
	SelectSound, BackSound, OKSound                string
	ClearColor, BodyColor, BorderColor             color.NRGBA
	LowColor, HighColor, SelectedColor, OtherColor color.NRGBA
	Scroll, Wrap, Interpolate, DistanceFade        bool
	InterpolateDistance, FadeMultiplier            float64
	Animations                                     []*MenuAnimation
	Notice                                         string

	font                  *text.GoTextFace
	background            *ebiten.Image
	layer                 *ebiten.Image
	ticks                 int
	opacity, scrollOffset float64
}

type MenuOption struct {
	Name, Info, Kind, Path string
	Definition             *sexp.SExpr
	moveAnimation          *Animation
}

type MenuAnimation struct {
	Depth                              int
	Images                             map[int]*ebiten.Image
	Paths                              map[int]string
	Frames                             []MenuFrame
	VelocityX, VelocityY, AxisX, AxisY float64
	Clip                               image.Rectangle
	current, elapsed                   int
	scrollX, scrollY                   float64
}

type MenuFrame struct {
	Image, Time  int
	Alpha        uint8
	X, Y         float64
	HFlip, VFlip bool
}

// menuNumbers also accepts the comma-separated coordinates used by main.txt.
func menuNumber(node *sexp.SExpr, index int, fallback float64) float64 {
	if node == nil || index >= len(node.Children) {
		return fallback
	}
	value, err := strconv.ParseFloat(strings.Trim(node.GetValue(index), ","), 64)
	if err != nil {
		return fallback
	}
	return value
}

func menuText(node *sexp.SExpr) string {
	if node == nil {
		return ""
	}
	var fallback string
	for _, child := range node.Children {
		if child.Name == "language" {
			value := child.GetValue(1)
			if child.GetValue(0) == "English" {
				return strings.ReplaceAll(value, `\n`, "\n")
			}
			if fallback == "" {
				fallback = value
			}
		}
	}
	if fallback != "" {
		return strings.ReplaceAll(fallback, `\n`, "\n")
	}
	return strings.ReplaceAll(node.GetValue(0), `\n`, "\n")
}

func menuColor(node *sexp.SExpr, fallback color.NRGBA) color.NRGBA {
	if node == nil {
		return fallback
	}
	return color.NRGBA{
		R: uint8(max(0, min(255, menuNumber(node, 0, float64(fallback.R))))),
		G: uint8(max(0, min(255, menuNumber(node, 1, float64(fallback.G))))),
		B: uint8(max(0, min(255, menuNumber(node, 2, float64(fallback.B))))),
		A: uint8(max(0, min(255, menuNumber(node, 3, float64(fallback.A))))),
	}
}

func menuPoint(x, y float64) (float64, float64) {
	return (x + 1) * data.ScreenWidth / 2, (y + 1) * data.ScreenHeight / 2
}

func defaultMenu() *Menu {
	return &Menu{
		Bounds: image.Rect(128, 192, 512, 408),
		InfoX:  320, InfoY: 120, MenuInfoX: 320, MenuInfoY: 468,
		Radius: 0, FontSize: 24, FontWidth: 24, FadeSpeed: 12, FontPath: "fonts/arial.ttf",
		ClearColor: color.NRGBA{A: 255}, BodyColor: color.NRGBA{A: 128}, BorderColor: color.NRGBA{R: 200, G: 200, B: 200, A: 255},
		LowColor: color.NRGBA{R: 19, G: 167, B: 168, A: 255}, HighColor: color.NRGBA{R: 27, G: 237, B: 239, A: 255},
		SelectedColor: color.NRGBA{R: 19, G: 167, B: 168, A: 255}, OtherColor: color.NRGBA{R: 255, G: 255, B: 255, A: 255},
		Scroll: true, Wrap: true, Interpolate: true, DistanceFade: true, InterpolateDistance: 50, FadeMultiplier: 35,
	}
}

// ParseMenu takes an optional parent for inherited presentation settings and a
// reader for submenu labels. It does not load graphics or require a display.
func ParseMenu(root *sexp.SExpr, parent *Menu, read func(string) (*sexp.SExpr, error)) (*Menu, error) {
	if root == nil || root.Name != "menu" {
		return nil, fmt.Errorf("expected menu definition")
	}
	menu := defaultMenu()
	if parent != nil {
		*menu = *parent
		menu.Options = nil
		menu.Selected, menu.ticks = 0, 0
		menu.opacity, menu.scrollOffset = 0, 0
		menu.Notice, menu.MenuInfo = "", ""
	}
	// The last name wins (arcade.txt includes both a legacy and localized name).
	for _, child := range root.Children {
		switch child.Name {
		case "name":
			menu.Name = menuText(child)
		case "info":
			menu.Info = menuText(child)
		}
	}
	menu.MenuInfo = menuText(root.GetChild("menuinfo"))
	if coordinate := root.GetChild("coordinate"); coordinate != nil {
		if relative := coordinate.GetChild("relative"); relative != nil {
			x1, y1 := menuPoint(menuNumber(relative, 0, -.6), menuNumber(relative, 1, -.2))
			x2, y2 := menuPoint(menuNumber(relative, 2, .6), menuNumber(relative, 3, .7))
			menu.Bounds = image.Rect(int(x1), int(y1), int(x2), int(y2))
		} else if absolute := coordinate.GetChild("absolute"); absolute != nil {
			x, y := int(menuNumber(absolute, 0, 0)), int(menuNumber(absolute, 1, 0))
			menu.Bounds = image.Rect(x, y, x+int(menuNumber(absolute, 2, 640)), y+int(menuNumber(absolute, 3, 480)))
		}
	}
	if menu.Bounds.Empty() {
		return nil, fmt.Errorf("menu %s has empty bounds", menu.Name)
	}
	if transforms := root.GetChild("transforms"); transforms != nil {
		menu.Radius = menuNumber(transforms.GetChild("radius"), 0, menu.Radius)
	}
	for _, field := range []struct {
		name string
		x, y *float64
	}{
		{"info-position", &menu.InfoX, &menu.InfoY}, {"menuinfo-position", &menu.MenuInfoX, &menu.MenuInfoY},
	} {
		if node := root.GetChild(field.name); node != nil {
			*field.x, *field.y = menuPoint(menuNumber(node, 0, 0), menuNumber(node, 1, 0))
		}
	}
	menu.BodyColor = menuColor(root.GetChild("position-body"), menu.BodyColor)
	menu.BorderColor = menuColor(root.GetChild("position-border"), menu.BorderColor)
	menu.ClearColor = menuColor(root.GetChild("clear-color"), menu.ClearColor)
	menu.FadeSpeed = max(1, menuNumber(root.GetChild("fade-speed"), 0, menu.FadeSpeed))
	if font := root.GetChild("font"); font != nil {
		menu.FontPath = font.GetValue(0)
		menu.FontWidth = max(1, menuNumber(font, 1, 24))
		menu.FontSize = max(1, menuNumber(font, 2, 24))
		menu.font = nil
	}
	for _, field := range []struct {
		name  string
		value *string
	}{
		{"select-sound", &menu.SelectSound}, {"back-sound", &menu.BackSound}, {"ok-sound", &menu.OKSound}, {"background", &menu.BackgroundPath},
	} {
		if node := root.GetChild(field.name); node != nil {
			*field.value = node.GetValue(0)
		}
	}
	if display := root.GetChild("display-list"); display != nil {
		if kind := display.GetChild("type"); kind != nil {
			menu.Scroll = kind.GetValue(0) != "normal"
		}
		menu.Wrap = menuNumber(display.GetChild("wrap"), 0, boolNumber(menu.Wrap)) != 0
		if items := display.GetChild("items"); items != nil {
			menu.Interpolate = menuNumber(items.GetChild("interpolate-selected"), 0, boolNumber(menu.Interpolate)) != 0
			menu.DistanceFade = menuNumber(items.GetChild("distance-fade"), 0, boolNumber(menu.DistanceFade)) != 0
			menu.InterpolateDistance = max(1, menuNumber(items.GetChild("interpolate-distance"), 0, menu.InterpolateDistance))
			menu.FadeMultiplier = menuNumber(items.GetChild("distance-fade-multiplier"), 0, menu.FadeMultiplier)
			menu.LowColor = menuColor(items.GetChild("color-low"), menu.LowColor)
			menu.HighColor = menuColor(items.GetChild("color-high"), menu.HighColor)
			menu.SelectedColor = menuColor(items.GetChild("selected-color"), menu.SelectedColor)
			menu.OtherColor = menuColor(items.GetChild("other-color"), menu.OtherColor)
			menu.SelectedColor.A = uint8(max(0, min(255, menuNumber(items.GetChild("selected-color-alpha"), 0, float64(menu.SelectedColor.A)))))
			menu.OtherColor.A = uint8(max(0, min(255, menuNumber(items.GetChild("other-color-alpha"), 0, float64(menu.OtherColor.A)))))
		}
	}
	var animations []*MenuAnimation
	for _, child := range root.Children {
		switch child.Name {
		case "anim":
			animations = append(animations, parseMenuAnimation(child))
		case "menu":
			// Inline tab contents can also be navigated as submenus.
			menu.Options = append(menu.Options, MenuOption{Kind: "menu", Name: menuText(child.GetChild("name")), Info: menuText(child.GetChild("info")), Definition: child})
		case "option":
			if len(child.Children) == 0 {
				continue
			}
			action := child.Children[0]
			option := MenuOption{Kind: action.Name, Name: menuText(action.GetChild("name")), Info: menuText(action.GetChild("info"))}
			if action.Name == "menu" {
				option.Path = action.GetValue(0)
				if option.Path == "" {
					option.Definition = action
				}
				if option.Path != "" && read != nil {
					sub, err := read(option.Path)
					if err != nil {
						return nil, fmt.Errorf("submenu %s: %w", option.Path, err)
					}
					for _, node := range sub.Children {
						if node.Name == "name" {
							option.Name = menuText(node)
						}
						if node.Name == "info" {
							option.Info = menuText(node)
						}
					}
				}
			}
			if info := child.GetChild("info"); info != nil {
				option.Info = menuText(info)
			}
			if option.Name == "" {
				option.Name = strings.ReplaceAll(action.Name, "-", " ")
			}
			menu.Options = append(menu.Options, option)
		}
	}
	if len(animations) > 0 {
		menu.Animations = animations
	}
	sort.SliceStable(menu.Animations, func(i, j int) bool { return menu.Animations[i].Depth < menu.Animations[j].Depth })
	return menu, nil
}

func boolNumber(value bool) float64 {
	if value {
		return 1
	}
	return 0
}

func readMenu(path string) (*sexp.SExpr, error) { return sexp.ReadSExpression(data.DataPath(path)) }

func LoadMenu(path string, parent *Menu) (*Menu, error) {
	root, err := readMenu(path)
	if err != nil {
		return nil, err
	}
	menu, err := ParseMenu(root, parent, readMenu)
	if err != nil {
		return nil, err
	}
	if err := menu.loadAssets(); err != nil {
		return nil, fmt.Errorf("loading menu %s: %w", path, err)
	}
	return menu, nil
}

func (menu *Menu) OpenSubmenu(option MenuOption) (*Menu, error) {
	if option.Definition == nil {
		return LoadMenu(option.Path, menu)
	}
	next, err := ParseMenu(option.Definition, menu, readMenu)
	if err != nil {
		return nil, err
	}
	if err := next.loadAssets(); err != nil {
		return nil, err
	}
	return next, nil
}

func (menu *Menu) loadAssets() error {
	if menu.font == nil {
		file, err := data.LoadFile(menu.FontPath)
		if err != nil {
			return err
		}
		fontData, err := io.ReadAll(file)
		file.Close()
		if err != nil {
			return err
		}
		// The font source retains its reader for lazy glyph loading.
		source, err := text.NewGoTextFaceSource(bytes.NewReader(fontData))
		if err != nil {
			return err
		}
		menu.font = &text.GoTextFace{Source: source, Size: menu.FontSize}
	}
	if menu.BackgroundPath != "" {
		bitmap, err := data.LoadPng(menu.BackgroundPath)
		if err != nil {
			return err
		}
		menu.background = ebiten.NewImageFromImage(bitmap)
	}
	for _, animation := range menu.Animations {
		for id, path := range animation.Paths {
			if animation.Images[id] != nil {
				continue
			}
			bitmap, err := data.LoadPng(path)
			if err != nil {
				return err
			}
			animation.Images[id] = ebiten.NewImageFromImage(bitmap)
		}
	}
	return nil
}

func (menu *Menu) Move(direction int) bool {
	if len(menu.Options) == 0 {
		return false
	}
	next := menu.Selected + direction
	if menu.Wrap {
		next = (next%len(menu.Options) + len(menu.Options)) % len(menu.Options)
	} else {
		next = max(0, min(len(menu.Options)-1, next))
	}
	if next == menu.Selected {
		return false
	}
	menu.Selected = next
	menu.scrollOffset += float64(direction)
	menu.Notice = ""
	return true
}

func (menu *Menu) Update() {
	menu.ticks++
	menu.opacity = min(1, menu.opacity+menu.FadeSpeed/255)
	menu.scrollOffset *= .8
	for _, animation := range menu.Animations {
		animation.Update()
	}
}

func (menu *Menu) itemColor(distance int) color.NRGBA {
	if distance == 0 {
		c := menu.SelectedColor
		if menu.Interpolate {
			// A triangular low/high pulse, as in the original gradient.
			phase := math.Mod(float64(menu.ticks), menu.InterpolateDistance*2) / menu.InterpolateDistance
			if phase > 1 {
				phase = 2 - phase
			}
			c.R = uint8(float64(menu.LowColor.R)*(1-phase) + float64(menu.HighColor.R)*phase)
			c.G = uint8(float64(menu.LowColor.G)*(1-phase) + float64(menu.HighColor.G)*phase)
			c.B = uint8(float64(menu.LowColor.B)*(1-phase) + float64(menu.HighColor.B)*phase)
		}
		return c
	}
	c := menu.OtherColor
	if menu.DistanceFade {
		c.A = uint8(max(0, float64(c.A)-math.Abs(float64(distance))*menu.FadeMultiplier))
	}
	return c
}

func menuRoundRect(screen *ebiten.Image, bounds image.Rectangle, radius float64, body, border color.NRGBA) {
	x, y, w, h := float32(bounds.Min.X), float32(bounds.Min.Y), float32(bounds.Dx()), float32(bounds.Dy())
	r := float32(max(0, min(radius, float64(min(w, h)/2))))
	var path vector.Path
	path.MoveTo(x+r, y)
	path.LineTo(x+w-r, y)
	path.QuadTo(x+w, y, x+w, y+r)
	path.LineTo(x+w, y+h-r)
	path.QuadTo(x+w, y+h, x+w-r, y+h)
	path.LineTo(x+r, y+h)
	path.QuadTo(x, y+h, x, y+h-r)
	path.LineTo(x, y+r)
	path.QuadTo(x, y, x+r, y)
	path.Close()
	fill := &vector.DrawPathOptions{AntiAlias: true}
	fill.ColorScale.ScaleWithColor(body)
	vector.FillPath(screen, &path, nil, fill)
	stroke := &vector.DrawPathOptions{AntiAlias: true}
	stroke.ColorScale.ScaleWithColor(border)
	vector.StrokePath(screen, &path, &vector.StrokeOptions{Width: 1}, stroke)
}

func (menu *Menu) drawText(screen *ebiten.Image, value string, x, y float64, c color.NRGBA) {
	if menu.font == nil || value == "" {
		return
	}
	var options text.DrawOptions
	options.GeoM.Scale(menu.FontWidth/menu.FontSize, 1)
	options.GeoM.Translate(x, y)
	options.PrimaryAlign = text.AlignCenter
	options.SecondaryAlign = text.AlignCenter
	options.LayoutOptions.LineSpacing = menu.FontSize * 1.2
	options.ColorScale.ScaleWithColor(c)
	text.Draw(screen, value, menu.font, &options)
}

func (menu *Menu) Draw(screen *ebiten.Image) {
	screen.Fill(menu.ClearColor)
	if menu.background != nil {
		screen.DrawImage(menu.background, nil)
	}
	for _, animation := range menu.Animations {
		if animation.Depth < 3 {
			animation.Draw(screen)
		}
	}
	menu.DrawOverlay(screen)
	for _, animation := range menu.Animations {
		if animation.Depth >= 3 {
			animation.Draw(screen)
		}
	}
}

// DrawOverlay leaves the underlying scene intact for in-game menus.
func (menu *Menu) DrawOverlay(screen *ebiten.Image) {
	// Fade the panel and its contents together, without fading the background.
	if menu.layer == nil {
		menu.layer = ebiten.NewImage(data.ScreenWidth, data.ScreenHeight)
	}
	layer := menu.layer
	layer.Clear()
	menuRoundRect(layer, menu.Bounds, menu.Radius, menu.BodyColor, menu.BorderColor)
	list := layer.SubImage(menu.Bounds.Intersect(layer.Bounds())).(*ebiten.Image)
	spacing := menu.FontSize / 1.3
	centerX := float64(menu.Bounds.Min.X+menu.Bounds.Max.X) / 2
	centerY := float64(menu.Bounds.Min.Y+menu.Bounds.Max.Y) / 2
	if len(menu.Options) > 0 {
		if menu.Scroll {
			count := int(float64(menu.Bounds.Dy())/spacing/2) + 2
			for distance := -count; distance <= count; distance++ {
				index := menu.Selected + distance
				if menu.Wrap {
					index = (index%len(menu.Options) + len(menu.Options)) % len(menu.Options)
				} else if index < 0 || index >= len(menu.Options) {
					continue
				}
				if len(menu.Options) == 1 && distance != 0 {
					continue
				}
				y := centerY + (float64(distance)+menu.scrollOffset)*spacing
				menu.drawText(list, menu.Options[index].Name, centerX, y, menu.itemColor(distance))
			}
		} else {
			// A normal list does not wrap visually and only scrolls to keep
			// its current selection within the panel.
			first := max(0, menu.Selected-int(float64(menu.Bounds.Dy())/spacing)+1)
			for index := first; index < len(menu.Options); index++ {
				y := float64(menu.Bounds.Min.Y) + spacing*(float64(index-first)+.75)
				menu.drawText(list, menu.Options[index].Name, centerX, y, menu.itemColor(index-menu.Selected))
			}
		}
		info := menu.Options[menu.Selected].Info
		if menu.Notice != "" {
			info = menu.Notice
		}
		menu.drawText(layer, info, menu.InfoX, menu.InfoY, color.NRGBA{R: 255, G: 255, B: 255, A: 255})
	}
	menu.drawText(layer, menu.MenuInfo, menu.MenuInfoX, menu.MenuInfoY, color.NRGBA{R: 255, G: 255, B: 255, A: 255})
	var options ebiten.DrawImageOptions
	options.ColorScale.ScaleAlpha(float32(menu.opacity))
	screen.DrawImage(layer, &options)
}

func parseMenuAnimation(root *sexp.SExpr) *MenuAnimation {
	animation := &MenuAnimation{Images: make(map[int]*ebiten.Image), Paths: make(map[int]string), Clip: image.Rect(0, 0, 640, 480)}
	basedir := ""
	for _, node := range root.Children {
		switch node.Name {
		case "basedir":
			basedir = node.GetValue(0)
		case "image":
			animation.Paths[int(menuNumber(node, 0, 0))] = filepath.Join(basedir, node.GetValue(1))
		case "depth":
			if node.GetValue(0) == "foreground" {
				animation.Depth = 3
			}
			switch node.GetValue(1) {
			case "middle":
				animation.Depth++
			case "top":
				animation.Depth += 2
			}
		case "velocity":
			animation.VelocityX, animation.VelocityY = menuNumber(node, 0, 0)*320, menuNumber(node, 1, 0)*240
		case "axis":
			animation.AxisX, animation.AxisY = menuNumber(node, 0, 0)*320, menuNumber(node, 1, 0)*240
		case "window":
			animation.Clip = image.Rect(int(-menuNumber(node, 0, 0)*320), int(-menuNumber(node, 1, 0)*240), 640-int(menuNumber(node, 2, 0)*320), 480-int(menuNumber(node, 3, 0)*240)).Intersect(image.Rect(0, 0, 640, 480))
		case "frame":
			frame := MenuFrame{Image: int(menuNumber(node.GetChild("image"), 0, 0)), Time: int(menuNumber(node.GetChild("time"), 0, 1)), Alpha: uint8(max(0, min(255, menuNumber(node.GetChild("alpha"), 0, 255))))}
			frame.X, frame.Y = menuNumber(node.GetChild("offset"), 0, 0)*320, menuNumber(node.GetChild("offset"), 1, 0)*240
			frame.HFlip, frame.VFlip = menuNumber(node.GetChild("hflip"), 0, 0) != 0, menuNumber(node.GetChild("vflip"), 0, 0) != 0
			animation.Frames = append(animation.Frames, frame)
		}
	}
	return animation
}

func (animation *MenuAnimation) Update() {
	if len(animation.Frames) == 0 {
		return
	}
	animation.scrollX += animation.VelocityX
	animation.scrollY += animation.VelocityY
	frame := animation.Frames[animation.current]
	if frame.Time < 0 {
		return
	}
	animation.elapsed++
	if animation.elapsed >= max(1, frame.Time) {
		animation.current = (animation.current + 1) % len(animation.Frames)
		animation.elapsed = 0
	}
}

func (animation *MenuAnimation) Draw(screen *ebiten.Image) {
	if len(animation.Frames) == 0 || animation.Clip.Empty() {
		return
	}
	frame := animation.Frames[animation.current]
	bitmap := animation.Images[frame.Image]
	if bitmap == nil || frame.Alpha == 0 {
		return
	}
	target := screen.SubImage(animation.Clip.Intersect(screen.Bounds())).(*ebiten.Image)
	w, h := float64(bitmap.Bounds().Dx()), float64(bitmap.Bounds().Dy())
	sx, sy := math.Mod(animation.scrollX, w), math.Mod(animation.scrollY, h)
	xs, ys := []float64{0}, []float64{0}
	if sx != 0 {
		xs = []float64{sx, sx - math.Copysign(w, sx)}
	}
	if sy != 0 {
		ys = []float64{sy, sy - math.Copysign(h, sy)}
	}
	for _, x := range xs {
		for _, y := range ys {
			var options ebiten.DrawImageOptions
			if frame.HFlip {
				options.GeoM.Scale(-1, 1)
				options.GeoM.Translate(w, 0)
			}
			if frame.VFlip {
				options.GeoM.Scale(1, -1)
				options.GeoM.Translate(0, h)
			}
			options.GeoM.Translate(animation.AxisX+frame.X+x, animation.AxisY+frame.Y+y)
			options.ColorScale.ScaleAlpha(float32(frame.Alpha) / 255)
			target.DrawImage(bitmap, &options)
		}
	}
}
