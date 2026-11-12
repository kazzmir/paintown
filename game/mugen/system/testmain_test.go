package main

import (
	"fmt"
	"os"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
)

// Production drawing tests need GPU readback inside the public game loop, as
// in Ebitengine's own image tests and this port's render package tests.
type systemTestGame struct {
	tests *testing.M
	code  int
}

func (g *systemTestGame) Update() error {
	g.code = g.tests.Run()
	return ebiten.Termination
}
func (*systemTestGame) Draw(*ebiten.Image)         {}
func (*systemTestGame) Layout(int, int) (int, int) { return 32, 32 }

func TestMain(m *testing.M) {
	g := &systemTestGame{tests: m, code: 1}
	ebiten.SetWindowSize(32, 32)
	ebiten.SetWindowTitle("MUGEN production renderer tests")
	if err := ebiten.RunGame(g); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	os.Exit(g.code)
}
