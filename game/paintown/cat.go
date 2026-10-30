package paintown

import (
	"fmt"
	"log"
	"math/rand/v2"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/data"
	"github.com/kazzmir/paintown/game/lib/sexp"
)

const catMeowSound = "misc/cat/meow.wav"

// Cats are scenery, not enemies: they cannot attack, be grabbed, or block
// level progression. Each cat owns its animation playback state.
type Cat struct {
	X, Z       float64
	Facing     Facing
	State      string
	Animations map[string]*Animation
}

type CatFactory struct {
	animations map[string]map[string]*Animation
}

func (factory *CatFactory) Make(object BlockObject) (*Cat, error) {
	animations := factory.animations[object.Path]
	if animations == nil {
		definition, err := sexp.ReadSExpression(data.DataPath(object.Path))
		if err != nil {
			return nil, err
		}
		if definition.Name != "cat" {
			return nil, fmt.Errorf("%s: expected cat definition", object.Path)
		}
		animations = make(map[string]*Animation)
		for _, raw := range definition.FindAll("cat", "animation") {
			animation, err := MakeAnimationFromDefinition("", raw)
			if err != nil {
				return nil, fmt.Errorf("loading cat %s: %w", object.Path, err)
			}
			animations[animation.Name] = animation
		}
		for _, name := range []string{"idle1", "idle2", "yawn", "walk", "run", "turn", "sit"} {
			if animations[name] == nil {
				return nil, fmt.Errorf("cat %s: missing animation %s", object.Path, name)
			}
		}
		if factory.animations == nil {
			factory.animations = make(map[string]map[string]*Animation)
		}
		factory.animations[object.Path] = animations
	}
	cat := &Cat{X: float64(object.Coords.X), Z: float64(object.Coords.Y), Facing: FacingRight, State: "idle1", Animations: make(map[string]*Animation)}
	for name, animation := range animations {
		cat.Animations[name] = animation.Clone()
		cat.Animations[name].Reset()
	}
	cat.Animations[cat.State].Update(false, &dummySystem{})
	return cat, nil
}

func (cat *Cat) turn() {
	if cat.Facing == FacingRight {
		cat.Facing = FacingLeft
	} else {
		cat.Facing = FacingRight
	}
}

func (cat *Cat) Update(level *Level, limit float64, system System) {
	cat.update(level, limit, system, rand.N)
}

func (cat *Cat) update(level *Level, limit float64, system System, random func(int) int) {
	speed := 0.0
	switch cat.State {
	case "walk":
		speed = 0.8
	case "run":
		speed = 2.2
	}
	if speed != 0 {
		// The original cat sprites face opposite their direction of travel.
		if cat.Facing == FacingRight {
			cat.X -= speed
		} else {
			cat.X += speed
		}
		if random(5) == 0 {
			cat.Z -= 0.2
		}
		if random(5) == 0 {
			cat.Z += 0.2
		}
		if cat.X < 0 || cat.X > limit {
			cat.X = max(0, min(limit, cat.X))
			cat.turn()
		}
	}
	cat.Z = max(float64(level.ZMinimum), min(float64(level.ZMaximum), cat.Z))

	animation := cat.Animations[cat.State]
	if animation != nil && animation.Update(false, system) {
		switch cat.State {
		case "idle1", "idle2":
			switch random(5) {
			case 0:
				cat.State = "idle1"
			case 1:
				cat.State = "idle2"
			default:
				cat.State = "yawn"
				if random(2) == 0 {
					cat.turn()
				}
			}
		case "yawn":
			if random(6) < 2 {
				cat.State = "sit"
			} else {
				cat.State = "walk"
			}
		case "sit":
			if random(2) == 0 {
				cat.State = "idle1"
			} else {
				cat.State = "idle2"
			}
		case "walk":
			switch random(12) {
			case 0:
				cat.State = "sit"
			case 1, 2, 3:
				cat.State = "turn"
			case 4, 5:
				cat.State = "run"
			}
		case "turn":
			cat.turn()
			cat.State = "walk"
		case "run":
			if random(4) == 0 {
				cat.State = "walk"
			}
		}
		next := cat.Animations[cat.State]
		next.Reset()
		next.Update(false, system)
	}
	// Sound belongs in simulation updates, not drawing (which may run at a
	// different frequency). Match the original occasional meow probability.
	if random(2000) == 0 {
		if err := system.PlaySound(catMeowSound); err != nil {
			log.Printf("Error playing cat meow: %v", err)
		}
	}
}

func (cat *Cat) Draw(screen *ebiten.Image, cameraX float64, shake ebiten.GeoM) {
	animation := cat.Animations[cat.State]
	if animation == nil || animation.CurrentFrame() == nil {
		return
	}
	frame := animation.CurrentFrame()
	var options ebiten.DrawImageOptions
	options.GeoM.Translate(-float64(frame.Bounds().Dx())/2+float64(animation.GetOffsetX()), -float64(frame.Bounds().Dy())+float64(animation.GetOffsetY()))
	if cat.Facing == FacingLeft {
		options.GeoM.Scale(-1, 1)
	}
	options.GeoM.Translate(cat.X-cameraX, cat.Z)
	options.GeoM.Concat(shake)
	screen.DrawImage(frame, &options)
}

func (model *GameModel) CreateCats(objects []BlockObject, level *Level, blockX float64) {
	for _, object := range objects {
		if object.Type != "cat" {
			continue
		}
		cat, err := model.CatFactory.Make(object)
		if err != nil {
			log.Printf("Error creating cat '%s': %v", object.Name, err)
			continue
		}
		cat.X += blockX
		cat.Z += float64(level.ZMinimum)
		model.Cats = append(model.Cats, cat)
	}
}

func (model *GameModel) UpdateCats(level *Level, system System) {
	for _, cat := range model.Cats {
		cat.Update(level, model.LevelLimit, system)
	}
}
