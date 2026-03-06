package character

import (
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
)

// Command represents a single MUGEN command from .cmd
type Command struct {
	Name    string
	Command string
	Time    int
	Buffer  int
}

// CommandBuffer tracks input history to match commands
type CommandBuffer struct {
	History []string // Raw input names per tick (e.g., "holdfwd", "a")
	MaxSize int
}

func NewCommandBuffer(size int) *CommandBuffer {
	return &CommandBuffer{
		History: make([]string, 0, size),
		MaxSize: size,
	}
}

func (cb *CommandBuffer) Add(inputs []string) {
	cb.History = append(cb.History, strings.Join(inputs, ","))
	if len(cb.History) > cb.MaxSize {
		cb.History = cb.History[1:]
	}
}

// Match checks if a command string (like "D, DF, F, a") is matched in history
func (cb *CommandBuffer) Match(cmdStr string, maxTime int) bool {
	// Simple stub for matching.
	// Real MUGEN command matching is complex (time limits, directional tolerance).
	// For now, let's just check if the last few ticks contain the keys.

	// TODO: Full implementation of MUGEN command grammar
	return false
}

// GetRawInputs returns the current pressed keys as MUGEN input names
// TODO: Allow these to be overridden/configured at a later time.
func GetRawInputs() []string {
	var inputs []string

	// Directional keys
	if ebiten.IsKeyPressed(ebiten.KeyArrowRight) || ebiten.IsKeyPressed(ebiten.KeyRight) {
		inputs = append(inputs, "holdfwd")
	}
	if ebiten.IsKeyPressed(ebiten.KeyArrowLeft) || ebiten.IsKeyPressed(ebiten.KeyLeft) {
		inputs = append(inputs, "holdback")
	}
	if ebiten.IsKeyPressed(ebiten.KeyArrowUp) || ebiten.IsKeyPressed(ebiten.KeyUp) {
		inputs = append(inputs, "holdup")
	}
	if ebiten.IsKeyPressed(ebiten.KeyArrowDown) || ebiten.IsKeyPressed(ebiten.KeyDown) {
		inputs = append(inputs, "holddown")
	}

	// Button keys
	// Map asdzxc to x,y,z,a,b,c
	if ebiten.IsKeyPressed(ebiten.KeyA) {
		inputs = append(inputs, "x")
	}
	if ebiten.IsKeyPressed(ebiten.KeyS) {
		inputs = append(inputs, "y")
	}
	if ebiten.IsKeyPressed(ebiten.KeyD) {
		inputs = append(inputs, "z")
	}
	if ebiten.IsKeyPressed(ebiten.KeyZ) {
		inputs = append(inputs, "a")
	}
	if ebiten.IsKeyPressed(ebiten.KeyX) {
		inputs = append(inputs, "b")
	}
	if ebiten.IsKeyPressed(ebiten.KeyC) {
		inputs = append(inputs, "c")
	}

	return inputs
}

// IsButton returns true if the input name represents a button (not a direction)
func IsButton(name string) bool {
	switch strings.ToLower(name) {
	case "a", "b", "c", "x", "y", "z", "s", "start":
		return true
	}
	return false
}
