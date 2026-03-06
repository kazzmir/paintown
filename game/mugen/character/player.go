package character

import (
	"fmt"
	"image/color"
	"os"
	"path/filepath"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

// Player is a high-level container that combines state, sprites, animations, and commands.
type Player struct {
	Character *Character
	SFF       *sff.SFF
	AIR       *air.Data
	CMD       *cmd.Data

	// Path to the character's directory
	Dir string

	// Sprite cache for Ebiten
	spriteCache map[int]map[int]*ebiten.Image

	// Command buffering
	Commands *CommandBuffer
}

// LoadPlayer initializes a Player by loading all associated files.
// baseDir should be something like "chars/kfm/"
// defFile is the .def file name, e.g., "kfm.def"
func LoadPlayer(baseDir, defFile string) (*Player, error) {
	// For now, we assume standard naming or we'd parse the .def file.
	// Since we don't have a .def parser yet, let's hardcode or assume names.

	// TODO: Implement .def parser to get these paths.
	// For KFM, they are kfm.sff, kfm.air, kfm.cns, kfm.cmd

	name := filepath.Base(defFile)
	name = name[:len(name)-len(filepath.Ext(name))]

	sffPath := filepath.Join(baseDir, name+".sff")
	airPath := filepath.Join(baseDir, name+".air")
	cnsPath := filepath.Join(baseDir, name+".cns")
	cmdPath := filepath.Join(baseDir, name+".cmd")

	p := &Player{
		Dir:         baseDir,
		spriteCache: make(map[int]map[int]*ebiten.Image),
		Commands:    NewCommandBuffer(120), // 2 seconds of history @ 60fps
	}

	// 0. Try to find a palette (.act)
	// Usually characters have a palette named kfm.act or similar
	var initialPalette color.Palette
	actPath := filepath.Join(baseDir, name+".act")
	if _, err := os.Stat(actPath); err != nil {
		// Fallback to searching for ANY .act file in the dir
		files, _ := os.ReadDir(baseDir)
		for _, f := range files {
			if strings.HasSuffix(strings.ToLower(f.Name()), ".act") {
				actPath = filepath.Join(baseDir, f.Name())
				break
			}
		}
	}

	if actFile, err := os.Open(actPath); err == nil {
		if pal, err := sff.ReadPaletteACT(actFile, true); err == nil {
			initialPalette = pal
		}
		actFile.Close()
	} else {
	}

	// 1. Load CNS
	cnsFile, err := os.Open(cnsPath)
	if err != nil {
		return nil, fmt.Errorf("open cns: %w", err)
	}
	defer cnsFile.Close()
	parsedCNS, err := cns.Parse(cnsFile)
	if err != nil {
		return nil, fmt.Errorf("parse cns: %w", err)
	}
	p.Character = NewCharacter(parsedCNS, p.AIR)
	p.Character.Commands = p.Commands

	// 2. Load SFF
	sffFile, err := os.Open(sffPath)
	if err != nil {
		return nil, fmt.Errorf("open sff: %w", err)
	}
	defer sffFile.Close()
	p.SFF, err = sff.ParseWithPalette(sffFile, initialPalette)
	if err != nil {
		return nil, fmt.Errorf("parse sff: %w", err)
	}

	// 3. Load AIR
	airFile, err := os.Open(airPath)
	if err != nil {
		return nil, fmt.Errorf("open air: %w", err)
	}
	defer airFile.Close()
	p.AIR, err = air.Parse(airFile)
	if err != nil {
		return nil, fmt.Errorf("parse air: %w", err)
	}
	p.Character.AirFile = p.AIR

	// 4. Load CMD (for commands and State -1)
	cmdFile, err := os.Open(cmdPath)
	if err != nil {
		return nil, fmt.Errorf("open cmd: %w", err)
	}
	defer cmdFile.Close()
	p.CMD, err = cmd.Parse(cmdFile)
	if err != nil {
		return nil, fmt.Errorf("parse cmd: %w", err)
	}

	// Also parse CMD with CNS parser to get [State -1] blocks
	cmdFile.Seek(0, 0)
	cmdStates, err := cns.Parse(cmdFile)
	if err == nil {
		for id, state := range cmdStates.States {
			p.Character.StateFile.States[id] = state
		}
	}

	// 5. Load common1.cns (Common states)
	// Try local first, then global data/
	commonPath := filepath.Join(baseDir, "common1.cns")
	if _, err := os.Stat(commonPath); err != nil {
		// Try global data directory relative to character
		// Structure is: chars/kfm/ and data/
		// From data-new/mugen/chars/kfm/ we need to go up to data-new/mugen/ and then to data/
		absBase, _ := filepath.Abs(baseDir)
		commonPath = filepath.Join(filepath.Dir(filepath.Dir(absBase)), "data", "common1.cns")
	}

	if commonFile, err := os.Open(commonPath); err == nil {
		commonCNS, err := cns.Parse(commonFile)
		if err == nil {
			for id, state := range commonCNS.States {
				// Don't overwrite states if already defined in character's CNS/CMD
				if _, ok := p.Character.StateFile.States[id]; !ok {
					p.Character.StateFile.States[id] = state
				}
			}
		}
		commonFile.Close()
	} else {
	}

	return p, nil
}

// Update advances the player's state.
func (p *Player) Update() {
	if p.Character.Time == 0 {
		fmt.Printf("Player.Update: Starting Tick 0\n")
	}
	// 1. Get raw inputs and add to buffer
	raw := GetRawInputs()
	p.Commands.Add(raw)

	// 2. (Removed redundant AnimDuration sync as it's now handled by ChangeAnim)

	// 3. Tick character state machine
	p.Character.Update()
}

// Draw renders the player to the screen.
func (p *Player) Draw(screen *ebiten.Image, camX, camY float64) {
	animNo := p.Character.GetAnim()
	animData, ok := p.AIR.Actions[animNo]
	if !ok {
		return
	}

	var currentElement *air.Element

	// Check for total animation time to handle loops
	fullCycle := 0
	for _, el := range animData.Elements {
		if el.Time == -1 {
			fullCycle = -1 // Infinite
			break
		}
		fullCycle += el.Time
	}

	currentTime := p.Character.GetAnimTime()
	if fullCycle > 0 {
		currentTime = currentTime % fullCycle
	}

	elapsed := 0
	for i := range animData.Elements {
		el := &animData.Elements[i]
		if el.Time == -1 || (elapsed <= currentTime && currentTime < elapsed+el.Time) {
			currentElement = el
			break
		}
		elapsed += el.Time
	}

	if currentElement == nil {
		return
	}

	sprite := p.FindSprite(currentElement.Group, currentElement.Image)
	if sprite == nil || sprite.Image == nil {
		return
	}

	// Calculate final draw position
	drawX := p.Character.X - camX + float64(currentElement.XOffset) - float64(sprite.XAxis)
	drawY := p.Character.Y - camY + float64(currentElement.YOffset) - float64(sprite.YAxis)

	// Convert image.Image to *ebiten.Image using cache
	if p.spriteCache[currentElement.Group] == nil {
		p.spriteCache[currentElement.Group] = make(map[int]*ebiten.Image)
	}
	ebitenSprite, cached := p.spriteCache[currentElement.Group][currentElement.Image]
	if !cached {
		ebitenSprite = ebiten.NewImageFromImage(sprite.Image)
		p.spriteCache[currentElement.Group][currentElement.Image] = ebitenSprite
	}

	opts := &ebiten.DrawImageOptions{}
	opts.GeoM.Translate(drawX, drawY)
	screen.DrawImage(ebitenSprite, opts)
}

func (p *Player) FindSprite(group, image int) *sff.Sprite {
	for i := range p.SFF.Sprites {
		s := &p.SFF.Sprites[i]
		if int(s.GroupNumber) == group && int(s.ImageNumber) == image {
			return s
		}
	}
	return nil
}
