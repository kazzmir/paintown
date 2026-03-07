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
	"github.com/kazzmir/paintown/game/mugen/parsers"
	"github.com/kazzmir/paintown/game/mugen/sff"
)

var commonCNSCache = make(map[string]*cns.CNS)

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

	// Player ID (1 or 2)
	PlayerID int
}

// LoadPlayer initializes a Player by loading all associated files.
// baseDir should be something like "chars/kfm/"
// defFile is the .def file name, e.g., "kfm.def"
func LoadPlayer(baseDir, defFile string) (*Player, error) {
	defPath := filepath.Join(baseDir, defFile)
	defFileObj, err := os.Open(defPath)
	if err != nil {
		return nil, fmt.Errorf("open def: %w", err)
	}
	defer defFileObj.Close()
	defData, err := parsers.ParseDef(defFileObj)
	if err != nil {
		return nil, fmt.Errorf("parse def: %w", err)
	}

	p := &Player{
		Dir:         baseDir,
		spriteCache: make(map[int]map[int]*ebiten.Image),
		Commands:    NewCommandBuffer(120),
		PlayerID:    1,
	}

	// 0. Extract paths from defData
	sffFile := defData.Files["sprite"]
	if sffFile == "" {
		sffFile = defData.Files["sff"]
	}
	airFile := defData.Files["anim"]
	if airFile == "" {
		airFile = defData.Files["air"]
	}
	cmdFile := defData.Files["cmd"]
	cnsFile := defData.Files["cns"]
	commonFile := defData.Files["stcommon"]
	if commonFile == "" {
		commonFile = defData.Files["common1"]
	}

	// Collect all ST files (st, st0, st1, ...)
	var stFiles []string
	if st, ok := defData.Files["st"]; ok && st != "" {
		stFiles = append(stFiles, st)
	}
	for i := 0; i < 100; i++ {
		key := fmt.Sprintf("st%d", i)
		if st, ok := defData.Files[key]; ok && st != "" {
			stFiles = append(stFiles, st)
		} else if i > 0 {
			// Usually sequential, but let's be safe and check a few more?
			// MUGEN usually doesn't have gaps.
			if i > 5 {
				break
			}
		}
	}

	name := filepath.Base(defFile)
	name = name[:len(name)-len(filepath.Ext(name))]

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

	// 1. Load CMD (needed for Character's Command definitions and State -1)
	cmdPath := filepath.Join(baseDir, cmdFile)
	cmdF, err := os.Open(cmdPath)
	if err != nil {
		return nil, fmt.Errorf("open cmd %s: %w", cmdPath, err)
	}
	defer cmdF.Close()
	p.CMD, err = cmd.Parse(cmdF)
	if err != nil {
		return nil, fmt.Errorf("parse cmd: %w", err)
	}

	// Also parse CMD with CNS parser to get [State -1] blocks
	cmdF.Seek(0, 0)
	cmdStates, _ := cns.Parse(cmdF)

	// 2. Load all CNS/ST files
	masterCNS := &cns.CNS{
		States: make(map[int]*cns.StateDef),
	}

	// Load main CNS first (for constants)
	if cnsFile != "" {
		cnsPath := filepath.Join(baseDir, cnsFile)
		if f, err := os.Open(cnsPath); err == nil {
			parsed, err := cns.Parse(f)
			if err == nil {
				masterCNS.Data = parsed.Data
				masterCNS.Size = parsed.Size
				masterCNS.Velocity = parsed.Velocity
				masterCNS.Movement = parsed.Movement
				for id, state := range parsed.States {
					masterCNS.States[id] = state
				}
			}
			f.Close()
		}
	}

	// Load all ST files and merge
	for _, stName := range stFiles {
		stPath := filepath.Join(baseDir, stName)
		if f, err := os.Open(stPath); err == nil {
			parsed, err := cns.Parse(f)
			if err == nil {
				for id, state := range parsed.States {
					masterCNS.States[id] = state
				}
			}
			f.Close()
		}
	}

	// Merge State -1 blocks from CMD
	if cmdStates != nil {
		for id, state := range cmdStates.States {
			masterCNS.States[id] = state
		}
	}

	p.Character = NewCharacter(masterCNS, nil, p.CMD)
	p.Character.Commands = p.Commands

	// 3. Load SFF
	sffPath := filepath.Join(baseDir, sffFile)
	sffF, err := os.Open(sffPath)
	if err != nil {
		return nil, fmt.Errorf("open sff %s: %w", sffPath, err)
	}
	defer sffF.Close()
	p.SFF, err = sff.ParseWithPalette(sffF, initialPalette)
	if err != nil {
		return nil, fmt.Errorf("parse sff: %w", err)
	}

	// 4. Load AIR
	airPath := filepath.Join(baseDir, airFile)
	airF, err := os.Open(airPath)
	if err != nil {
		return nil, fmt.Errorf("open air %s: %w", airPath, err)
	}
	defer airF.Close()
	p.AIR, err = air.Parse(airF)
	if err != nil {
		return nil, fmt.Errorf("parse air: %w", err)
	}
	p.Character.AirFile = p.AIR

	// 4. Merge common states (common1.cns)
	commonPath := ""
	cleanBase := filepath.Clean(baseDir)
	if commonFile != "" {
		commonPath = filepath.Join(baseDir, commonFile)
		// If not found in character dir, try system data dir
		if _, err := os.Stat(commonPath); os.IsNotExist(err) {
			// Try looking in ../../data/ (relative to chars/kfm/)
			commonPath = filepath.Join(filepath.Dir(filepath.Dir(cleanBase)), "data", commonFile)
		}
	} else {
		// Default fallback
		commonPath = filepath.Join(filepath.Dir(filepath.Dir(cleanBase)), "data", "common1.cns")
	}

	if parsed, ok := commonCNSCache[commonPath]; ok {
		for id, state := range parsed.States {
			if _, ok := p.Character.StateFile.States[id]; !ok {
				p.Character.StateFile.States[id] = state
			}
		}
	} else if f, err := os.Open(commonPath); err == nil {
		parsed, err := cns.Parse(f)
		if err == nil && parsed != nil {
			commonCNSCache[commonPath] = parsed
			for id, state := range parsed.States {
				if _, ok := p.Character.StateFile.States[id]; !ok {
					p.Character.StateFile.States[id] = state
				}
			}
		}
		f.Close()
	}

	return p, nil
}

// Update advances the player's state.
func (p *Player) Update() {
	/*
		if p.Character.Time == 0 {
			fmt.Printf("Player.Update: Starting Tick 0\n")
		}
	*/
	// 1. (Removed redundant CommandBuffer.Add as Character.Update now handles it)

	// 2. (Removed redundant AnimDuration sync as it's now handled by ChangeAnim)
	// 3. Tick character state machine
	p.Character.Update()
}

// Draw renders the player to the screen at the specified screen coordinates.
func (p *Player) Draw(screen *ebiten.Image, screenX, screenY float64) {
	p.DrawScaled(screen, screenX, screenY, 1.0, 1.0)
}

// DrawScaled renders the player to the screen at the specified screen coordinates with a specific scale.
func (p *Player) DrawScaled(screen *ebiten.Image, screenX, screenY, scaleX, scaleY float64) {
	animNo := p.Character.GetAnim()
	animData, ok := p.AIR.Actions[animNo]
	if !ok {
		return
	}

	var currentElement *air.Element
	if p.Character.AnimElem < len(animData.Elements) {
		currentElement = &animData.Elements[p.Character.AnimElem]
	}

	if currentElement == nil {
		return
	}

	sprite := p.FindSprite(currentElement.Group, currentElement.Image)
	if sprite == nil || sprite.Image == nil {
		return
	}

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

	// 1. Pivot translation (move "feet/center" to 0,0)
	opts.GeoM.Translate(-float64(sprite.XAxis), -float64(sprite.YAxis))

	// 2. Element offsets (air displacement)
	opts.GeoM.Translate(float64(currentElement.XOffset), float64(currentElement.YOffset))

	// 3. Scaling
	opts.GeoM.Scale(scaleX, scaleY)

	// 4. Final translation to specified screen position
	opts.GeoM.Translate(screenX, screenY)

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
