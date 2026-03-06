package character

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/input"
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
func GetRawInputs(player int) []string {
	var inputs []string

	if input.GlobalManager == nil {
		return inputs
	}

	// Directional keys
	if input.GlobalManager.IsPressed(player, input.ActionRight) {
		inputs = append(inputs, "holdfwd")
	}
	if input.GlobalManager.IsPressed(player, input.ActionLeft) {
		inputs = append(inputs, "holdback")
	}
	if input.GlobalManager.IsPressed(player, input.ActionJump) {
		inputs = append(inputs, "holdup")
	}
	if input.GlobalManager.IsPressed(player, input.ActionCrouch) {
		inputs = append(inputs, "holddown")
	}

	// Button keys
	if input.GlobalManager.IsPressed(player, input.ActionX) {
		inputs = append(inputs, "x")
	}
	if input.GlobalManager.IsPressed(player, input.ActionY) {
		inputs = append(inputs, "y")
	}
	if input.GlobalManager.IsPressed(player, input.ActionZ) {
		inputs = append(inputs, "z")
	}
	if input.GlobalManager.IsPressed(player, input.ActionA) {
		inputs = append(inputs, "a")
	}
	if input.GlobalManager.IsPressed(player, input.ActionB) {
		inputs = append(inputs, "b")
	}
	if input.GlobalManager.IsPressed(player, input.ActionC) {
		inputs = append(inputs, "c")
	}
	if input.GlobalManager.IsPressed(player, input.ActionStart) {
		inputs = append(inputs, "start")
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
