package paintown

import (
	"image"
	"image/color"
	"math"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/lib/sexp"
)

type creditLine struct {
	text  string
	title bool
	gap   float64
}

type creditsView struct {
	lines                 []creditLine
	offset, speed         float64
	pause                 int
	atEnd                 bool
	titleColor, textColor color.NRGBA
}

func (credits *creditsView) block(title string, names []string, gap float64) {
	credits.lines = append(credits.lines, creditLine{text: title, title: true})
	for _, name := range names {
		credits.lines = append(credits.lines, creditLine{text: name})
	}
	credits.lines = append(credits.lines, creditLine{gap: gap})
}

func makeCreditsMenu(parent *Menu, definition *sexp.SExpr) (*Menu, error) {
	menu := *parent
	menu.Name, menu.MenuInfo, menu.Notice = "Credits", "", ""
	menu.Options, menu.Animations = nil, nil
	menu.Selected, menu.ticks = 0, 0
	menu.opacity, menu.scrollOffset = 0, 0
	menu.layer, menu.background = nil, nil
	menu.BackgroundPath = ""
	menu.Bounds = image.Rect(40, 65, 600, 415)
	menu.ClearColor = color.NRGBA{A: 255}
	menu.BodyColor = color.NRGBA{A: 180}
	credits := &creditsView{speed: .5, pause: arcadeRespawnDelay() * 2, titleColor: color.NRGBA{G: 255, B: 255, A: 255}, textColor: color.NRGBA{R: 255, G: 255, B: 255, A: 255}}
	menu.credits = credits
	// Static contributors match the original engine's built-in credits.
	credits.block("Paintown", nil, 0)
	credits.block("Programming", []string{"Jon Rafkind", "Miguel Gavidia"}, 0)
	credits.block("Level design", []string{"Jon Rafkind", "Miguel Gavidia"}, 0)
	credits.block("Contact", []string{"Website: http://paintown.org", "Email: jon@rafkind.com"}, 0)
	if definition != nil {
		credits.titleColor = menuColor(definition.GetChild("titlecolor"), credits.titleColor)
		credits.textColor = menuColor(definition.GetChild("color"), credits.textColor)
		menu.ClearColor = menuColor(definition.GetChild("clear-color"), menu.ClearColor)
		for _, child := range definition.Children {
			switch child.Name {
			case "anim":
				menu.Animations = append(menu.Animations, parseMenuAnimation(child))
			case "sequence":
				credits.speed = max(.1, math.Abs(menuNumber(child.GetChild("speed"), 0, credits.speed)))
				for _, block := range child.Children {
					if block.Name != "block" {
						continue
					}
					var names []string
					for _, line := range block.Children {
						if line.Name == "credit" {
							names = append(names, menuText(line))
						}
					}
					credits.block(menuText(block.GetChild("title")), names, max(0, menuNumber(block.GetChild("spacing"), 0, 0)))
				}
			}
		}
	}
	if err := menu.loadAssets(); err != nil {
		return nil, err
	}
	return &menu, nil
}

func (credits *creditsView) maxOffset(menu *Menu) float64 {
	height := float64(len(credits.lines)) * menu.listSpacing()
	for _, line := range credits.lines {
		height += line.gap
	}
	return max(0, height-float64(menu.Bounds.Dy()))
}

func (credits *creditsView) move(menu *Menu, direction int) bool {
	next := max(0, min(credits.maxOffset(menu), credits.offset+float64(direction)*menu.listSpacing()*2))
	if next == credits.offset {
		return false
	}
	credits.offset, credits.pause, credits.atEnd = next, arcadeRespawnDelay()*2, false
	return true
}

func (credits *creditsView) update(menu *Menu) {
	if credits.pause > 0 {
		credits.pause--
		return
	}
	if credits.atEnd {
		credits.offset, credits.atEnd, credits.pause = 0, false, arcadeRespawnDelay()*2
		return
	}
	maximum := credits.maxOffset(menu)
	if maximum <= 0 {
		return
	}
	credits.offset = min(maximum, credits.offset+credits.speed)
	if credits.offset == maximum {
		credits.atEnd, credits.pause = true, arcadeRespawnDelay()*3
	}
}

func (credits *creditsView) draw(menu *Menu, screen *ebiten.Image) {
	menuRoundRect(screen, menu.Bounds, menu.Radius, menu.BodyColor, menu.BorderColor)
	clipped := screen.SubImage(menu.Bounds.Intersect(screen.Bounds())).(*ebiten.Image)
	spacing := menu.listSpacing()
	y := float64(menu.Bounds.Min.Y) + spacing/2 - credits.offset
	for _, line := range credits.lines {
		tint := credits.textColor
		if line.title {
			tint = credits.titleColor
		}
		menu.drawText(clipped, line.text, 320, y, tint)
		y += spacing + line.gap
	}
	menu.drawText(screen, "Credits", 320, 28, credits.titleColor)
	menu.drawText(screen, "Up / Down: scroll   Escape / Circle: back", 320, 450, credits.textColor)
}
