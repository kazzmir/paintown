package main

import (
	"fmt"
	"log"
	"path/filepath"

	"github.com/kazzmir/paintown/game/mugen/character"
	"github.com/kazzmir/paintown/game/mugen/cns"
)

func main() {
	fmt.Println("--- PHYSICS TEST START ---")
	dataDir := "data-new/mugen"
	root := dataDir

	// cfg := &config.MugenConfig{}
	// input.GlobalManager = input.NewInputManager(cfg)

	// Load KFM
	baseDir := filepath.Join(root, "chars/kfm")
	p1, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load p1: %v", err)
	}
	p2, err := character.LoadPlayer(baseDir, "kfm.def")
	if err != nil {
		log.Fatalf("failed to load p2: %v", err)
	}

	p2.Character.PlayerID = 2
	p2.Character.Target = []*character.Character{p1.Character}
	p2.Character.ChangeState(0, -1, -1)

	fmt.Println("Starting Console Physics Test...")

	// Tick 0-59: Idle
	for i := 0; i < 60; i++ {
		p2.Update()
	}

	fmt.Printf("Tick %d: P2 State: %d, Y: %.2f\n", 60, p2.Character.GetStateNo(), p2.Character.Y)

	// Tick 60: Hit by Uppercut
	fmt.Println("--- TRIGGERING UP CUT HIT ---")
	hit := cns.HitDef{
		Attr:          "S, HA",
		Fall:          1,
		GroundVel:     []float64{1.0, -10.5},
		AirVel:        []float64{1.0, -10.5},
		GroundHitTime: 20,
		AirHitTime:    20,
		PauseTime:     []int{5, 5}, // Shorter pause for test
		YAccel:        0.4,
	}
	p2.Character.ProcessHit(p1.Character, hit)

	// Tick forward and observe
	for i := 61; i < 600; i++ {
		p2.Update()

		// Simulate Air Recovery at Tick 100
		if i == 100 {
			fmt.Println("--- SIMULATING AIR RECOVERY (State 5210) ---")
			p2.Character.ChangeState(5210, -1, -1)
		}

		state := p2.Character.GetStateNo()
		if i%10 == 0 || state != p2.Character.GetStateNo() {
			// We'll print on state changes or every 10 ticks once hit
			if i >= 60 {
				fmt.Printf("Tick %d: State: %d, Y: %.2f, VelY: %.2f, Shake: %d, Freeze: %v, Fall: %v\n",
					i, state, p2.Character.Y, p2.Character.VelY, p2.Character.ShakeTime, p2.Character.PosFrozen, p2.Character.HitFall)
			}
		}

		// Break if we returned to state 0 on ground
		if i > 100 && state == 0 && p2.Character.Y >= 0 {
			fmt.Printf("Tick %d: P2 reached Ground in State 0. Test Success!\n", i)
			break
		}
	}

	// Jump Test
	fmt.Println("--- TRIGGERING JUMP (State 40) ---")
	p2.Character.ChangeState(40, -1, -1)
	// Mock velocity like State 40 would
	p2.Character.VelY = -8.5

	for i := 600; i < 900; i++ {
		p2.Update()
		state := p2.Character.GetStateNo()
		if i%10 == 0 || state != p2.Character.GetStateNo() {
			fmt.Printf("Tick %d: Jump State: %d, Y: %.2f, VelY: %.2f, Freeze: %v\n",
				i, state, p2.Character.Y, p2.Character.VelY, p2.Character.PosFrozen)
		}
		if i > 650 && state == 0 && p2.Character.Y >= 0 {
			fmt.Printf("Tick %d: Jump Landing Success!\n", i)
			break
		}
	}
}
